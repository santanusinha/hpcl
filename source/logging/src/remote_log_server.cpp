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

#include "print.h"
#include "remote_log_server.h"
#include "server_socket.h"
#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

RemoteLogServer::~RemoteLogServer() {
}

RemoteLogServer::SignalLogMessageReceived &
RemoteLogServer::signal_log_message_received() {
    return m_signal_log_message_received;
}

void
RemoteLogServer::listen( int32_t in_port, std::exception_ptr &out_error ) {
    if( m_server ) {
        return;
    }
    m_server = m_factory->create_server_socket();
    m_server->signal_client_created().connect( boost::bind(
                    std::mem_fn( &RemoteLogServer::on_create ), this, _1));
    m_server->listen( in_port, out_error );
}

void
RemoteLogServer::stop() {
    if( !m_server ) {
        return;
    }
    m_server->shutdown();
}

RemoteLogServer::RemoteLogServer( const SocketFactoryPtr &in_factory )
    :m_factory( in_factory ),
    m_server(),
    m_signal_log_message_received(),
    m_listen_thread() {
}

void
RemoteLogServer::on_log_message_received( const std::string &in_log_message ) {
    if( in_log_message.empty() ) {
        return;
    }
    hpcl_info("Message: %s", in_log_message.c_str() );
    return;
}

void
RemoteLogServer::on_create( const SocketPtr &in_socket ) {
    in_socket->signal_data_received().connect( boost::bind(
                std::mem_fn( &RemoteLogServer::on_log_received), this, _1, _2));
}

void
RemoteLogServer::on_log_received(
                    const SocketPtr &in_socket, const MemInfo &in_info ) {
    std::string message( in_info.get_data() );
    m_signal_log_message_received( message );
    return;
}

} //namespace Hpcl

