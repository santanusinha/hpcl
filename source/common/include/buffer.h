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

#ifndef HPCL_BUFFER_H
#define HPCL_BUFFER_H

namespace Hpcl {

class Buffer {
    public:
        void *
        get_memory( size_t in_size );

        size_t
        get_size() const;

        void *
        get_buffer() const;

        Buffer();

        Buffer( const Buffer & ) = default;

        Buffer &
        operator = ( const Buffer & ) = default;

        ~Buffer();

    private:
        void *m_buffer;
        size_t m_current_size;
        size_t m_alloc_size;
};

} //namespace Hpcl

#endif //HPCL_BUFFER_H
