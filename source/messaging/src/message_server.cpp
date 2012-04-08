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

#include "message_server.h"
#include "message_component_factory.h"
#include "messaging_messages.h"
#include "print.h"
#include "server_socket.h"
#include "socket.h"
#include "socket_factory.h"


namespace Hpcl {
MessageServer::MessageServer( const MessageComponentFactoryPtr &in_factory,
                                const SocketFactoryPtr &in_socket_factory )
    :m_factory( in_factory),
    m_socket_factory( in_socket_factory),
    m_server( m_socket_factory->create_server() ),
    m_listener(),
    m_server_error(),
    m_message_mutex(),
    m_message_event(),
    m_stop(),
    m_clients(),
    m_message_queue(),
    m_connected_sockets(),
    m_signal_message_received() {
}

MessageServer::~MessageServer() {
}

MessageServer::SignalMessageReceived &
MessageServer::signal_message_received() {
    return m_signal_message_received;
}

void
MessageServer::listen( int32_t in_port, std::exception_ptr &out_error ) {
    m_server->signal_client_created().connect( boost::bind(
                    std::mem_fn( &MessageServer::handle_new_client ),
                    this, _1 ));

    m_server->signal_client_connected().connect( boost::bind(
                    std::mem_fn( &MessageServer::handle_new_client_connect ),
                    this, _1 ));

    m_server->listen( in_port, out_error );
    hpcl_debug("Stopped\n");
}

void
MessageServer::stop() {
    if( !m_server ) {
        return;
    }
    m_server->shutdown();
}

void
MessageServer::on_message_received( const MemInfo &in_msg,
                                     const MessageClientPtr &in_ptr ) {
    if( in_ptr ) {
    }
}

void
MessageServer::handle_client_request( const SocketPtr &in_client,
                                         const MemInfo &in_data ) {
    MessageClientPtr msg_client;
    {
        std::unique_lock<std::mutex> l(m_message_mutex);
    //    char *msg_buf = new char[in_data.get_data_size()];
    //    ::memcpy(msg_buf, in_data.get_data(), in_data.get_data_size());
    //    MemInfo msg( msg_buf, in_data.get_data_size() );
    //    m_message_queue.push( std::make_pair(in_client,msg) );
    //    m_message_event.notify_one();
        auto it = m_clients.find( in_client );
        if( it == m_clients.end() )
        {
            hpcl_debug("Message from invalid client\n");
            return;
        }
        else
        {
            msg_client = (*it).second;
        }
    }
    on_message_received( in_data, msg_client );
    m_signal_message_received( in_data, msg_client );
    return;
}

void
MessageServer::handle_new_client( const SocketPtr &in_socket ) {
    std::lock_guard<std::mutex> l( m_message_mutex);
    in_socket->signal_remote_disconnect().connect( boost::bind(
        std::mem_fn( &MessageServer::handle_client_disconnect ), this, _1));
    in_socket->signal_data_received().connect( boost::bind(
        std::mem_fn( &MessageServer::handle_client_request ), this, _1, _2));
    m_connected_sockets.insert( in_socket );
    MessageClientPtr client = m_factory->create_server_side_client( in_socket );
    m_clients.insert( std::make_pair( in_socket, client ) );
}

void
MessageServer::handle_new_client_connect( const SocketPtr &in_socket ) {
    MessagingMessage msg( MessagingMessageType::MESSAGE_INIT );
    in_socket->send_data( MemInfo( reinterpret_cast<char *>( &msg ),
                            sizeof(MessagingMessage)));
}

void
MessageServer::handle_client_disconnect( const SocketPtr &in_client ) {
    std::unique_lock<std::mutex> l(m_message_mutex);
    m_clients.erase( in_client );
    m_connected_sockets.erase( in_client );
    m_message_event.notify_one();
}


} //namespace Hpcl
