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

#ifndef HPCL_COMMUNICATION_POINTER_TYPES_H
#define HPCL_COMMUNICATION_POINTER_TYPES_H

#include <memory>

namespace Hpcl {
class LocalCommunicator;
typedef std::shared_ptr<LocalCommunicator> LocalCommunicatorPtr;
typedef std::weak_ptr<LocalCommunicator> LocalCommunicatorWeakPtr;

class Socket;
typedef std::shared_ptr<Socket> SocketPtr;
typedef std::weak_ptr<Socket> SocketWeakPtr;

class ServerSocket;
typedef std::shared_ptr<ServerSocket> ServerSocketPtr;
typedef std::weak_ptr<ServerSocket> ServerSocketWeakPtr;

class SocketFactory;
typedef std::shared_ptr<SocketFactory> SocketFactoryPtr;
typedef std::weak_ptr<SocketFactory> SocketFactoryWeakPtr;

} //namespace Hpcl
#endif //HPCL_COMMUNICATION_POINTER_TYPES_H
