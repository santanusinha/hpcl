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

#include <cstring>
#include <sstream>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "data_server.h"
#include "data_messages.h"
#include "meminfo.h"
#include "server_socket.h"
#include "socket_factory.h"
#include "socket.h"

namespace Hpcl {
DataServer::DataServer( const SocketFactoryPtr &in_socket_factory )
    :m_socket_factory( in_socket_factory ),
    m_server() {
}

DataServer::~DataServer() {
}

void
DataServer::listen( int32_t in_port, std::exception_ptr &out_error ) {
    m_server = m_socket_factory->create_server_socket();
    m_server->signal_client_created().connect( boost::bind(
                        std::mem_fn( &DataServer::on_client_created),
                        this, _1 ));
    m_server->listen( in_port, out_error );
    return;
}

void
DataServer::stop() {
    m_server->shutdown();
}

void
DataServer::on_client_created( const SocketPtr &in_client ) {
    in_client->signal_data_received().connect( boost::bind(
                    std::mem_fn(&DataServer::on_data_received),
                    this, _1, _2));
    m_server->signal_client_created().connect( boost::bind(
                        std::mem_fn( &DataServer::on_client_disconnect),
                        this, _1 ));
}
void
DataServer::on_client_disconnect( const SocketPtr &in_client ) {
    std::cout<<"Client disconnected"<<std::endl;
}

void
DataServer::on_data_received(
            const SocketPtr &in_socket, const MemInfo &in_msg ) {
    const DataServiceMessage *pMsg
            = reinterpret_cast<const DataServiceMessage *>(
                                    in_msg.get_data());
    DataServiceMessage reply( DATA_SERVICE_GENERAL_REPLY );
    bool status = false;
    switch( pMsg->m_header.m_type ) {
        case DataMessageType::DATA_SERVICE_REGISTER:
        {
            std::string buf = pMsg->m_data;
            std::istringstream ifs( buf );
            boost::archive::text_iarchive ia( ifs );
            ServiceRequestData data;
            ia & data;
            status = on_service_register_request(
                                        data.m_service_name,
                                        data.m_hostname, data.m_port );
            break;
        }
        case DataMessageType::DATA_SERVICE_UNREGISTER:
        {
            std::string buf = pMsg->m_data;
            std::istringstream ifs( buf );
            boost::archive::text_iarchive ia( ifs );
            ServiceRequestData data;
            ia & data;
            status = on_service_unregister_request(
                                        data.m_service_name,
                                        data.m_hostname, data.m_port );
            break;
        }
        case DataMessageType::DATA_SERVICE_INFO_REQUEST:
        {
            std::string buf = pMsg->m_data;
            std::istringstream ifs( buf );
            boost::archive::text_iarchive ia( ifs );
            ServiceRequestData data;
            ia & data;
            ServiceInfoPtr info;
            status = on_service_info_request( data.m_service_name, info );
            if( info ) {
                std::ostringstream sout;
                boost::archive::text_oarchive oa(sout);
                oa & info;
                std::strncpy( reply.m_data,
                                sout.str().c_str(), sout.str().size());
            }
            reply.m_header.m_type =  DATA_SERVICE_INFO_REPLY;
            break;
        }
        default:
        {
        }
    }
    reply.m_status = status;
    MemInfo replyMsg( reinterpret_cast<char *>( &reply ),
                        sizeof( DataServiceMessage ) );
    in_socket->send_data( replyMsg );
}

} //namespace Hpcl
