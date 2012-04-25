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

#ifndef HPCL_DATA_SERVER_H
#define HPCL_DATA_SERVER_H

#include <exception>

#include <boost/serialization/string.hpp>

#include "common_pointer_types.h"
#include "communication_pointer_types.h"
#include "data_pointer_types.h"
#include "service_info.h"

namespace Hpcl { class MemInfo; }
namespace Hpcl { class DataComponentFactory; }

namespace Hpcl {
class DataServer {
    public:
        DataServer( const DataServer &) = delete;
        DataServer &
        operator =( const DataServer &) = delete;

        virtual
        ~DataServer();

        void
        listen( int32_t in_port, std::exception_ptr &out_error );

        void
        stop();

    protected:
        DataServer( const SocketFactoryPtr &in_socket_factory );

    private:
        virtual bool
        on_service_register_request( const std::string &in_service_name,
                        const std::string &in_hostname, int32_t in_port ) = 0;

        virtual bool
        on_service_unregister_request( const std::string &in_service_name,
                        const std::string &in_hostname, int32_t in_port ) = 0;

        virtual bool
        on_service_info_request( const std::string &in_service_name,
                    ServiceInfoPtr &out_service_info ) = 0;

        void
        on_client_created( const SocketPtr &in_client );

        void
        on_data_received( const SocketPtr &in_socket,
                            const MemInfo &in_msg );

        void
        on_client_disconnect( const SocketPtr &in_client );

        SocketFactoryPtr m_socket_factory;
        ServerSocketPtr m_server;
};

} //namespace Hpcl

#endif //HPCL_DATA_SERVER_H
