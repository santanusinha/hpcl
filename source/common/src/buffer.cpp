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

#include <cstdlib>
#include <cstdio>

#include "buffer.h"

namespace Hpcl {

void *
Buffer::get_memory( size_t in_size ) {
    if( nullptr == m_buffer
            || m_current_size + in_size > m_alloc_size )
    {
        while( m_current_size + in_size > m_alloc_size )
        {
            m_alloc_size = m_alloc_size << 1;
        }
        m_buffer = realloc( m_buffer, m_alloc_size );
        //TODO::ERROR
    }
    size_t old_buf = m_current_size;
    m_current_size += in_size;
    return reinterpret_cast<char *>(m_buffer) + old_buf;
}

size_t
Buffer::get_size() const {
    return m_current_size;
}

void *
Buffer::get_buffer() const {
    return m_buffer;
}

Buffer::Buffer()
    :m_buffer(),
    m_current_size(),
    m_alloc_size(BUFSIZ) {
}

Buffer::~Buffer() {
    if( m_buffer ) {
        free( m_buffer );
    }
}

} //namespace Hpcl

