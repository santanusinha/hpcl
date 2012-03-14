#include <iostream>
#include <thread>
#include "exec_client.h"

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
    SocketFactoryPtr factory
                        = std::make_shared<SocketFactory>();
    std::shared_ptr<ExecClient> client = std::make_shared<ExecClient>(
                                                                factory );
    client->signal_process_completed().connect( boost::bind(
                                                        completed, _1 ) );
    std::string cmd;
    while(true)
    {
        std::cout<<"cmd? ";
        getline( std::cin, cmd );
        if( "QUIT" == cmd ) {
            break;
        }
        client->execute("127.0.0.1",1025, cmd);
    }

    {
        std::unique_lock<std::mutex> l( g_complete_mutex );
        while( !g_completed )
        {
            g_wait_cond.wait( l );
        }
    }
    return 0;
}
