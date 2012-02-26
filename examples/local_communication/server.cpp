#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include "local_communicator.h"
#include "semantic_error.h"
#include "syscall_error.h"

using namespace Hpcl;

const int32_t c_module_id = 101;

int main(int argc, char *argv[] ) {
    if(argc) {
    }
    try
    {
        std::string exec_name = argv[0];
        std::cout<<"Init string:"<<exec_name<<std::endl;
        LocalCommunicatorPtr comm = LocalCommunicatorPtr(
                                            new LocalCommunicator());
        comm->init( c_module_id, exec_name, BUFSIZ,
                                    LocalCommunicator::Type::SERVER);
        MemInfo received_data(0,0);
        comm->receive( received_data );
        std::cout<<"Received: "<<received_data.get_data()<<std::endl;
        sleep(1);
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
