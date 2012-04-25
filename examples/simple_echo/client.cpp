#include <iostream>
#include "socket.h"
#include "socket_factory.h"
#include "syscall_error.h"

using namespace Hpcl;

std::mutex g_stop_mutex;
bool g_stop = false;

void
handle_data( const SocketPtr &in_socket, const MemInfo &in_data ) {
    std::string msg = in_data.get_data();
    std::cout<<"From server: "<<msg<<std::endl;
    return;
}

void
handle_stop( const SocketPtr &in_socket ) {
    std::cout<<"Disconnect from server..press <ENTER> to exit"<<std::endl;
    std::lock_guard<std::mutex> l( g_stop_mutex );
    g_stop = true;
    return;
}

void
send_data( const SocketPtr &in_socket, const std::string &in_msg ) {
    if( in_socket->get_is_connected() )
    {
        MemInfo info( in_msg.c_str(), in_msg.length() + 1 );
        in_socket->send_data( info );
    }
}

int main() 
try {
    SocketFactoryPtr factory = SocketFactory::create_factory();
    SocketPtr socket = factory->create_socket( true );
    socket->signal_data_received().connect( handle_data );
    socket->signal_remote_disconnect().connect( handle_stop );
    socket->connect_to_server( "127.0.0.1", 1024 );
    while( true )
    {
        {
            std::lock_guard<std::mutex> l(g_stop_mutex);
            if( g_stop )
            {
                socket->shutdown();
                break;
            }
        }
        std::string msg;
        std::cout<<"say? ";
        getline( std::cin, msg );
        if( "QUIT" == msg )
        {
            std::lock_guard<std::mutex> l(g_stop_mutex);
            g_stop = true;
            socket->shutdown();
        }
        else if( "STOPLISTENER" == msg )
        {
            send_data( socket, msg );
            socket->shutdown();
            std::lock_guard<std::mutex> l(g_stop_mutex);
            g_stop = true;
        }
        else
        {
            send_data( socket, msg );
        }
    }
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

