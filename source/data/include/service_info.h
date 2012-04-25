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

#ifndef HPCL_SERVICE_INFO_H
#define HPCL_SERVICE_INFO_H

#include <string>
#include <list>

#include <boost/serialization/access.hpp>

#include "host_info.h"

namespace Hpcl {

class ServiceInfo {
    friend class boost::serialization::access;
    public:
        typedef boost::shared_ptr<ServiceInfo> Pointer;
        typedef boost::weak_ptr<ServiceInfo> WeakPointer;
        typedef std::list<HostInfoPtr> HostInfoList;

        ServiceInfo();

        ServiceInfo( const std::string &in_name );

        ServiceInfo( const ServiceInfo &in_rhs );

        ServiceInfo &
        operator =( const ServiceInfo &in_rhs );

        const std::string &
        get_service_name() const;

        void
        set_service_name( const std::string &in_service_name );

        void
        get_host_info_list( HostInfoList &out_host_info_list );

        void
        add_host_info( const HostInfoPtr &in_host_info );

        void
        erase_host_info( const HostInfoPtr &in_host_info );

        bool
        has_host_info() const;

    private:
        template<typename _Archive>
        void
        serialize( _Archive &out_ar, uint32_t);

        std::string m_service_name;
        HostInfoList m_host_info_list;
};

typedef ServiceInfo::Pointer ServiceInfoPtr;
typedef ServiceInfo::WeakPointer ServiceInfoWeakPtr;

} //namespace Hpcl
#endif //HPCL_SERVICE_INFO_H
