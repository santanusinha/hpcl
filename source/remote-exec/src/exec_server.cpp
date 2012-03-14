#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/tokenizer.hpp>

#include "exec_server.h"
#include "exec_message.h"
#include "print.h"
#include "semantic_error.h"
#include "syscall_error.h"

namespace {
using namespace Hpcl;

void
parse_command(char *argv[], int64_t &in_num_args,
                const std::string &in_command ) {
    boost::char_separator<char> delimiter(" ");
    boost::tokenizer<boost::char_separator<char>> tok( in_command, delimiter );
    int64_t i = 0;
    for(auto s : tok)
    {
        argv[i]=new char[ s.length() + 1 ];
        ::strcpy( argv[i], s.c_str() );
        i++;
        if( i == in_num_args )
        {
            return;
        }
    }
    argv[i] = nullptr;
    in_num_args = i;
}

}

namespace Hpcl {

ExecServer::ExecServer()
    :enable_shared_from_this(),
    m_server(),
    m_signal_child_started(),
    m_signal_child_completed() {
}

ExecServer::~ExecServer() {
}

void
ExecServer::start( const SocketFactoryPtr &in_factory, int32_t in_port ) {
    if( !in_factory ) {
        throw SemanticError()
            <<errinfo_errorid(ErrorID::SEMANTIC_ERR_INVALID_PARAMETER);
    }
    m_server = std::make_shared<ServerSocket>( in_factory );//TODO::FACTORY
    m_server->signal_client_connected().connect( boost::bind(
                        std::mem_fn( &ExecServer::on_client_connected ),
                        this, _1 ));
    m_server->signal_client_connected().connect( boost::bind(
                std::mem_fn( &ExecServer::on_client_remote_disconnect),
                this, _1 ));
    std::exception_ptr error;
    m_server->listen( in_port, error );
    if( error ) {
        std::rethrow_exception( error );
    }
}

void
ExecServer::stop() {
    m_server->shutdown();
}

ExecServer::SignalChildStarted &
ExecServer::signal_child_started() {
    return m_signal_child_started;
}

ExecServer::SignalChildCompleted &
ExecServer::signal_child_completed() {
    return m_signal_child_completed;
}

void
ExecServer::on_child_start_request( const std::string &in_cmd ) {
    hpcl_debug("Child start request with: %s\n", in_cmd.c_str());
}

void
ExecServer::on_child_started( const ChildProcessPtr &in_child ) {
    hpcl_debug("Child with pid %d started\n", in_child->get_pid());
}

void
ExecServer::on_child_completed( pid_t in_pid, int32_t in_status ) {
    hpcl_debug("Child with pid %d completed with status %d\n",
                                    in_pid, in_status);
}

void
ExecServer::on_command_received( const SocketPtr &in_socket,
                                    const MemInfo &in_cmd) {
    if( 0 == in_cmd.get_data_size() ) {
        hpcl_debug("Called with no data\n");
        return;
    }
    const ExecMessage *msg = reinterpret_cast<const ExecMessage *>(
                                                    in_cmd.get_data());
    switch( msg->m_msg_type ) {
        case ExecMessageCode::EXEC_COMMAND:
        {
            std::string command = msg->m_command;
            hpcl_debug("Executing command: %s\n",command.c_str());
            handle_process_creation( command, in_socket );
            break;
        }
        case ExecMessageCode::EXEC_PIN_COMMAND:
        {
            //TODO
        }
        case ExecMessageCode::EXEC_DEBUG_COMMAND:
        {
            //TODO
        }
    }
}

void
ExecServer::handle_process_creation( const std::string &in_command,
                                            const SocketPtr &in_socket ) {
    int child_stdout[2], child_stdin[2], child_stderr[2];
    if( -1 == pipe( child_stdout )
            || -1 == pipe( child_stderr ) 
            || -1 == pipe( child_stdin ) ) {
        throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("pipe");
    }
    //child_process->m_communicator //TODO
    pid_t pid = 0;
    switch(pid = fork())
    {
        case -1:
        {
            throw SyscallError() <<boost::errinfo_errno(errno)
                        << boost::errinfo_api_function("vfork");

            return;
        }
        case 0:
        {
           //Child process
            if( -1 == dup2( child_stdout[1], 1 )
                    || -1 == dup2( child_stderr[1], 2 )
                    || -1 == dup2( child_stdin[0], 0 ) )
            {
                throw SyscallError() <<boost::errinfo_errno(errno)
                        << boost::errinfo_api_function("dup2");
            }
            if( -1 == close( child_stdin[1] )
                    || -1 == close( child_stdout[0] )
                    || -1 == close( child_stderr[0] ) )
            {
                throw SyscallError() <<boost::errinfo_errno(errno)
                        << boost::errinfo_api_function("close");

            }
            int64_t num_max_args = sysconf(_SC_ARG_MAX);
            if( -1 == num_max_args )
            {
                perror("sysconf");
                num_max_args = 4096;
            }
            char **argv = new char *[num_max_args];
            parse_command(argv, num_max_args, in_command);
            if( -1 == execvp(argv[0], argv) )
            {
                perror("execv");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
            break;
        }
        default:
        {
            ChildProcessPtr child_process
                                = std::make_shared<ChildProcess>();
            if( -1 == close( child_stdout[1] ) ) {
                throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
            }
            if( -1 == close( child_stderr[1] ) ) {
               throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
            }
            if( -1 == close( child_stdin[0] ) ) {
                throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
            }
            child_process->set_pid( pid );
            child_process->set_stdout( child_stdout[0] );
            child_process->set_stderr( child_stderr[0] );
            child_process->set_stdin( child_stdin[1] );
            on_handler_process_spawn( child_process );
            int status = 0;
            waitpid(pid, &status, 0);
            if( -1 == close( child_process->get_stdout() ) ) {
                throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
            }
            if( -1 == close( child_process->get_stderr() ) ) {
                throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
            }
            if( -1 == close( child_process->get_stdin() ) ) {
                throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
            }
            hpcl_debug("Sending status %d\n", status );
            if( in_socket ) {
                ExecReplyMessage reply;
                reply.m_status = status;
                in_socket->send_data(
                                MemInfo( reinterpret_cast<char *>(&reply),
                                    sizeof( ExecReplyMessage ) ) );
            }
            on_child_completed( pid, status );
            m_signal_child_completed( pid, status );
            break;
        }
    }
    return;
}

void
ExecServer::on_client_connected( const SocketPtr &in_client ) {
    if( in_client ) {
        in_client->signal_data_received().connect( boost::bind(
                    std::mem_fn(&ExecServer::on_command_received),
                    this, _1, _2));
    }
}

void
ExecServer::on_client_remote_disconnect( const SocketPtr &in_client ) {
    if( in_client ) {
        //TODO
    }
}

void
ExecServer::on_handler_process_spawn( const ChildProcessPtr &in_child ) {
    on_child_started( in_child );
    m_signal_child_started( in_child );
}

} //namespace Hpcl