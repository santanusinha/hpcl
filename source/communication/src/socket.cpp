/*
 * Copyright (C) 2012 Santanu Sinha (santanu.sinha@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>


#include "buffer.h"
#include "notifier.h"
#include "print.h"
#include "socket.h"
#include "syscall_error.h"

namespace {
    enum SocketEvents {
        NO_EVENT,
        SHUTDOWN_REQUEST
    };
}

namespace Hpcl {

Socket::SignalDataReceived &
Socket::signal_data_received() {
    return m_signal_data_received;
}

Socket::SignalShutdown &
Socket::signal_shutdown() {
    return m_signal_shutdown;
}

Socket::SignalRemoteDisconnect &
Socket::signal_remote_disconnect() {
    return m_signal_remote_disconnect;
}

void
Socket::connect_to_server(
                    const std::string &in_server, int32_t in_port ) {
    int32_t client_socket = 0;
    if ((client_socket = socket(
                            PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        throw SyscallError() << boost::errinfo_errno(errno)
                            <<boost::errinfo_api_function("socket");
    } 
    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = PF_INET;
    server_info.sin_addr.s_addr = inet_addr( in_server.c_str() );
    server_info.sin_port = htons(in_port);
    /* Establish connection */
    if (::connect(client_socket, (struct sockaddr *) &server_info,
                sizeof(server_info)) < 0) {
        throw SyscallError() << boost::errinfo_errno(errno)
                            <<boost::errinfo_api_function("connect");
    }
    int32_t flags = fcntl(client_socket, F_GETFL, 0);
    if( fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) )
    {
        throw SyscallError() <<boost::errinfo_errno(errno)
                        << boost::errinfo_api_function("fcntl");
    }
    start( client_socket );
    return;
}

void
Socket::start(int32_t in_connection_fd) {
    m_socket = in_connection_fd;
    m_event_notifier->init();
    if( m_is_duplex ) {
        m_receiver_thread
            = std::shared_ptr<std::thread>(
                new std::thread( boost::bind( std::mem_fn(
                                &Socket::wait_for_data), this, m_error)));
    }
}

bool
Socket::get_is_connected() const {
    return m_is_connected.load();
}

void
Socket::send_data( const MemInfo &in_data ) {
    if( !m_is_connected ) {
        hpcl_debug("Ignoring data .. Not connected\n");
    }
    ssize_t data_size = in_data.get_data_size();
    if( data_size != send(
                    m_socket, in_data.get_data(), data_size, 0 ) ) {
        throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("send");
        //TODO::SELECT/RETRY
    }
}

void
Socket::shutdown() {
    if( m_is_duplex && !m_receiver_thread )
        return;
    if( m_is_duplex ) {
        m_event_notifier->notify( SocketEvents::SHUTDOWN_REQUEST );
        m_receiver_thread->join();
        hpcl_debug("Thread stopped\n");
    }
    if( -1 == ::shutdown( m_socket, SHUT_RDWR ) )
    {
//        throw SyscallError() << boost::errinfo_errno(errno)
//                            << boost::errinfo_api_function("shutdown");
    }
    if( -1 == ::close( m_socket ) )
    {
        throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
    }
    m_is_connected.store( false );
    m_socket = 0;
    m_receiver_thread.reset();
    m_signal_shutdown( shared_from_this() );
    on_shutdown( shared_from_this() );
    if( m_error ) {
        std::rethrow_exception( m_error );
    }
}

Socket::Socket( bool in_is_duplex )
    :enable_shared_from_this<Socket>(),
    m_socket(),
    m_signal_data_received(),
    m_signal_shutdown(),
    m_signal_remote_disconnect(),
    m_receiver_thread(),
    m_event_notifier(std::make_shared<Notifier>()),
    m_error(),
    m_is_connected(),
    m_is_duplex( in_is_duplex ) {
        m_is_connected.store(false);
}

Socket::~Socket() {
}

void
Socket::on_data_received( const MemInfo &in_data ) {
    hpcl_debug("Received %llu bytes of data\n", in_data.get_data_size());
}

void
Socket::on_shutdown( const SocketPtr &in_this ) {
    if( in_this ) {
    }
    hpcl_debug("Client socket shutdown completed\n");
}

void
Socket::on_remote_disconnect( const SocketPtr &in_this ) {
    if( in_this ) {
    }
    hpcl_debug("Exiting receiver thread\n");
}

void
Socket::wait_for_data( std::exception_ptr &out_error ) {
    out_error = std::exception_ptr();
    m_is_connected.store(true);
    try {
        ssize_t received = 0;
        int32_t receivedError = 0;
        do
        {
            receivedError = 0;
            char recv_buf[BUFSIZ];
            fd_set waitFDs;
            FD_ZERO(&waitFDs);
            if( m_is_duplex ) {
                FD_SET( m_event_notifier->get_wait_fd(), &waitFDs );
            }
            FD_SET( m_socket, &waitFDs );
            if( -1 == select( FD_SETSIZE, &waitFDs, NULL, NULL, NULL) )
            {
                throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("select");
            }
            if( m_is_duplex && SocketEvents::SHUTDOWN_REQUEST
                            == m_event_notifier->get_event_type() )
            {
                hpcl_debug("Client socket shutdown requested\n");
                return;
            }
            Buffer buffer;
            while( ( received = recv( m_socket, recv_buf, BUFSIZ, 0 ) )
                        > 0 )
            {
                void *p_memory = buffer.get_memory(received);
                ::memcpy( p_memory, recv_buf, received );
            }
//            std::cout<<"Received: "<<buffer.get_size()<<std::endl;
            receivedError = errno;
            if( received < 0 && EAGAIN != errno )
            {
                throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("recv");
            }
            else if( buffer.get_size() > 0 )
            {
                MemInfo info(
                        reinterpret_cast<char *>(buffer.get_buffer()),
                        buffer.get_size() );
                try
                {
                    m_signal_data_received( shared_from_this(), info );
                    on_data_received( info );
                }
                catch(...) {
                    throw std::current_exception();
                }
            }
            else
            {
                //shutdown called
                break;
            }
        }
        while( received > 0 || receivedError == EAGAIN );
        m_is_connected.store(false);
        m_signal_remote_disconnect( shared_from_this() );
        on_remote_disconnect( shared_from_this() );
    }
    catch( ... ) {
        out_error = std::current_exception();
    }
    return;
}

} //namespace Hpcl

