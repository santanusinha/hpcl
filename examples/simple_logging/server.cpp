#include <iostream>
#include <fstream>

#include "logging_component_factory.h"
#include "remote_log_server.h"

using namespace std;
using namespace Hpcl;

class LogServer {
    public:
        void
        log_received( const std::string &in_message ) {
            m_fout<<in_message;
            m_fout.close();
            m_server->stop();
        }

        void
        run() {
            std::exception_ptr error;
            m_server->listen( 1028, error );
        }

        LogServer( const std::string &in_filename )
            :m_server( LoggingComponentFactory::create_factory()->create_server() ),
            m_fout( in_filename.c_str(), std::ios::out ) {
                m_server->signal_log_message_received().connect( boost::bind(
                            std::mem_fn( &LogServer::log_received ),
                            this, _1));
        }

        ~LogServer() {
        }

    private:
        RemoteLogServerPtr m_server;
        std::ofstream m_fout;
};


int main() {
    LogServer server("log.txt");
    server.run();
    return 0;
}

