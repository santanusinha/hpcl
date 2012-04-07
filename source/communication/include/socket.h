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

#ifndef HPCL_SOCKET_H
#define HPCL_SOCKET_H

#include <atomic>
#include <exception>
#include <memory>
#include <thread>

#include <boost/signals2.hpp>

#include "communication_pointer_types.h"
#include "meminfo.h"

namespace Hpcl { class Notifier; }
namespace Hpcl { class SocketFactory; }

namespace Hpcl {

class Socket : public std::enable_shared_from_this<Socket> {
    friend class SocketFactory;
    friend class ServerSocket;

    public:
        typedef std::shared_ptr<Socket> Pointer;
        typedef std::weak_ptr<Socket> WeakPointer;

        typedef boost::signals2::signal<
                void(const Pointer &, const MemInfo &)> SignalDataReceived;
        typedef boost::signals2::signal<
                void(const Pointer &)> SignalShutdown;
        typedef boost::signals2::signal<
                void(const Pointer &)> SignalRemoteDisconnect;

        virtual ~Socket();

        SignalDataReceived &
        signal_data_received();

        SignalShutdown &
        signal_shutdown();

        SignalRemoteDisconnect &
        signal_remote_disconnect();

        void
        connect_to_server( const std::string &in_server, int32_t port );

        void
        listen();

        bool
        get_is_connected() const;

        void
        send_data( const MemInfo &in_data );

        void
        shutdown();    

    protected:
        Socket( bool in_is_duplex );

        virtual void
        on_data_received( const MemInfo &in_data );

        virtual void
        on_shutdown( const Pointer &in_this );

        virtual void
        on_remote_disconnect( const Pointer &in_this );

    private:
        void
        start(int32_t in_connection_fd );

        void
        wait_for_data( std::exception_ptr &out_error );

        int32_t m_socket;
        SignalDataReceived m_signal_data_received;
        SignalShutdown m_signal_shutdown;
        SignalRemoteDisconnect m_signal_remote_disconnect;
        std::shared_ptr<std::thread> m_receiver_thread;
        std::shared_ptr<Notifier> m_event_notifier;
        std::exception_ptr m_error;
        std::atomic<bool> m_is_connected;
        bool m_is_duplex;
};

} //namespace Hpcl
#endif //HPCL_SOCKET_H
