#include <iostream>
#include <thread>
#include "remote_process.h"
#include "remote_exec_component_factory.h"

using namespace Hpcl;

std::mutex g_complete_mutex;
std::condition_variable g_wait_cond;
bool g_completed = false;

void
completed(int32_t) {
    std::unique_lock<std::mutex> l( g_complete_mutex );
    g_completed = true;
    g_wait_cond.notify_one();
}

int main() {
    RemoteExecComponentFactoryPtr factory
                = RemoteExecComponentFactory::create_factory();
    RemoteProcessPtr client = factory->create_remote_process();
    client->signal_process_completed().connect(
                        boost::bind( completed, _1 ) );
    std::string cmd;
    client->connect("127.0.0.1",1025);
    while(true)
    {
        std::cout<<"cmd? ";
        getline( std::cin, cmd );
        if( "QUIT" == cmd ) {
            break;
        }
        client->execute(cmd);
        std::unique_lock<std::mutex> l( g_complete_mutex );
        while( !g_completed )
        {
            g_wait_cond.wait( l );
        }
    }
    return 0;
}
