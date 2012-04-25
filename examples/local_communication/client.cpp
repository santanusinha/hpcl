#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include "local_communicator.h"
#include "semantic_error.h"
#include "syscall_error.h"

using namespace Hpcl;

int main(int argc, char *argv[] ) {
    std::string exec_name = "LocalCommExample";
    try {
        LocalCommunicatorPtr comm = LocalCommunicatorPtr(
                                            new LocalCommunicator());
        comm->init( exec_name, LocalCommunicator::Type::CLIENT, BUFSIZ);
        std::string msg = "Hello World";
        comm->send( msg );
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
