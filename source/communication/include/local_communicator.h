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

#ifndef HPCL_LOCAL_COMMUNICATOR_H
#define HPCL_LOCAL_COMMUNICATOR_H

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/exception/all.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "communication_pointer_types.h"
#include "meminfo.h"

namespace Hpcl {

class DataExchange;
typedef DataExchange * DataExchangePtr;

class LocalCommunicator {
    public:
        enum Type {
            SERVER,
            CLIENT
        };
        typedef std::shared_ptr<LocalCommunicator> Pointer;
        typedef std::weak_ptr<LocalCommunicator> WeakPointer;

        LocalCommunicator();
        ~LocalCommunicator();

        LocalCommunicator( const LocalCommunicator &) = delete;
        LocalCommunicator &
        operator = ( const LocalCommunicator &) = delete;

        void
        init( const std::string &in_id_string, Type in_type,
                size_t in_max_size);

        template<typename _Tp>
        void
        send( const _Tp &in_data );

        template<typename _Tp>
        void
        receive( _Tp &out_data );

    private:
        struct ShmRemove;
        typedef std::shared_ptr<ShmRemove> ShmRemovePtr;

        void
        create_server_memory();

        void
        create_client_memory();

        void
        send_data( const std::string &in_data );

        void
        receive_data( std::string &out_data );

        typedef boost::interprocess::shared_memory_object SharedMemory;
        typedef std::shared_ptr<SharedMemory> SharedMemoryPtr;
        typedef boost::interprocess::mapped_region MappedRegion;
        typedef std::shared_ptr<MappedRegion> MappedRegionPtr;

        std::string m_id_string;
        Type m_type;
        size_t m_max_size;
        SharedMemoryPtr m_shm;
        MappedRegionPtr m_region;
        DataExchangePtr m_exchange;
        ShmRemovePtr m_remover;
};

template<typename _Tp>
void
LocalCommunicator::send( const _Tp &in_data ) {
    std::ostringstream ofs;
    boost::archive::text_oarchive oa(ofs);
    oa & in_data;
    send_data( ofs.str() );
 }

template<typename _Tp>
void
LocalCommunicator::receive( _Tp &out_data ) {
    std::string buf;
    receive_data(buf);
    std::istringstream ifs( buf );
    boost::archive::text_iarchive ia( ifs );
    ia & out_data;
}

} //namespace Hpcl

#endif //HPCL_LOCAL_COMMUNICATOR_H
