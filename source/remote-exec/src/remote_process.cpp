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

#include "remote_process.h"
#include "exec_message.h"
#include "print.h"
#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

RemoteProcess::RemoteProcess( const SocketFactoryPtr &in_factory )
    :enable_shared_from_this(),
    m_pid(),
    m_factory( in_factory ),
    m_socket(),
    m_signal_process_started(),
    m_signal_process_completed(),
    m_proc_complete_mutex(),
    m_proc_end_event(),
    m_process_is_active() {
}

RemoteProcess::~RemoteProcess() {
    disconnect();
}

RemoteProcess::SignalProcessStarted &
RemoteProcess::signal_process_started() {
    return m_signal_process_started;
}

RemoteProcess::SignalProcessCompleted &
RemoteProcess::signal_process_completed() {
    return m_signal_process_completed;
}

pid_t
RemoteProcess::get_pid() const {
    return m_pid;
}

void
RemoteProcess::connect(const std::string &in_server, int32_t in_port) {
    m_socket = m_factory->create_client( true );
    m_socket->signal_data_received().connect( boost::bind(
                    std::mem_fn( &RemoteProcess::handle_server_response ),
                        this, _1, _2));
    m_socket->connect_to_server( in_server, in_port ); 
}

void
RemoteProcess::disconnect() {
    if( m_socket && m_socket->get_is_connected() ) {
        std::unique_lock<std::mutex> l(m_proc_complete_mutex);
        while(m_process_is_active) {
            m_proc_end_event.wait(l);
        }
        m_socket->shutdown();
    }
}

void
RemoteProcess::execute( const std::string &in_command ) {
    ExecMessage execCmd;
    ::strcpy(execCmd.m_command, in_command.c_str() );
    m_socket->send_data( MemInfo( reinterpret_cast<char *>( &execCmd ),
                            sizeof(ExecMessage) ) );
    std::lock_guard<std::mutex> l(m_proc_complete_mutex);
    m_process_is_active = true;
    hpcl_debug("Child process request sent\n" );
}

void
RemoteProcess::on_process_started( pid_t in_pid,
                                const RemoteProcessPtr &in_proc ) {
    if( in_proc ) {
    }
    hpcl_debug("Child process started with pid %d\n", in_pid );
}

void
RemoteProcess::on_process_completed( pid_t in_pid, int32_t in_status ) {
    hpcl_debug("Child process exited with status %d\n", in_status );
}

void
RemoteProcess::handle_server_response(const SocketPtr &in_socket,
                                     const MemInfo &in_data ) {
    if( in_socket ) {
    }
    const ExecMessageHeader *header
                    = reinterpret_cast<const ExecMessageHeader *>(
                                                    in_data.get_data());
    switch( header->m_msg_type ) {
        case EXEC_PROCESS_STARTED:
        {
            const ExecProcessStartedMessage *msg
                = reinterpret_cast<const ExecProcessStartedMessage *>(
                                                in_data.get_data());
            RemoteProcessPtr this_ptr = shared_from_this();
            m_pid = msg->m_pid;
            on_process_started( msg->m_pid, this_ptr );
            m_signal_process_started( msg->m_pid, this_ptr );
            break;
        }
        case EXEC_PROCESS_COMPLETED:
        {
            const ExecReplyMessage *msg
                = reinterpret_cast<const ExecReplyMessage *>(
                                                in_data.get_data());
            on_process_completed( msg->m_pid, msg->m_status );
            m_signal_process_completed( msg->m_pid, msg->m_status );
            std::lock_guard<std::mutex> l(m_proc_complete_mutex);
            m_process_is_active = false;
            m_proc_end_event.notify_one();
            break;
        }
        default:
        {
            hpcl_error("Should not come here...\n");
        }
    }
}

void
RemoteProcess::handle_remote_disconnect( const SocketPtr &in_socket ) {
    if( in_socket ) {
        //TODO
    }
}

} //namespace Hpcl
