#include <atomic>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <map>
#include <set>
#include <thread>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <boost/exception/all.hpp>

#include "messages.h"
#include "remote_process.h"
#include "remote_exec_component_factory.h"
#include "message_server.h"
#include "message_component_factory.h"
#include "message_client.h"

#include "ports.h"

using namespace Hpcl;

class Controller {

    public:
        Controller()
            :m_exec_factory(
                    RemoteExecComponentFactory::create_factory() ),
            m_msg_factory(
                    MessageComponentFactory::create_factory() ),
            m_server( m_msg_factory->create_message_server() ),
            m_server_error(),
            m_log_server_proc(),
            m_log_messenger(),
            m_event_mutex(),
            m_event(),
            m_stop(),
            m_process_count_mutex(),
            m_process_count() {
        }

        void
        run() {
            m_server->signal_message_received().connect( boost::bind(
                        std::mem_fn( &Controller::handle_client_message ),
                        this, _1, _2 ));
            auto t = std::make_shared<std::thread>( boost::bind(
                        std::mem_fn( &MessageServer::listen ),
                        m_server, Ports::MESSAGE_PORT, std::ref( m_server_error ) ) );
            m_log_server_proc = m_exec_factory->create_remote_process();
            m_log_server_proc->signal_process_started().connect( boost::bind(
                            std::mem_fn( &Controller::handle_process_start ),
                            this, _1, _2));
            m_log_server_proc->connect("localhost", Ports::EXECD_PORT);
            m_log_server_proc->execute("./logd");
            std::unique_lock< std::mutex > l( m_event_mutex );
            while( !m_stop ) {
                m_event.wait(l);
            }
            m_server->stop();
            t->join();
            return;
        }

        void
        handle_process_start( pid_t in_pid,
                            const RemoteProcessPtr &in_process ) {
            std::cout<<"Remote process started with pid "<<in_pid
                        <<std::endl;
        }

        void
        handle_process_stop( pid_t in_pid, int32_t in_status ) {
            std::lock_guard<std::mutex> l( m_process_count_mutex );
            m_process_count--;
            std::cout<<"Child process "<<in_pid<<" has completed with "
                    <<in_status<<std::endl;
            if( 0 == m_process_count )
            {
                std::cout<<"All clients completed"<<std::endl;
                if( m_log_messenger ) {
                    MessageLogdShutdown log_stop;
                    m_log_messenger->send_message( log_stop );
                    std::cout<<"log shutdown requested"<<std::endl;
                }
            }
        }

        void
        handle_client_message( const MemInfo &in_msg,
                                const MessageClientPtr &in_client ) {
            const MessageHeader *header
                                = reinterpret_cast<const MessageHeader *>(
                                                in_msg.get_data() );
            switch( header->m_type ) {
                case MessageType::MESSAGE_LOGD_START_COMPLETE:
                {
                    std::cout<<"Log started"<<std::endl;
                    m_log_messenger = in_client;
                    RemoteProcessPtr process1 = m_exec_factory->create_remote_process();
                    process1->signal_process_started().connect( boost::bind(
                                std::mem_fn( &Controller::handle_process_start ),
                                this, _1, _2));
                    process1->signal_process_completed().connect( boost::bind(
                                std::mem_fn( &Controller::handle_process_stop ),
                                this, _1, _2));
                    RemoteProcessPtr process2 = m_exec_factory->create_remote_process();
                    process2->signal_process_started().connect( boost::bind(
                                std::mem_fn( &Controller::handle_process_start ),
                                this, _1, _2));
                    process2->signal_process_completed().connect( boost::bind(
                                std::mem_fn( &Controller::handle_process_stop ),
                                this, _1, _2));
                    m_agents.insert( process1 );
                    m_agents.insert( process2 );
                    {
                        std::lock_guard<std::mutex> l( m_process_count_mutex );
                        m_process_count = 2;
                    }
                    process1->connect("localhost", Ports::EXECD_PORT);
                    process2->connect("localhost", Ports::EXECD_PORT);
                    process1->execute("./agent");
                    process2->execute("./agent");
                    break;
                }
                case MessageType::MESSAGE_LOGD_SHUTDOWN_COMPLETED:
                {
                    std::cout<<"Log stopped"<<std::endl;
                    std::lock_guard<std::mutex> l(m_event_mutex);
                    m_stop = true;
                    m_event.notify_one();
                    break;
                }
                case MessageType::MESSAGE_INIT:
                {
                    std::cout<<"Client ready .. sending message: ";
                    MessageData data;
                    ::strcpy( data.msg, "Hello World\n" );
                    in_client->send_message( data );
                    std::cout<<"Done"<<std::endl;
                    break;
                }
                case MessageType::MESSAGE_COMPLETED:
                {
                    std::lock_guard<std::mutex> l( m_process_count_mutex );
                    std::cout<<"Client completed"<<std::endl;
                    break;
                }
                default:
                {
                }
            }
            return;
        }


    private:
        RemoteExecComponentFactoryPtr m_exec_factory;
        MessageComponentFactoryPtr m_msg_factory;
        MessageServerPtr m_server;
        std::exception_ptr m_server_error;
        RemoteProcessPtr m_log_server_proc;
        MessageClientPtr m_log_messenger;
        std::set<RemoteProcessPtr> m_agents;
        std::mutex m_event_mutex;
        std::condition_variable m_event;
        bool m_stop;
        std::mutex m_process_count_mutex;
        uint32_t m_process_count;
};

int main()
try {
    Controller controller;
    controller.run();
    return 0;
}
catch( boost::exception &error ) {
    int const *errorNo
                = boost::get_error_info<boost::errinfo_errno>( error );
    std::cerr<<"Error occurred [" <<strerror(*errorNo) <<"]"<<std::endl;
    return -1;
}
catch( std::exception &e ) {
    std::cerr<<"Error occurred [" <<e.what() <<"]"<<std::endl;
    return -1;
}

