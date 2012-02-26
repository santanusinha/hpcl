#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include "local_communicator.h"
#include "semantic_error.h"
#include "syscall_error.h"

using namespace Hpcl;

const int32_t c_module_id = 101;

int main(int argc, char *argv[] ) {
    if(argc < 1 ) {
        std::cerr<<"Please provide init string given by server as argument"<<std::endl;
        return -1;
    }
    std::string exec_name = argv[1]; //Init string from server console
    try {
        LocalCommunicatorPtr comm = LocalCommunicatorPtr(
                                            new LocalCommunicator());
        comm->init( c_module_id, exec_name, BUFSIZ,
                                    LocalCommunicator::Type::CLIENT);
        char buf[BUFSIZ];
        std::string msg = "Hello World";
        ::strcpy(buf, msg.c_str());
        comm->send( MemInfo( buf, strlen(buf) + 1) );
    }
    catch(SemanticError &error) {
        std::cerr<<"Error occurred in local communicator"<<std::endl;
        return -1;
    }
    catch( SyscallError &error ) {
        int const *errorNo
                    = boost::get_error_info<boost::errinfo_errno>( error );
        std::cerr<<"Error occurred during \'"
                    <<boost::get_error_info<boost::errinfo_api_function>(error)
                    <<"\' [" <<strerror(*errorNo) <<"]"<<std::endl;
        return -1;
    }
    catch( std::exception &e ) {
        std::cerr<<"Error occurred [" <<e.what() <<"]"<<std::endl;
        return -1;
    }

    return 0;
}
