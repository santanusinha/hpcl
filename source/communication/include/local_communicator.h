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
#include <sys/ipc.h>

#include <boost/exception/all.hpp>

#include "meminfo.h"

namespace Hpcl {

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
        init( int32_t in_module_id,
              const std::string &in_path,
              size_t in_data_size,
              Type in_type );

        void
        send( const MemInfo &in_data );

        void
        receive( MemInfo &out_data );

    private:
        void
        create_server_memory();

        void
        create_client_memory();

        int32_t m_module_id;
        key_t m_data_key;
        size_t m_data_size;
        Type m_type;
        int32_t m_data_id;
        char *m_memory;
};

typedef LocalCommunicator::Pointer LocalCommunicatorPtr;
typedef LocalCommunicator::WeakPointer LocalCommunicatorWeakPtr;

} //namespace Hpcl

#endif //HPCL_LOCAL_COMMUNICATOR_H
