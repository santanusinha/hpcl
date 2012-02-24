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

#ifndef HPCL_SERVER_FACTORY_H
#define HPCL_SERVER_FACTORY_H
#include <memory>

#include "socket.h"

namespace Hpcl {
class SocketFactory {
    public:
        typedef std::shared_ptr<SocketFactory> Pointer;
        typedef std::weak_ptr<SocketFactory> WeakPointer;

        SocketFactory();
        virtual ~SocketFactory();

        SocketFactory( const SocketFactory & ) = delete;

        SocketFactory &
        operator =( const SocketFactory & ) = delete;

        SocketPtr
        create_on_server( int32_t in_connection_fd );

        SocketPtr
        create_on_client( const std::string &in_server, int32_t port );

    protected:
        virtual SocketPtr
        create_new_socket( int32_t in_connection_fd );
};

typedef SocketFactory::Pointer SocketFactoryPtr;
typedef SocketFactory::WeakPointer SocketFactoryWeakPtr;

} //namespace Hpcl

#endif

