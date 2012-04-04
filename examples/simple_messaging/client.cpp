#include <iostream>

#include "message_client.h"
#include "message_component_factory.h"

#include "messages.h"

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
                    std::cout<<data->msg;
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
            m_client->connect("127.0.0.1", 1026);
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
                    MessageComponentFactory::create_factory()->create_client() ) {
                m_client->signal_message_received().connect( boost::bind(
                        std::mem_fn( &ClientFSM::handle_server_request),
                        this, _1 ));
        }
    private:
        MessageClientPtr m_client;
};

int main() {
    ClientFSM fsm;
    fsm.listen();
    return 0;
}
