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

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "print.h"
#include "server_socket.h"
#include "syscall_error.h"

namespace {

enum ServerSocketEvent {
    NO_EVENT,
    SHUTDOWN_REQUEST,
    CLIENT_SOCKET_SHUTDOWN_REQUEST
};

}

namespace Hpcl {
ServerSocket::ServerSocket( const SocketFactoryPtr &in_factory )
    :m_server_fd(),
    m_port(),
    m_signal_client_connected(),
    m_listen_thread(),
    m_factory( in_factory ),
    m_event_notifier(),
    m_client_remote_disconnect_mutex(),
    m_client_mutex(),
    m_all_clients_disconnected(),
    m_clients(),
    m_disconnected_clients() {
}

ServerSocket::~ServerSocket() {
}

ServerSocket::SignalClientConnected &
ServerSocket::signal_client_connected() {
    return m_signal_client_connected;
}

void
ServerSocket::listen( int32_t in_port, std::exception_ptr &out_error ) {
    m_port = in_port;
    out_error = std::exception_ptr();
    try {
        m_event_notifier.init();
        if( ( m_server_fd = socket(
                            PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 ) {
            throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("socket");
        }
        int32_t optval;
        optval=1;

        if (-1 == setsockopt(
                    m_server_fd,SOL_SOCKET,SO_REUSEADDR,
                    &optval,sizeof(optval)))
        {
            throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("setsockopt");
        } 
        struct sockaddr_in server_info;
        bzero( &server_info, sizeof( sockaddr_in ) );
        server_info.sin_family = PF_INET;
        server_info.sin_addr.s_addr = htons(INADDR_ANY);
        server_info.sin_port = htons(m_port);
        if( bind( m_server_fd, (struct sockaddr *)&server_info,
                                                sizeof( server_info )) < 0 ) {
            throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("bind");
        }
        if( ::listen( m_server_fd, 1024 ) < 0 ) {
            throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("listen");
        }
        while( true ) {
            struct sockaddr_in client_info;
            bzero( &client_info, sizeof( sockaddr_in ) );
            socklen_t len = sizeof( client_info );
            int32_t client_sock = 0;
            fd_set waitFDs;
            FD_ZERO(&waitFDs);
            FD_SET( m_event_notifier.get_wait_fd(), &waitFDs );
            FD_SET( m_server_fd, &waitFDs );
            int32_t maxFD = std::max( m_event_notifier.get_wait_fd(),
                                        m_server_fd );
            if( -1 == select( maxFD + 1, &waitFDs, NULL, NULL, NULL) )
            {
                throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("select");
            }
            switch( m_event_notifier.get_event_type() )
            {
                case ServerSocketEvent::SHUTDOWN_REQUEST:
                {
                    std::vector<SocketPtr> connected;
                    {
                        std::unique_lock<std::mutex> l( m_client_mutex );
                        copy( m_clients.begin(), m_clients.end(),
                                    back_inserter(connected));
                    }
                    for( auto& client: connected )
                    {
                        client->shutdown();
                    }
                    std::unique_lock<std::mutex> l( m_client_mutex );
                    while( !m_clients.empty() )
                    {
                        m_all_clients_disconnected.wait(l);
                    }
                    connected.clear();
                    hpcl_debug("All clients exited\n");
                    if( -1 == ::shutdown( m_server_fd, SHUT_RDWR ) )
                    {
                        throw SyscallError()
                            << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("shutdown");
                    }
                    if( -1 == ::close( m_server_fd ) )
                    {
                        throw SyscallError()
                            << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("close");
                    }
                    hpcl_debug("Server socket closed\n");
                    m_server_fd = 0;
                    return;
                }
                case ServerSocketEvent::CLIENT_SOCKET_SHUTDOWN_REQUEST:
                {
                    std::unique_lock<std::mutex> l(
                                        m_client_remote_disconnect_mutex);
                    for( auto & client : m_disconnected_clients ) {
                        client->shutdown();
                    }
                    m_event_notifier.acknowledge();
                    hpcl_debug("Disconnected clients shut down requested\n");
                    continue;
                }
            }
            if( (client_sock = accept( m_server_fd,
                            (struct sockaddr *)&client_info, &len )) < 0 )
            {
                throw SyscallError() << boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("accept");
            }
            SocketPtr client= m_factory->create_on_server(client_sock);
            {
                std::unique_lock<std::mutex> l( m_client_mutex );
                m_clients.insert( client );
            }
            client->signal_shutdown().connect( boost::bind(
                                std::mem_fn( &ServerSocket::client_stop ),
                                this, _1 ));
            client->signal_remote_disconnect().connect( boost::bind(
                        std::mem_fn( &ServerSocket::client_remote_stop),
                        this, _1 ));
            m_signal_client_connected( client );
            on_client_connected( client );
        }
    }
    catch( ... )
    {
        out_error = std::current_exception();
    }
    return;
    m_server_fd = 0;
}


void
ServerSocket::shutdown() {
    m_event_notifier.notify( ServerSocketEvent::SHUTDOWN_REQUEST );
}

void
ServerSocket::on_client_connected( const SocketPtr &in_client ) {
    if(in_client) {
    }
    hpcl_debug("New client created");
}

void
ServerSocket::client_stop( const SocketPtr &in_stopped_socket ) {
    std::unique_lock<std::mutex> l( m_client_mutex );
    m_clients.erase( in_stopped_socket );
    hpcl_debug("Local disconnect called");
    if( m_clients.empty() )
    {
        m_all_clients_disconnected.notify_one();
    }
}

void
ServerSocket::client_remote_stop( const SocketPtr &in_stopped_socket ) {
    std::unique_lock<std::mutex> l( m_client_remote_disconnect_mutex );
    m_disconnected_clients.push_back( in_stopped_socket );
    hpcl_debug("Remote disconnect called");
    if( !m_event_notifier.get_event_type() )
    {
        m_event_notifier.notify(
                    ServerSocketEvent::CLIENT_SOCKET_SHUTDOWN_REQUEST );
    }
}

} //namespace Hpcl
