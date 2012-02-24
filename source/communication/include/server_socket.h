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

#ifndef HPCL_SERVER_SOCKET_H
#define HPCL_SERVER_SOCKET_H

#include <cstdint>
#include <exception>
#include <memory>
#include <set>
#include <thread>
#include <vector>

#include <boost/signals2.hpp>

#include "notifier.h"
#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

class ServerSocket {
    public:
        typedef std::set<SocketPtr> Clients;
        typedef std::shared_ptr<ServerSocket> Pointer;
        typedef std::weak_ptr<ServerSocket> WeakPointer;

        typedef boost::signals2::signal<
                    void(const SocketPtr &)> SignalClientConnected;

        ServerSocket( const SocketFactoryPtr &in_factory );
        virtual ~ServerSocket();

        SignalClientConnected &
        signal_client_connected(); 

        void
        listen( int32_t in_port, std::exception_ptr &out_error );

        void
        shutdown();

    protected:
        virtual void
        on_client_connected( const SocketPtr &in_client );

    private:
        void
        listen_function( std::exception_ptr &out_error );

        void
        client_stop( const SocketPtr &in_stopped_socket );

        void
        client_remote_stop( const SocketPtr &in_stopped_socket );

        int32_t m_server_fd;
        int32_t m_port;
        SignalClientConnected m_signal_client_connected;
        std::shared_ptr<std::thread> m_listen_thread;
        SocketFactoryPtr m_factory;
        Notifier m_event_notifier;
        std::mutex m_client_remote_disconnect_mutex;
        std::mutex m_client_mutex;
        std::condition_variable m_all_clients_disconnected;
        Clients m_clients;
        std::vector<SocketPtr> m_disconnected_clients;
};


} //namespace Hpcl

#endif //HPCL_SERVER_SOCKET_H
