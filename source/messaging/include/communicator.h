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

#ifndef HPCL_COMMUNICATOR_H
#define HPCL_COMMUNICATOR_H

#include <memory>
#include <thread>

#include "server_socket.h"
#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

class Communicator {
    public:
        typedef std::shared_ptr<Communicator> Pointer;
        typedef std::weak_ptr<Communicator> WeakPointer;

        virtual
        ~Communicator();

    private:
        Communicator( const SocketFactoryPtr &in_factory,
                        int32_t in_listen_port );
        Communicator( const Communicator &) = delete;
        Communicator &
        operator = ( const Communicator &) = delete;
};

} //namespace Hpcl

#endif //HPCL_COMMUNICATOR_H
