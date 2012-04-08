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

#ifndef HPCL_REMOTE_LOG_SERVER_H
#define HPCL_REMOTE_LOG_SERVER_H

#include <exception>
#include <string>

#include <boost/signals2.hpp>

#include "common_pointer_types.h"
#include "communication_pointer_types.h"
#include "logging_pointer_types.h"

namespace Hpcl { class MemInfo; }

namespace Hpcl {

class RemoteLogServer {
    friend class LoggingComponentFactory;
    public:
        typedef RemoteLogServerPtr Pointer;
        typedef RemoteLogServerWeakPtr WeakPointer;

        typedef boost::signals2::signal<
                        void(const std::string &)> SignalLogMessageReceived;

        virtual
        ~RemoteLogServer();

        RemoteLogServer( const RemoteLogServer &) = delete;
        RemoteLogServer &
        operator = ( const RemoteLogServer &) = delete;

        SignalLogMessageReceived &
        signal_log_message_received();

        void
        listen( int32_t in_port, std::exception_ptr &out_error );

        void
        stop();

    protected:
        explicit
        RemoteLogServer( const SocketFactoryPtr &in_factory );

        virtual void
        on_log_message_received( const std::string &in_log_message );

    private:
        void
        on_create( const SocketPtr &in_socket );

        void
        on_log_received( const SocketPtr &in_socket, const MemInfo &in_info );

        SocketFactoryPtr m_factory;
        ServerSocketPtr m_server;
        SignalLogMessageReceived m_signal_log_message_received;
        ThreadPtr m_listen_thread;
};

} //namespace Hpcl


#endif //HPCL_REMOTE_LOG_SERVER_H
