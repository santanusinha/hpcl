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

#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "data_messages.h"
#include "meminfo.h"
#include "name_service_client.h"
#include "semantic_error.h"
#include "socket.h"
#include "socket_factory.h"
#include "syscall_error.h"

namespace Hpcl {

NameServiceClient::NameServiceClient( const SocketFactoryPtr &in_socket_factory )
        : m_socket_factory( in_socket_factory ),
        m_client(),
        m_mutex(),
        m_response_ready_cond(),
        m_response_ready(),
        m_info(),
        m_status(),
        m_stop() {
}

NameServiceClient::~NameServiceClient() {
    try {
        disconnect();
    }
    catch(...) {
    }
}

void
NameServiceClient::connect( const std::string &in_hostname, int32_t in_port ) {
    m_client = m_socket_factory->create_socket( true );
    m_client->signal_data_received().connect( boost::bind(
                std::mem_fn( &NameServiceClient::on_data_received ),
                this, _1, _2 ));
    m_client->signal_remote_disconnect().connect( boost::bind(
                std::mem_fn( &NameServiceClient::on_disconnect ),
                this, _1 ));
    m_client->connect_to_server( in_hostname, in_port );
}

void
NameServiceClient::disconnect() {
    if( m_client )
    {
        std::unique_lock<std::mutex> l( m_mutex );
        m_stop = true;
        m_client->shutdown();
        std::cout<<"Socket shutdown"<<std::endl;
        m_client.reset();
        m_response_ready_cond.notify_one();
    }
    return;
}

bool
NameServiceClient::register_service( const std::string &in_service_name,
                                int32_t in_port ) {
    if( !m_client || !m_client->get_is_connected() ) {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_NOT_CONNECTED);
    }
    char hostname[NAME_MAX];
    if( -1 == gethostname( hostname, NAME_MAX ) ) {
            throw SyscallError() <<boost::errinfo_errno(errno)
                    << boost::errinfo_api_function("gethostname");
    }
    ServiceRequestData request;
    request.m_service_name = in_service_name;
    request.m_hostname = hostname;
    request.m_port = in_port;
    std::ostringstream sout;
    {
        boost::archive::text_oarchive oa( sout );
        oa & request;
    }
    DataServiceMessage message( DATA_SERVICE_REGISTER );
    std::strncpy( message.m_data, sout.str().c_str(), sout.str().size());
    MemInfo data( reinterpret_cast<char *>( &message ),
                    sizeof(DataServiceMessage));
    m_client->send_data( data );
    std::unique_lock<std::mutex> l( m_mutex );
    while( !m_stop && !m_response_ready ) {
        m_response_ready_cond.wait( l );
    }
    if( m_stop ) {
        return false;
    }
    bool status = m_status;
    m_status = false;
    m_response_ready = false;
    return status;
}

bool
NameServiceClient::service_info_query( const std::string &in_service_name,
                                            ServiceInfoPtr &out_info ) {
    if( !m_client || !m_client->get_is_connected() ) {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_NOT_CONNECTED);
    }
    ServiceRequestData request;
    request.m_service_name = in_service_name;
    std::ostringstream sout;
    {
        boost::archive::text_oarchive oa( sout );
        oa & request;
    }
    DataServiceMessage message( DATA_SERVICE_INFO_REQUEST );
    std::strncpy( message.m_data, sout.str().c_str(), sout.str().size());
    MemInfo data( reinterpret_cast<char *>( &message ),
                    sizeof(DataServiceMessage));
    m_client->send_data( data );
    std::unique_lock<std::mutex> l( m_mutex );
    while( !m_stop && !m_response_ready ) {
        m_response_ready_cond.wait( l );
    }
    if( m_stop ) {
        return false;
    }
    bool status = m_status;
    m_status = false;
    m_response_ready = false;
    out_info = m_info;
    m_info.reset();
    return status;
}

bool
NameServiceClient::unregister_service( const std::string &in_service_name,
                                    int32_t in_port ) {
    if( !m_client || !m_client->get_is_connected() ) {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_NOT_CONNECTED);
    }
    char hostname[NAME_MAX];
    if( -1 == gethostname( hostname, NAME_MAX ) ) {
            throw SyscallError() <<boost::errinfo_errno(errno)
                    << boost::errinfo_api_function("gethostname");
    }
    ServiceRequestData request;
    request.m_service_name = in_service_name;
    request.m_hostname = hostname;
    request.m_port = in_port;
    std::ostringstream sout;
    {
        boost::archive::text_oarchive oa( sout );
        oa & request;
    }
    DataServiceMessage message( DATA_SERVICE_UNREGISTER );
    std::strncpy( message.m_data, sout.str().c_str(), sout.str().size());
    MemInfo data( reinterpret_cast<char *>( &message ),
                    sizeof(DataServiceMessage));
    m_client->send_data( data );
    std::unique_lock<std::mutex> l( m_mutex );
    while( !m_stop && !m_response_ready ) {
        m_response_ready_cond.wait( l );
    }
    if( m_stop ) {
        return false;
    }
    bool status = m_status;
    m_status = false;
    m_response_ready = false;
    return status;
}

void
NameServiceClient::on_data_received( const SocketPtr &in_socket,
                            const MemInfo &in_msg ) {
    std::unique_lock<std::mutex> l( m_mutex );
    const DataServiceMessage *pMsg
                = reinterpret_cast<const DataServiceMessage *>(
                                                    in_msg.get_data());
    m_status = pMsg->m_status;
    switch( pMsg->m_header.m_type ) {
        case DataMessageType::DATA_SERVICE_INFO_REPLY:
        {
            std::string data(pMsg->m_data);
            std::istringstream sin( data );
            boost::archive::text_iarchive ia( sin );
            ia & m_info;
            break;
        }
        default:
        {
        }
    }
    m_response_ready = true;
    m_response_ready_cond.notify_one();
    return;
} 

void
NameServiceClient::on_disconnect( const SocketPtr &in_socket ) {
    std::cout<<"Disconnect"<<std::endl;
}

} //namespace Hpcl

