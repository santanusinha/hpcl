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

#include "exec_server.h"
#include "remote_process.h"
#include "socket_factory.h"

namespace Hpcl {
RemoteExecComponentFactory::RemoteExecComponentFactory()
    :enable_shared_from_this(),
    m_socket_factory( SocketFactory::create_factory() ) {
}

RemoteExecComponentFactory::~RemoteExecComponentFactory() {
}

RemoteExecComponentFactoryPtr
RemoteExecComponentFactory::create_factory() {
    return RemoteExecComponentFactoryPtr(
                            new RemoteExecComponentFactory() );
}

ExecServerPtr
RemoteExecComponentFactory::create_exec_server() {
    return create_new_exec_server();
}

RemoteProcessPtr
RemoteExecComponentFactory::create_remote_process() {
    return create_new_remote_process();
}

ExecServerPtr
RemoteExecComponentFactory::create_new_exec_server() {
    return ExecServerPtr( new ExecServer(
                    shared_from_this(), m_socket_factory ) );
}

RemoteProcessPtr
RemoteExecComponentFactory::create_new_remote_process() {
    return RemoteProcessPtr( new RemoteProcess( m_socket_factory ) );
}

} //namespace Hpcl
