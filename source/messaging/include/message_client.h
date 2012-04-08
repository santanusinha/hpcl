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
#ifndef HPCL_MESSAGE_CLIENT_H
#define HPCL_MESSAGE_CLIENT_H

#include <memory>
#include <thread>

#include <boost/signals2.hpp>

#include "communication_pointer_types.h"
#include "meminfo.h"
#include "messaging_pointer_types.h"

namespace Hpcl {

enum MessageClientType {
    MESSAGE_CLIENT_SERVER_SIDE,
    MESSAGE_CLIENT_CLIENT_SIDE
};

class MessageClient : public std::enable_shared_from_this<MessageClient> {
    friend class MessageComponentFactory;

    public:
        typedef std::shared_ptr<MessageClient> Pointer;
        typedef std::weak_ptr<MessageClient> WeakPointer;
        typedef boost::signals2::signal<
                            void(const MemInfo &)> SignalMessageReceived;

        MessageClient( const MessageClient &) = delete;
        MessageClient &
        operator =( const MessageClient &) = delete;

        virtual
        ~MessageClient();

        void
        connect(const std::string &in_server, int32_t in_port);

        void
        disconnect();

        template <typename _Tp>
        void
        send_message( const _Tp &in_msg ) {
            MemInfo info( reinterpret_cast<const char *>(&in_msg), sizeof(_Tp) );
            send_message_impl( info );
        }

        SignalMessageReceived &
        signal_message_received();

    protected:

        MessageClient( const SocketFactoryPtr &in_factory );

#if 0
        //Open up when delegation is supported (gcc 4.7 series)
        MessageClient( const SocketPtr &in_socket );
#else
        void
        connect( const SocketPtr &in_socket );
#endif

        virtual void
        on_message_received( const Pointer &in_client,
                                const MemInfo &in_message );
    private:

        void
        send_message_impl( const MemInfo &in_message );

        void
        handle_data_received( const SocketPtr &in_socket,
                                const MemInfo &in_msg );

        void
        handle_stop( const SocketPtr &in_socket );

        std::mutex m_comm_mutex;
        std::condition_variable m_comm_event;
        SocketFactoryPtr m_socket_factory;
        SocketPtr m_socket;
        bool m_stop;
        bool m_is_waiting_for_response;
        int32_t m_received_msg_type;
        SignalMessageReceived m_signal_message_received;
        MessageClientType m_type;
};

} //namespace Hpcl

#endif //HPCL_MESSAGE_CLIENT_H

