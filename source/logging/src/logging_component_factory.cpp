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

#include "logging_component_factory.h"
#include "remote_log_server.h"
#include "remote_logger.h"
#include "socket_factory.h"

namespace Hpcl {

LoggingComponentFactory::~LoggingComponentFactory() {
}

LoggingComponentFactoryPtr
LoggingComponentFactory::create_factory() {
    return LoggingComponentFactoryPtr(new LoggingComponentFactory());
}

RemoteLogServerPtr
LoggingComponentFactory::create_server() {
    return create_new_log_server();
}

RemoteLoggerPtr
LoggingComponentFactory::create_client() {
    return create_new_logger();
}

LoggingComponentFactory::LoggingComponentFactory()
    :m_factory( SocketFactory::create_factory() ) {
}

RemoteLogServerPtr
LoggingComponentFactory::create_new_log_server() {
    return std::shared_ptr<RemoteLogServer>(
                            new RemoteLogServer( m_factory ) );
}

RemoteLoggerPtr
LoggingComponentFactory::create_new_logger() {
    return std::shared_ptr<RemoteLogger>( new RemoteLogger( m_factory ) );
}

} //namespace Hpcl
