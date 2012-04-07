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

#ifndef HPCL_REMOTE_LOGGER_H
#define HPCL_REMOTE_LOGGER_H

#include <string>

#include "communication_pointer_types.h"
#include "logging_pointer_types.h"

namespace Hpcl {

class RemoteLogger {
    friend class LoggingComponentFactory;
    public:
        typedef RemoteLoggerPtr Pointer;
        typedef RemoteLoggerWeakPtr WeakPointer;

        virtual
        ~RemoteLogger();

        RemoteLogger( const RemoteLogger &) = delete;
        RemoteLogger &
        operator =( const RemoteLogger &) = delete;

        void
        connect( const std::string &in_host, int32_t in_port );

        void
        disconnect();

        void
        log( const std::string &in_log_message );

    protected:
        RemoteLogger( const SocketFactoryPtr &in_factory );

    private:
        SocketFactoryPtr m_factory;
        SocketPtr m_socket;
};

} //namespace Hpcl

#endif //HPCL_REMOTE_LOGGER_H
