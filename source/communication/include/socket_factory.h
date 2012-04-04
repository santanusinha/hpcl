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

#include "communication_pointer_types.h"

namespace Hpcl {
class SocketFactory: public std::enable_shared_from_this<SocketFactory> {
    public:
        typedef std::shared_ptr<SocketFactory> Pointer;
        typedef std::weak_ptr<SocketFactory> WeakPointer;

        static Pointer
        create_factory();
        
        virtual ~SocketFactory();

        SocketFactory( const SocketFactory & ) = delete;

        SocketFactory &
        operator =( const SocketFactory & ) = delete;

        ServerSocketPtr
        create_server();

        SocketPtr
        create_client();

    protected:
        SocketFactory();

        virtual ServerSocketPtr
        create_new_server_socket();

        virtual SocketPtr
        create_new_socket();
};

} //namespace Hpcl

#endif

