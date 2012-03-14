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

#ifndef HPCL_EXEC_CLIENT_H
#define HPCL_EXEC_CLIENT_H

#include <cstdint>
#include <boost/signals2.hpp>

#include "socket.h"
#include "socket_factory.h"

namespace Hpcl {

class ExecClient {
    public:
        typedef boost::signals2::signal<
                            void(int32_t)> SignalProcessCompleted;

        ExecClient( const SocketFactoryPtr &in_factory );

        virtual
        ~ExecClient();

        ExecClient( const ExecClient &) = delete;
        ExecClient &
        operator = ( const ExecClient &) = delete;

        SignalProcessCompleted &
        signal_process_completed();

        void
        execute( const std::string &in_server, int32_t in_port,
                 const std::string &in_command );

    protected:
        virtual void
        on_process_completed( int32_t in_status );

    private:
        void
        handle_server_response( const SocketPtr &in_socket,
                                const MemInfo &in_data ); 

        void
        handle_remote_disconnect( const SocketPtr &in_socket );

        SocketFactoryPtr m_factory;
        SocketPtr m_socket;
        SignalProcessCompleted m_signal_process_completed;

};
} //namespace Hpcl

#endif //HPCL_EXEC_CLIENT_H
