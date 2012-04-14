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

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include "local_communicator.h"
#include "semantic_error.h"

namespace {

}

namespace Hpcl {

struct DataExchange {
    boost::interprocess::interprocess_mutex m_mutex;
    boost::interprocess::interprocess_condition m_empty_cond;
    boost::interprocess::interprocess_condition m_full_cond;
    bool m_has_data_from_client;
    bool m_has_data_from_server;
    size_t m_data_size;
    DataExchange()
        :m_mutex(),
        m_empty_cond(),
        m_full_cond(),
        m_has_data_from_client(),
        m_has_data_from_server(),
        m_data_size() {
    }

    DataExchange( const DataExchange &) = delete;
    DataExchange &
    operator =( const DataExchange &) = delete;
};

struct LocalCommunicator::ShmRemove {
    ShmRemove(const std::string &in_id )
        :m_id( in_id ) {
        boost::interprocess::shared_memory_object::remove(m_id.c_str());
    }
    ~ShmRemove() {
        boost::interprocess::shared_memory_object::remove(m_id.c_str());
    }
    std::string m_id;
};


LocalCommunicator::LocalCommunicator()
    :m_id_string(),
    m_type(),
    m_max_size(),
    m_shm(),
    m_region(),
    m_exchange(),
    m_remover() {
}

LocalCommunicator::~LocalCommunicator() {
}

void
LocalCommunicator::init( const std::string &in_id_string, Type in_type,
        size_t in_max_size) {

    m_id_string = in_id_string;
    m_type = in_type;
    m_max_size = in_max_size;
    switch( m_type ) {
        case Type::SERVER:
        {
            create_server_memory();
            break;
        }
        case Type::CLIENT:
        {
            create_client_memory();
            break;
        }
    }
}

void
LocalCommunicator::create_server_memory() {
    m_remover = std::make_shared<ShmRemove>( m_id_string );
    try {
        m_shm = std::make_shared<SharedMemory>(
                                boost::interprocess::create_only,
                                m_id_string.c_str(),
                                boost::interprocess::read_write);
        m_shm->truncate(sizeof(DataExchange) + m_max_size);
        m_region =std::make_shared<MappedRegion>( (*m_shm),
                                    boost::interprocess::read_write);
        void * addr = m_region->get_address();
        m_exchange = new (addr)DataExchange();
    }
    catch(boost::interprocess::interprocess_exception &ex){
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_IPC_FAILURE);
    }

}

void
LocalCommunicator::create_client_memory() {
    try{
        m_shm = std::make_shared<SharedMemory>(
                                boost::interprocess::open_only,
                                m_id_string.c_str(),
                                boost::interprocess::read_write);
        m_region =std::make_shared<MappedRegion>( (*m_shm),
                                    boost::interprocess::read_write);
        void * addr = m_region->get_address();
        m_exchange = reinterpret_cast<DataExchange *>(addr);
    }
    catch(boost::interprocess::interprocess_exception &ex){
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_IPC_FAILURE);
    }
}

void
LocalCommunicator::send_data( const std::string &in_data ) {
    if( !m_exchange ) {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_INIT_NOT_DONE);
    }
    boost::interprocess::scoped_lock<
            boost::interprocess::interprocess_mutex> lock(
                                                m_exchange->m_mutex );
    if( (SERVER == m_type && m_exchange->m_has_data_from_server )
            || (CLIENT == m_type && m_exchange->m_has_data_from_client) ) {
        m_exchange->m_empty_cond.wait( lock );
    }
    char *data = reinterpret_cast<char *>(m_exchange) + sizeof(DataExchange);
    std::strncpy( data, in_data.c_str(), in_data.size());
    m_exchange->m_data_size = in_data.size();
    m_exchange->m_has_data_from_server = (SERVER == m_type) && true;
    m_exchange->m_has_data_from_client = (CLIENT == m_type) && true;
    m_exchange->m_full_cond.notify_one();
    return;
}

void
LocalCommunicator::receive_data( std::string &out_data ) {
    if( !m_exchange ) {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_INIT_NOT_DONE);
    }
    boost::interprocess::scoped_lock<
            boost::interprocess::interprocess_mutex> lock(
                                                m_exchange->m_mutex );
    if( (SERVER == m_type && !m_exchange->m_has_data_from_client )
            || (CLIENT == m_type && !m_exchange->m_has_data_from_server ) ) {
        m_exchange->m_full_cond.wait(lock);
    }
    out_data = std::string(reinterpret_cast<char *>(m_exchange) + sizeof(DataExchange),
                            m_exchange->m_data_size);
    m_exchange->m_data_size = 0;
    m_exchange->m_has_data_from_server = (CLIENT == m_type) && false;
    m_exchange->m_has_data_from_client = (SERVER == m_type) && false;
    m_exchange->m_empty_cond.notify_one();
    return;
}

} //namespace Hpcl
