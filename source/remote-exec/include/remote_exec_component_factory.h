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

#ifndef HPCL_REMOTE_EXEC_COMPONENT_FACTORY_H
#define HPCL_REMOTE_EXEC_COMPONENT_FACTORY_H
#include <memory>

#include "communication_pointer_types.h"
#include "communication_pointer_types.h"

namespace Hpcl {
class RemoteExecComponentFactory
        : public std::enable_shared_from_this<RemoteExecComponentFactory>{
    public:
        typedef std::shared_ptr<RemoteExecComponentFactory> Pointer;
        typedef std::weak_ptr<RemoteExecComponentFactory> WeakPointer;

        static Pointer
        create_factory();
        
        virtual ~RemoteExecComponentFactory();

        RemoteExecComponentFactory(
                    const RemoteExecComponentFactory & ) = delete;

        RemoteExecComponentFactory &
        operator =( const RemoteExecComponentFactory & ) = delete;

        ExecServerPtr
        create_exec_server();

        RemoteProcessPtr
        create_remote_process();

    protected:
        RemoteExecComponentFactory();

        virtual ExecServerPtr
        create_new_exec_server();

        virtual RemoteProcessPtr
        create_new_remote_process();

        SocketFactoryPtr m_socket_factory;
};

} //namespace Hpcl

#endif //HPCL_REMOTE_EXEC_COMPONENT_FACTORY_H

