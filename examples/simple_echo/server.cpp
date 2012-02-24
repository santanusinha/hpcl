#include <iostream>
#include "server_socket.h"
#include "socket.h"
#include "socket_factory.h"
#include "syscall_error.h"

using namespace Hpcl;

class EchoServer : public ServerSocket {
    public:
        EchoServer( const SocketFactoryPtr &in_factory )
            :ServerSocket( in_factory ) {
        }

        ~EchoServer() {
        }

        void
        handle_client_disconnect( const SocketPtr &in_client ) {
            std::cout<<"Echo client disconneted"<<std::endl;
        }

        void
        handle_client_request( const SocketPtr &in_client,
                                 const MemInfo &in_data ) {
            std::string msg = in_data.get_data();
            if( msg == "STOPLISTENER" ) {
                this->shutdown();
                std::cout<<"Server shut down"
                    <<std::endl;
                return;
            }
            std::cout<<msg<<std::endl;
            MemInfo ack( "Received", 9 );
            in_client->send_data( ack );
            return;
        }

    protected:
        void
        on_client_connected( const SocketPtr &in_socket ) {
            std::cout<<"Got new client connection"<<std::endl;
            in_socket->signal_shutdown().connect(
                        boost::bind(
                            std::mem_fn( &EchoServer::handle_client_disconnect ),
                            this, _1));
            in_socket->signal_data_received().connect(
                        boost::bind(
                            std::mem_fn( &EchoServer::handle_client_request ),
                            this, _1, _2));

        }

};

int main() {
    EchoServer server( SocketFactoryPtr( new SocketFactory() ) );
    try
    {
        std::exception_ptr error;
        server.listen( 1024, error );
        if( error ) {
            std::rethrow_exception( error );
        }
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
return 0;
}
