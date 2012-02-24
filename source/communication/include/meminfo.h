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

#ifndef HPCL_MEMINFO_H
#define HPCL_MEMINFO_H

#include <cstddef>

namespace Hpcl {
class MemInfo {
    public:
        MemInfo( const char * in_data, size_t in_data_size );
        MemInfo( const MemInfo &in_rhs ) = default;
        MemInfo &
        operator = ( const MemInfo &in_rhs ) = default;
        ~MemInfo() = default;

        inline const char *
        get_data() const;

        inline size_t
        get_data_size() const;

    private:
        const char * m_data;
        size_t m_data_size;
};

inline const char *
MemInfo::get_data() const {
    return m_data;
}

inline size_t
MemInfo::get_data_size() const {
    return m_data_size;
}


} //namespace Hpcl

#endif //HPCL_MEMINFO_H
