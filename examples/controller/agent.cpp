#include <sstream>

#include "logging_component_factory.h"
#include "message_client.h"
#include "message_component_factory.h"
#include "remote_logger.h"

#include "messages.h"
#include "ports.h"

using namespace Hpcl;

std::mutex g_mutex;
std::condition_variable g_event;
bool g_stop = false;


class ClientFSM {
    public:

        void
        handle_server_request( const MemInfo &in_msg ) {
           const MessageHeader *header
                                = reinterpret_cast<const MessageHeader *>(
                                                in_msg.get_data() );
            switch( header->m_type ) {
                case MessageType::MESSAGE_DATA:
                {
                    const MessageData *data
                                = reinterpret_cast<const MessageData *>(
                                                in_msg.get_data());
                    std::ostringstream sout;
                    sout<<"AGENT("<<getpid()<<"): "<<data->msg;
                    m_logger->log(sout.str());
                    m_logger->disconnect();
                    MessageCompleted completed;
                    m_client->send_message( completed );
                    std::lock_guard<std::mutex> l(g_mutex);
                    g_stop = true;
                    g_event.notify_one();
                    break;
                }
                default:
                {
                }
            }
            return;
        }

        void
        listen() {
            m_logger->connect("localhost", Ports::LOGD_PORT);
            m_client->connect("localhost", Ports::MESSAGE_PORT);
            MessageInit init;
            m_client->send_message(init);
            std::unique_lock<std::mutex> l(g_mutex);
            while( !g_stop ) {
                g_event.wait(l);
            }
            m_client->disconnect();
        }

        ClientFSM()
            :m_client(
                    MessageComponentFactory::create_factory()->create_message_client()),
            m_logger(
                    LoggingComponentFactory::create_factory()->create_remote_logger()) {
                m_client->signal_message_received().connect( boost::bind(
                        std::mem_fn( &ClientFSM::handle_server_request),
                        this, _1 ));
        }
    private:
        MessageClientPtr m_client;
        RemoteLoggerPtr m_logger;
};

int main() {
    ClientFSM fsm;
    fsm.listen();
    return 0;
}
