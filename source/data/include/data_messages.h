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

#ifndef HPCL_DATA_MESSAGES_H
#define HPCL_DATA_MESSAGES_H

#include <strings.h>

namespace Hpcl {

enum DataMessageType {
    DATA_SERVICE_REGISTER,
    DATA_SERVICE_UNREGISTER,
    DATA_SERVICE_INFO_REQUEST,
    DATA_SERVICE_GENERAL_REPLY,
    DATA_SERVICE_INFO_REPLY
};

struct DataMessageHeader {
    DataMessageType m_type;

    DataMessageHeader( DataMessageType in_type )
        :m_type( in_type ) {
    }

    DataMessageHeader( const DataMessageHeader &) = default;
    DataMessageHeader &
    operator =( const DataMessageHeader &) = default;
};

struct DataServiceMessage {
    DataMessageHeader m_header;
    char m_data[BUFSIZ];
    bool m_status;

    DataServiceMessage( DataMessageType in_type )
        :m_header( in_type ),
        m_status() {
            ::bzero(m_data,BUFSIZ);
    }
};

struct ServiceRequestData {
    std::string m_service_name;
    std::string m_hostname;
    int32_t m_port;

    ServiceRequestData()
        :m_service_name(),
        m_hostname(),
        m_port() {
    }

    template<typename _Archive>
    void
    serialize( _Archive &inout_ar, int32_t ) {
        inout_ar & m_service_name;
        inout_ar & m_hostname;
        inout_ar & m_port;
    }
};

} //namespace Hpcl
#endif //HPCL_DATA_MESSAGES_H
