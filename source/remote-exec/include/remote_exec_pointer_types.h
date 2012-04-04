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

#ifndef HPCL_REMOTE_EXEC_POINTER_TYPES_H
#define HPCL_REMOTE_EXEC_POINTER_TYPES_H

#include <memory>

namespace Hpcl {

class ChildProcess;
typedef std::shared_ptr<ChildProcess> ChildProcessPtr;
typedef std::weak_ptr<ChildProcess> ChildProcessWeakPtr;

class RemoteProcess;
typedef std::shared_ptr<RemoteProcess> RemoteProcessPtr;
typedef std::weak_ptr<RemoteProcess> RemoteProcessWeakPtr;

class ExecServer;
typedef std::shared_ptr<ExecServer> ExecServerPtr;
typedef std::weak_ptr<ExecServer> ExecServerWeakPtr;

class RemoteExecComponentFactory;
typedef std::shared_ptr<RemoteExecComponentFactory> RemoteExecComponentFactoryPtr;
typedef std::weak_ptr<RemoteExecComponentFactory> RemoteExecComponentFactoryWeakPtr;

} //namespace Hpcl
#endif //HPCL_REMOTE_EXEC_POINTER_TYPES_H
