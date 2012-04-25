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

#ifndef HPCL_NAME_SERVICE_CLIENT_H
#define HPCL_NAME_SERVICE_CLIENT_H

#include <thread>
#include "communication_pointer_types.h"
#include "data_pointer_types.h"
#include "service_info.h"

namespace Hpcl { class DataComponentFactory; }
namespace Hpcl { class MemInfo; }

namespace Hpcl {

class NameServiceClient {
    friend class DataComponentFactory;
    public:
        ~NameServiceClient();

        NameServiceClient( const NameServiceClient &) = delete;

        NameServiceClient &
        operator =( const NameServiceClient &) = delete;

        void
        connect( const std::string &in_hostname, int32_t in_port );

        void
        disconnect();

        bool
        register_service( const std::string &in_service_name,
                            int32_t in_port );

        bool
        service_info_query( const std::string &in_service_name,
                    ServiceInfoPtr &out_info );

        bool
        unregister_service( const std::string &in_service_name,
                            int32_t in_port );

    protected:
        NameServiceClient( const SocketFactoryPtr &in_socket_factory );

    private:
        void
        on_data_received( const SocketPtr &in_socket,
                            const MemInfo &in_msg );

        void
        on_disconnect( const SocketPtr &in_socket );

        SocketFactoryPtr m_socket_factory;
        SocketPtr m_client;
        std::mutex m_mutex;
        std::condition_variable m_response_ready_cond;
        bool m_response_ready;
        ServiceInfoPtr m_info;
        bool m_status;
        bool m_stop;
};

} //namespace Hpcl
#endif //HPCL_NAME_SERVICE_CLIENT_H
