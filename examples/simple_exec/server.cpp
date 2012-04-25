#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include "exec_server.h"
#include "child_process.h"
#include "remote_exec_component_factory.h"
#include "syscall_error.h"

bool
is_closed(int fd) {
    bool retval =false;
    int bytestoread=0;
    fd_set myfd;

    FD_ZERO(&myfd);
    FD_SET(fd,&myfd);
    int sio=select(FD_SETSIZE,&myfd, 0,0,0);
    //have to do select first for some reason
    if( -1 == ioctl(fd, FIONREAD, &bytestoread) ) {
        perror("ioctl");
    }
    retval=((bytestoread==0)&&(sio==1));

    return retval;
}

using namespace Hpcl;

void
handle_child_process( const ChildProcessPtr &in_child ) {
    int32_t stdout_fd = in_child->get_stdout();
    int32_t stderr_fd = in_child->get_stderr();
    std::cout<<"HERE"<<std::endl;
    {
        int32_t flags = fcntl(stdout_fd, F_GETFL, 0);
        fcntl(stdout_fd, F_SETFL, flags | O_NONBLOCK);
    }
    {
        int32_t flags = fcntl(stderr_fd, F_GETFL, 0);
        fcntl(stderr_fd, F_SETFL, flags | O_NONBLOCK);
    }
    while(true) {
        fd_set waitFDs;
        FD_ZERO(&waitFDs);
        if( !is_closed( stdout_fd ) )
            FD_SET( stdout_fd, &waitFDs );
        if( !is_closed( stderr_fd ) )
            FD_SET( stderr_fd, &waitFDs );
        int max_fd = std::max( stdout_fd, stderr_fd );

        int retVal = 0;
        std::cout<<"WAITING"<<std::endl;
        if( -1 == (retVal
                = select( FD_SETSIZE, &waitFDs, NULL, NULL, NULL) ) ) {
            perror("select");
            return;
        }
        int32_t fd = 0;
        if( FD_ISSET( stdout_fd, &waitFDs ) ) {
            fd = stdout_fd;
        }
        else
        {
            if( FD_ISSET( stderr_fd, &waitFDs ) ) {
                fd = stderr_fd;
            }
        }
        char buf[BUFSIZ];
        while( read( fd, buf, BUFSIZ ) > 0 ) {
            std::cout<<"CHILD("<<in_child->get_pid()<<")::"<<buf;
        }
        int32_t err_no = errno;
        if( is_closed( fd ) ) {
            break;
        }
        if( err_no == EAGAIN )
        {
            continue;
        }
//        std::cout<<"DONE"<<std::endl;
//        break;
    }
    return;
}

int main()
try {
    RemoteExecComponentFactoryPtr factory
                = RemoteExecComponentFactory::create_factory();
    ExecServerPtr server = factory->create_exec_server();
    server->signal_child_started().connect( handle_child_process );
    std::exception_ptr error;
    server->listen( 1025, error );
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

