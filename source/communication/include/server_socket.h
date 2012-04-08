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

#include "communication_pointer_types.h"

namespace Hpcl { class Notifier; }

namespace Hpcl {

class ServerSocket {
    friend class SocketFactory;

    public:
        typedef std::shared_ptr<ServerSocket> Pointer;
        typedef std::weak_ptr<ServerSocket> WeakPointer;

        typedef boost::signals2::signal<
                    void(const SocketPtr &)> SignalClientCreated;

        typedef boost::signals2::signal<
                    void(const SocketPtr &)> SignalClientConnected;

        virtual ~ServerSocket();

        SignalClientConnected &
        signal_client_created(); 

        SignalClientConnected &
        signal_client_connected(); 

        void
        listen( int32_t in_port, std::exception_ptr &out_error );

        void
        shutdown();

    protected:
        ServerSocket( const SocketFactoryPtr &in_factory );
        
        virtual void
        on_client_created( const SocketPtr &in_client );

        virtual void
        on_client_connected( const SocketPtr &in_client );

    private:
        void
        listen_function( std::exception_ptr &out_error );

        void
        client_stop( const SocketPtr &in_stopped_socket );

        void
        client_remote_stop( const SocketPtr &in_stopped_socket );

        struct ClientConnectionInfo {
            boost::signals2::connection m_shutdown_connection;
            boost::signals2::connection m_remote_shutdown_connection;

            ClientConnectionInfo();

            ClientConnectionInfo( const ClientConnectionInfo &) = default;

            ClientConnectionInfo &
            operator = ( const ClientConnectionInfo &) = default;

            ~ClientConnectionInfo() = default;
        };

        typedef std::map<SocketPtr, ClientConnectionInfo> Clients;

        int32_t m_server_fd;
        int32_t m_port;
        SignalClientCreated m_signal_client_created;
        SignalClientConnected m_signal_client_connected;
        std::shared_ptr<std::thread> m_listen_thread;
        SocketFactoryPtr m_factory;
        std::shared_ptr<Notifier> m_event_notifier;
        std::mutex m_client_remote_disconnect_mutex;
        std::mutex m_client_mutex;
        std::condition_variable m_all_clients_disconnected;
        Clients m_clients;
        std::vector<SocketPtr> m_disconnected_clients;
        std::mutex m_stop_check_mutex;
        bool m_is_being_stopped;
};


} //namespace Hpcl

#endif //HPCL_SERVER_SOCKET_H
