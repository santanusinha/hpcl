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

#ifndef HPCL_HOST_INFO_H
#define HPCL_HOST_INFO_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>

namespace Hpcl {

class HostInfo {
    friend class boost::serialization::access;
    public:
        typedef boost::shared_ptr<HostInfo> Pointer;
        typedef boost::weak_ptr<HostInfo> WeakPointer;

        HostInfo();

        HostInfo( const std::string &in_hostname, int32_t in_port );

        HostInfo( const HostInfo &in_rhs );

        HostInfo &
        operator =( const HostInfo &in_rhs );

        ~HostInfo();

        const std::string &
        get_hostname() const;

        void
        set_hostname( const std::string &in_hostname );

        int32_t
        get_port();

        void
        set_port( int32_t in_port );

    private:

        template<typename _Archive>
        void
        serialize( _Archive &out_ar, uint32_t );

        std::string m_hostname;
        int32_t m_port;

};

typedef HostInfo::Pointer HostInfoPtr;
typedef HostInfo::WeakPointer HostInfoWeakPtr;

} //namespace Hpcl

#endif //HPCL_HOST_INFO_H
