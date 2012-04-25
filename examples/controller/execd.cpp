#include <iostream>
#include "exec_server.h"
#include "child_process.h"
#include "remote_exec_component_factory.h"
#include "syscall_error.h"

#include "ports.h"

using namespace Hpcl;

void
handle_child_process( const ChildProcessPtr &in_child ) {
    std::cout<<"Process started with PID "<<in_child->get_pid()<<std::endl;
}

void
handle_child_completion( pid_t in_pid, int32_t in_status ) {
    std::cout<<"Process with PID "<<in_pid
            <<" ended with status "<<in_status<<std::endl;
}

int main()
try {
    RemoteExecComponentFactoryPtr factory
                = RemoteExecComponentFactory::create_factory();
    ExecServerPtr server = factory->create_exec_server();
//    server->signal_child_started().connect( handle_child_process );
//    server->signal_child_completed().connect( handle_child_completion );
    std::exception_ptr error;
    server->listen( Ports::EXECD_PORT, error );
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

