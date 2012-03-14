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

#include "exec_client.h"
#include "exec_message.h"
#include "print.h"

namespace Hpcl {

ExecClient::ExecClient( const SocketFactoryPtr &in_factory )
    :m_factory( in_factory ),
    m_socket(),
    m_signal_process_completed() {
}

ExecClient::~ExecClient() {
    if( m_socket ) {
        m_socket->shutdown();
    }
}

ExecClient::SignalProcessCompleted &
ExecClient::signal_process_completed() {
    return m_signal_process_completed;
}

void
ExecClient::execute( const std::string &in_server, int32_t in_port,
                 const std::string &in_command ) {
    if( m_socket && m_socket->get_is_connected() ) {
        m_socket->shutdown();
    }
    m_socket = m_factory->create_on_client( in_server, in_port ); 
    m_socket->signal_data_received().connect( boost::bind(
                        std::mem_fn( &ExecClient::handle_server_response ),
                        this, _1, _2));
    ExecMessage execCmd;
    execCmd.m_msg_type = ExecMessageCode::EXEC_COMMAND;
    ::strcpy(execCmd.m_command, in_command.c_str() );
    m_socket->send_data( MemInfo( reinterpret_cast<char *>( &execCmd ),
                            sizeof(ExecMessage) ) );
    hpcl_debug("Child process request sent\n" );
}

void
ExecClient::on_process_completed( int32_t in_status ) {
    hpcl_debug("Child process exited with status %d\n", in_status );
}

void
ExecClient::handle_server_response(const SocketPtr &in_socket,
                                     const MemInfo &in_data ) {
    if( in_socket ) {
    }
    const ExecReplyMessage *msg
                    = reinterpret_cast<const ExecReplyMessage *>(
                                                    in_data.get_data());
    on_process_completed( msg->m_status );
    m_signal_process_completed( msg->m_status );
}

void
ExecClient::handle_remote_disconnect( const SocketPtr &in_socket ) {
    if( in_socket ) {
        //TODO
    }
}

} //namespace Hpcl
