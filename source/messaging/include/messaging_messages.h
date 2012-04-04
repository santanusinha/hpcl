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

#ifndef HPCL_MESSAGING_MESSAGES_H
#define HPCL_MESSAGING_MESSAGES_H

namespace Hpcl {

enum MessagingMessageType {
    MESSAGE_INIT = 1,
    MESSAGE_INIT_ACK,
    MESSAGE_EXIT,
    MESSAGE_EXIT_ACK,
    MESSAGE_CUSTOM_DATA
};

struct MessagingMessage {
    MessagingMessage( MessagingMessageType in_type )
        :m_type(in_type) {
    }

    MessagingMessage( const MessagingMessage & ) = default;
    MessagingMessage &
    operator = ( const MessagingMessage & ) = default;
    ~MessagingMessage() = default;

    MessagingMessageType m_type;
};

} // namespace Hpcl

#endif //HPCL_MESSAGING_MESSAGES_H
