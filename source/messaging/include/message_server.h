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

#ifndef HPCL_MESSAGE_SERVER_H
#define HPCL_MESSAGE_SERVER_H

#include <memory>
#include <queue>
#include <set>
#include <thread>

#include <boost/signals2.hpp>

#include "common_pointer_types.h"
#include "communication_pointer_types.h"
#include "meminfo.h"
#include "messaging_pointer_types.h"

namespace Hpcl {

class MessageServer {
    friend class MessageComponentFactory;

    public:
        typedef std::shared_ptr<MessageServer> Pointer;
        typedef std::weak_ptr<MessageServer> WeakPointer;
        typedef boost::signals2::signal< void (const MemInfo &,
                        const MessageClientPtr &)> SignalMessageReceived;

        virtual
        ~MessageServer();

        SignalMessageReceived &
        signal_message_received();

        void
        listen( int32_t in_port );

        void
        stop_request();

    protected:
        MessageServer( const MessageComponentFactoryPtr &in_factory,
                       const SocketFactoryPtr &in_socket_factory );

        void
        on_message_received( const MemInfo &in_msg,
                             const MessageClientPtr &in_ptr );

    private:

        void
        handle_client_request( const SocketPtr &in_client,
                                 const MemInfo &in_data );

        void
        handle_new_client( const SocketPtr &in_socket );

        void
        handle_new_client_connect( const SocketPtr &in_socket );

        void
        handle_client_disconnect( const SocketPtr &in_client );

        typedef std::map<SocketPtr, MessageClientPtr> Clients;
        typedef std::pair<SocketPtr, MemInfo> ReceivedMessageInfo;

        MessageComponentFactoryPtr m_factory;
        SocketFactoryPtr m_socket_factory;
        ServerSocketPtr m_server;
        ThreadPtr m_listener;
        std::exception_ptr m_server_error;
        std::mutex m_message_mutex;
        std::condition_variable m_message_event;
        bool m_stop;
        Clients m_clients;
        std::queue<ReceivedMessageInfo> m_message_queue;
        std::set<SocketPtr> m_connected_sockets;
        SignalMessageReceived m_signal_message_received;
};

} //namespace Hpcl
#endif //HPCL_MESSAGE_SERVER_H
