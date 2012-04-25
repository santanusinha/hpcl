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

#include "message_component_factory.h"
#include "message_client.h"
#include "message_server.h"
#include "socket_factory.h"

namespace Hpcl {
MessageComponentFactoryPtr
MessageComponentFactory::create_factory() {
    SocketFactoryPtr factory = SocketFactory::create_factory();
    return Pointer( new MessageComponentFactory( factory ) );
}

MessageComponentFactory::~MessageComponentFactory() {
}

MessageServerPtr
MessageComponentFactory::create_message_server() {
    return create_new_message_server();
}

MessageClientPtr
MessageComponentFactory::create_message_client() {
    return create_new_client_side_message_client();
}

MessageServerPtr
MessageComponentFactory::create_new_message_server() {
    return MessageServerPtr(
            new MessageServer( shared_from_this(), m_factory ) );
}

MessageClientPtr
MessageComponentFactory::create_new_server_side_message_client(
                                const SocketPtr &in_socket ) {
    //TODO::REWRITE USING DELEGATED CONSTRUCTOR
    MessageClientPtr client(new MessageClient( SocketFactoryPtr() ) );
    client->connect( in_socket );
    return client;
}

MessageClientPtr
MessageComponentFactory::create_new_client_side_message_client() {
    return MessageClientPtr( new MessageClient( m_factory ) );
}

MessageComponentFactory::MessageComponentFactory(
                            const SocketFactoryPtr &in_factory )
    :m_factory( in_factory ) {
}

} //namespace Hpcl
