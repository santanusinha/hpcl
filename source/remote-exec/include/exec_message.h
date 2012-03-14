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

#ifndef HPCL_EXEC_MESSAGE_H
#define HPCL_EXEC_MESSAGE_H

namespace Hpcl {

enum ExecMessageCode {
    EXEC_COMMAND,
    EXEC_PIN_COMMAND,
    EXEC_DEBUG_COMMAND,
};

struct ExecMessage {
    ExecMessageCode m_msg_type;
    char m_command[PATH_MAX];
    char m_tool_command[PATH_MAX]; //Used for PIN

    ExecMessage()
        :m_msg_type( ExecMessageCode::EXEC_COMMAND ) {
        ::bzero( m_command, PATH_MAX );
        ::bzero( m_tool_command, PATH_MAX );
    }

    ExecMessage( const ExecMessage &in_rhs )
        :m_msg_type( in_rhs.m_msg_type ) {
        ::memcpy( m_command, in_rhs.m_command, PATH_MAX );
        ::memcpy( m_tool_command, in_rhs.m_tool_command, PATH_MAX );
    }

    ExecMessage &
    operator = ( const ExecMessage &in_rhs ) {
        if( this != &in_rhs ) {
            m_msg_type = in_rhs.m_msg_type;
            ::memcpy( m_command, in_rhs.m_command, PATH_MAX );
            ::memcpy( m_tool_command, in_rhs.m_tool_command, PATH_MAX );
        }
        return *this;
    }

    ~ExecMessage() = default;
};

struct ExecReplyMessage {
    int32_t m_status;
    int32_t m_debug_port;

    ExecReplyMessage()
        :m_status(),
        m_debug_port() {
    }

    ExecReplyMessage( const ExecReplyMessage &in_rhs ) = default;
    ExecReplyMessage &
    operator = ( const ExecReplyMessage &in_rhs ) = default;

    ~ExecReplyMessage() = default;
};

}

#endif //HPCL_EXEC_MESSAGE_H
