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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "local_communicator.h"
#include "semantic_error.h"
#include "syscall_error.h"

namespace {

struct ControlData {
    sem_t m_wait_sem;
    size_t m_data_size;
};

}

namespace Hpcl {

LocalCommunicator::LocalCommunicator()
    :m_module_id(),
    m_data_key(),
    m_data_size(),
    m_type(),
    m_data_id(),
    m_memory() {
}

LocalCommunicator::~LocalCommunicator() {
    if( !m_memory )
    {
        return;
    }
    ControlData *ctl = reinterpret_cast<ControlData *>( m_memory );
    sem_destroy( &(ctl->m_wait_sem) );
    shmdt((void *) m_memory);
    switch( m_type )
    {
        case Type::SERVER:
        {
            shmctl(m_data_id, IPC_RMID, NULL);
            break;
        }
        case Type::CLIENT:
        {
            break;
        }
    }
}

void
LocalCommunicator::init( int32_t in_module_id,
                            const std::string &in_path,
                            size_t in_data_size,
                            Type in_type ) {
    if( 0 >= in_module_id || in_path.empty() //Invalid params
            || m_memory ) //Already initialized
    {
        throw SemanticError()
            <<errinfo_errorid( ErrorID::SEMANTIC_ERR_INVALID_PARAMETER );
    }
    m_module_id = in_module_id;
    m_data_size = in_data_size + sizeof(ControlData);
    m_type = in_type;
    if( -1 == ( m_data_key = ftok( in_path.c_str(), m_module_id ) ) )
    {
        throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("ftok");
    }

    switch( in_type )
    {
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
    return;
}

void
LocalCommunicator::send( const MemInfo &in_data ) {
    if( in_data.get_data_size() > m_data_size ) {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_INIT_NOT_DONE);
    }
    ControlData *ctl = reinterpret_cast<ControlData *>( m_memory );
    ctl->m_data_size = in_data.get_data_size();
    char *data = m_memory + sizeof(ControlData);
    ::memmove( data, in_data.get_data(), ctl->m_data_size );

    if( -1 == sem_post( &(ctl->m_wait_sem) ) )
    {
        throw SyscallError() <<boost::errinfo_errno(errno)
                        << boost::errinfo_api_function("sem_post");
    }

}

void
LocalCommunicator::receive( MemInfo &out_data ) {
    ControlData *ctl = reinterpret_cast<ControlData *>( m_memory );
    if( -1 == sem_wait( &(ctl->m_wait_sem) ) )
    {
        throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("sem_wait");
    }
    char *data = m_memory + sizeof(ControlData);
    out_data = MemInfo( data, ctl->m_data_size );
    return;
}

void
LocalCommunicator::create_server_memory() {
    if( !m_data_key )
    {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_INIT_NOT_DONE);
    }
    if( -1 == ( m_data_id = shmget( m_data_key, m_data_size,
                                    IPC_CREAT | IPC_EXCL | 0600 ) ) ) {
        throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("shmget");
    }
    m_memory = reinterpret_cast<char *>(shmat( m_data_id, NULL, 0 ));
    new(m_memory) ControlData();
    ControlData *ctl = reinterpret_cast<ControlData *>(m_memory);
    if( -1 == sem_init(&(ctl->m_wait_sem), 1, 0) )
    {
        throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("sem_init");
    }
    return;
}

void
LocalCommunicator::create_client_memory() {
    if( !m_data_key )
    {
        throw SemanticError()
                <<errinfo_errorid( ErrorID::SEMANTIC_ERR_INIT_NOT_DONE);
    }
    if( -1 == ( m_data_id = shmget( m_data_key, m_data_size, 0600 ) ) ) {
        throw SyscallError() <<boost::errinfo_errno(errno)
                            << boost::errinfo_api_function("shmget");
    }
    m_memory = reinterpret_cast<char *>(shmat( m_data_id, NULL, 0 ));
    return;
}

} //namespace Hpcl
