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

#include "server_socket.h"
#include "socket.h"
#include "socket_factory.h"
#include "syscall_error.h"

namespace Hpcl {
SocketFactory::SocketFactory()
    :enable_shared_from_this() {
}

SocketFactory::~SocketFactory() {
}

SocketFactoryPtr
SocketFactory::create_factory() {
    return SocketFactoryPtr( new SocketFactory() );
}

ServerSocketPtr
SocketFactory::create_server_socket() {
    return create_new_server_socket();
}

SocketPtr
SocketFactory::create_socket( bool in_is_duplex ) {
    return create_new_socket( in_is_duplex );
}

ServerSocketPtr
SocketFactory::create_new_server_socket() {
    return ServerSocketPtr( new ServerSocket( shared_from_this() ) );
}

SocketPtr
SocketFactory::create_new_socket( bool in_is_duplex ) {
    return SocketPtr( new Socket(in_is_duplex) );
}

} //namespace Hpcl
