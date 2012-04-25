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

#ifndef HPCL_LOGGING_COMPONENT_FACTORY_H
#define HPCL_LOGGING_COMPONENT_FACTORY_H

#include "communication_pointer_types.h"
#include "logging_pointer_types.h"

namespace Hpcl {

class LoggingComponentFactory {
    public:
        typedef LoggingComponentFactoryPtr Pointer;
        typedef LoggingComponentFactoryWeakPtr WeakPointer;

        virtual
        ~LoggingComponentFactory();

        LoggingComponentFactory( const LoggingComponentFactory &) = delete;
        LoggingComponentFactory &
        operator = ( const LoggingComponentFactory &) = delete;

        static LoggingComponentFactoryPtr
        create_factory();

        RemoteLogServerPtr
        create_remote_log_server();

        RemoteLoggerPtr
        create_remote_logger();

    protected:
        LoggingComponentFactory();

        virtual RemoteLogServerPtr
        create_new_remote_log_server();

        virtual RemoteLoggerPtr
        create_new_remote_logger();

    private:
        SocketFactoryPtr m_factory;
};

} //namespace Hpcl

#endif //HPCL_LOGGING_COMPONENT_FACTORY_H
