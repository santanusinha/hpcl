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

#include "socket_factory.h"
#include "syscall_error.h"

namespace Hpcl {
SocketFactory::SocketFactory() {
}

SocketFactory::~SocketFactory() {
}

SocketPtr
SocketFactory::create_on_server( int32_t in_connection_fd ) {
    SocketPtr new_socket = create_new_socket( in_connection_fd );
    new_socket->start_listening();
    return new_socket;
}

SocketPtr
SocketFactory::create_on_client(
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
    SocketPtr new_socket = create_new_socket( client_socket );
    new_socket->start_listening();
    return new_socket;
}

SocketPtr
SocketFactory::create_new_socket( int32_t in_connection_fd ) {
    return SocketPtr( new Socket( in_connection_fd ) );
}

} //namespace Hpcl
