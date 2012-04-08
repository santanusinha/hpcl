#include <iostream>
#include <fstream>

#include "logging_component_factory.h"
#include "remote_log_server.h"
#include "message_client.h"
#include "message_component_factory.h"


#include "ports.h"
#include "messages.h"

using namespace std;
using namespace Hpcl;

class LogServer {
    public:
        void
        log_received( const std::string &in_message ) {
            std::ofstream fout( m_filename.c_str(), std::ios::out
                                                    | std::ios::app );
            fout<<in_message;
            fout.close();
        }

        void
        run() {
            m_messenger = MessageComponentFactory::create_factory()
                                                    ->create_client();
            m_messenger->signal_message_received().connect(
                    boost::bind(
                        std::mem_fn( &LogServer::handle_server_commands),
                        this, _1));
            m_messenger->connect( "localhost", Ports::MESSAGE_PORT);
            m_log_server = LoggingComponentFactory::create_factory()
                                        ->create_server();
            m_log_server->signal_log_message_received().connect(
                    boost::bind(
                        std::mem_fn( &LogServer::log_received ),
                        this, _1));
            m_log_thread = std::make_shared<std::thread>( boost::bind(
                                std::mem_fn( &RemoteLogServer::listen ),
                                m_log_server, Ports::LOGD_PORT,
                                m_log_server_error ) );
            MessageLogStartComplete log_started;
            m_messenger->send_message( log_started );
            std::unique_lock<std::mutex> l( m_log_mutex );
            while( !m_stop )
            {
                m_log_event.wait( l );
            }
            if( m_log_server ) {
                m_log_server->stop();
            }
            if( m_log_thread ) {
                m_log_thread->join();
            }
            MessageLogdShutdownCompleted completed;
            m_messenger->send_message( completed );
            m_messenger->disconnect();
        }

        void
        handle_server_commands( const MemInfo &in_msg ) {
            const MessageHeader *header
                                    = reinterpret_cast<const MessageHeader *>(
                                                        in_msg.get_data() );
            switch( header->m_type ) {
                case MESSAGE_LOGD_SHUTDOWN:
                {
                    std::lock_guard<std::mutex> l( m_log_mutex );
                    m_stop = true;
                    m_log_event.notify_one();
                    break;
                }
                default:
                {
                }
            }
        }

        LogServer( const std::string &in_filename )
            :m_log_server(),
            m_log_thread(),
            m_log_server_error(),
            m_filename( in_filename ),
            m_file_mutex(),
            m_log_mutex(),
            m_log_event(),
            m_stop() {
        }

        ~LogServer() {
        }

    private:
        MessageClientPtr m_messenger;
        RemoteLogServerPtr m_log_server;
        ThreadPtr m_log_thread;
        std::exception_ptr m_log_server_error;
        std::string m_filename;
        std::mutex m_file_mutex;
        std::mutex m_log_mutex;
        std::condition_variable m_log_event;
        bool m_stop;
};


int main() {
    LogServer server("log.txt");
    server.run();
    return 0;
}

