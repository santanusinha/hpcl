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
#endif //HAVE_CONFIG_H


#include "message_client.h"
#include "messaging_messages.h"
#include "print.h"
#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

MessageClient::~MessageClient() {
    disconnect();
}

void
MessageClient::connect(const std::string &in_server, int32_t in_port) {
    if( MessageClientType::MESSAGE_CLIENT_CLIENT_SIDE != m_type ) {
        return;
    }
    if( !m_socket ) {
        m_socket = m_socket_factory->create_socket( true );
        m_socket->signal_data_received().connect( boost::bind(
                std::mem_fn( &MessageClient::handle_data_received ),
                this, _1, _2));
        m_socket->connect_to_server( in_server, in_port );
    }
    std::unique_lock<std::mutex> l(m_comm_mutex);
    m_is_waiting_for_response = true;
    while( !m_stop && MessagingMessageType::MESSAGE_INIT
                                    != m_received_msg_type ) {
        m_comm_event.wait(l);
    }
    m_is_waiting_for_response = false;
    hpcl_debug("Received ACK\n");
    return;
}

void
MessageClient::disconnect() {
    if( m_socket->get_is_connected() ) {
        m_socket->shutdown();
    }
}

void
MessageClient::send_message_impl( const MemInfo &in_message ) {
    if( m_socket->get_is_connected() ) {
        m_socket->send_data( in_message );
        return;
    }
    //TODO::SEMANTIC ERROR
    return;
}

MessageClient::SignalMessageReceived &
MessageClient::signal_message_received() {
    return m_signal_message_received;
}

MessageClient::MessageClient( const SocketFactoryPtr &in_factory )
    :m_comm_mutex(),
    m_comm_event(),
    m_socket_factory( in_factory ),
    m_socket(),
    m_stop(),
    m_is_waiting_for_response(),
    m_received_msg_type(),
    m_signal_message_received(),
    m_type( MessageClientType::MESSAGE_CLIENT_CLIENT_SIDE ) {
}

#if 0
//Open up when delegation is supported (gcc 4.7 series)
MessageClient::MessageClient( const SocketPtr &in_socket )
    :MessageClient( SocketFactoryPtr() ) {
        m_type = MessageClientType::MESSAGE_CLIENT_SERVER_SIDE;
        m_socket = in_socket;
        m_socket->signal_data_received().connect( boost::bind(
                std::mem_fn( &MessageClient::handle_data_received ),
                this, _1, _2));
    return;
}
#else
void
MessageClient::connect( const SocketPtr &in_socket ) {
        m_type = MessageClientType::MESSAGE_CLIENT_SERVER_SIDE;
        m_socket = in_socket;
//        m_socket->signal_remote_disconnect().connect( boost::bind(
//                std::mem_fn( &MessageClient::handle_stop),
//                this, _1));
        m_socket->signal_data_received().connect( boost::bind(
                std::mem_fn( &MessageClient::handle_data_received ),
                this, _1, _2));
    return;
}
#endif

void
MessageClient::on_message_received( const Pointer &in_client,
                        const MemInfo &in_message ) {
    hpcl_debug("Message received of size %lu\n",
                        in_message.get_data_size());
}

void
MessageClient::handle_data_received( const SocketPtr &in_socket,
                        const MemInfo &in_msg ) {
    std::lock_guard<std::mutex> l(m_comm_mutex);
    if( m_is_waiting_for_response ) {
        const MessagingMessage *m_msg
                = reinterpret_cast<const MessagingMessage *>(
                                        in_msg.get_data());
        m_received_msg_type = m_msg->m_type;
        m_comm_event.notify_one();
        return;
    }
    on_message_received( shared_from_this(), in_msg );
    m_signal_message_received( in_msg );
    return;
}

void
MessageClient::handle_stop( const SocketPtr &in_socket ) {
    if( MESSAGE_CLIENT_SERVER_SIDE == m_type ) {
        return;
    }
    std::lock_guard<std::mutex> l(m_comm_mutex);
    m_stop = true;
    if( m_socket->get_is_connected() ) {
        m_socket->shutdown();
    }
    m_comm_event.notify_one();
}


} //namespace Hpcl
