#include <cstring>
#include <iostream>
#include "message_client.h"
#include "message_component_factory.h"
#include "message_server.h"

#include "messages.h"

using namespace Hpcl;

class ServerFSM {

    public:

        void
        handle_client_message( const MemInfo &in_msg,
                                const MessageClientPtr &in_client ) {
            const MessageHeader *header
                                = reinterpret_cast<const MessageHeader *>(
                                                in_msg.get_data() );
            switch( header->m_type ) {
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
                    std::cout<<"Client completed"<<std::endl;
                    m_server->stop();
                    break;
                }
                default:
                {
                }
            }
            return;
        }

        void
        run() {
            std::exception_ptr error;
            m_server->listen( 1026, error );
            if( error ) {
                std::rethrow_exception( error );
            }
        }

        ServerFSM()
            :m_factory( MessageComponentFactory::create_factory() ),
            m_server( m_factory->create_server() ) {
                m_server->signal_message_received().connect( boost::bind(
                        std::mem_fn( &ServerFSM::handle_client_message ),
                        this, _1, _2 ));
        }
    private:
        MessageComponentFactoryPtr m_factory;
        MessageServerPtr m_server;
};

int main()
{
    ServerFSM fsm;
    fsm.run();
    return 0;
}

