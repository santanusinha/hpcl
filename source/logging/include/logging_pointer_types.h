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

#ifndef HPCL_LOGGING_POINTER_TYPES_H
#define HPCL_LOGGING_POINTER_TYPES_H

#include <memory>

namespace Hpcl {

class RemoteLogServer;
typedef std::shared_ptr<RemoteLogServer>  RemoteLogServerPtr;
typedef std::weak_ptr<RemoteLogServer>  RemoteLogServerWeakPtr;

class RemoteLogger;
typedef std::shared_ptr<RemoteLogger>  RemoteLoggerPtr;
typedef std::weak_ptr<RemoteLogger>  RemoteLoggerWeakPtr;

class LoggingComponentFactory;
typedef std::shared_ptr<LoggingComponentFactory>  LoggingComponentFactoryPtr;
typedef std::weak_ptr<LoggingComponentFactory>  LoggingComponentFactoryWeakPtr;

} //namespace Hpcl
#endif //HPCL_LOGGING_POINTER_TYPES_H
