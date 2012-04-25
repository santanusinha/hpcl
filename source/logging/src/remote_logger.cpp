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

#include "remote_logger.h"
#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

RemoteLogger::~RemoteLogger() {
    disconnect();
}

void
RemoteLogger::connect( const std::string &in_host, int32_t in_port ) {
    if( m_socket ) {
        return;
    }
    m_socket = m_factory->create_socket( false );
    m_socket->connect_to_server( in_host, in_port );
    return;
}

void
RemoteLogger::disconnect() {
    if( !m_socket ) {
        return;
    }
    m_socket->shutdown();
    m_socket.reset();
} 

void
RemoteLogger::log( const std::string &in_log_message ) {
    if( !m_socket ) {
        return;
    }
    if( in_log_message.size() > BUFSIZ ) {
        //TODO::ERROR
    }
    char msg[BUFSIZ];
    MemInfo log_msg( msg, BUFSIZ );
    ::strcpy( msg, in_log_message.c_str() );
    m_socket->send_data( log_msg );
}

RemoteLogger::RemoteLogger( const SocketFactoryPtr &in_factory )
    :m_factory( in_factory ),
    m_socket() {
}

} //namespace Hpcl
