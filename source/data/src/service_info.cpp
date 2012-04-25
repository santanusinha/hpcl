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

#include <algorithm>
#include <iterator>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "service_info.h"

namespace {

using namespace Hpcl;

struct Finder {
    Finder( const HostInfoPtr &in_obj )
       :m_obj( in_obj ) {
    }
    bool
    operator()( const HostInfoPtr &in_lhs ) const {
        return in_lhs->get_hostname() == m_obj->get_hostname()
                && in_lhs->get_port() == m_obj->get_port();
    }
    HostInfoPtr m_obj;
};

}

namespace Hpcl {
ServiceInfo::ServiceInfo()
    :m_service_name(),
    m_host_info_list() {
}

ServiceInfo::ServiceInfo( const std::string &in_name )
    :m_service_name( in_name ),
    m_host_info_list() {
}

ServiceInfo::ServiceInfo( const ServiceInfo &in_rhs )
    :m_service_name( in_rhs.m_service_name ),
    m_host_info_list( in_rhs.m_host_info_list ) {
}

ServiceInfo &
ServiceInfo::operator =( const ServiceInfo &in_rhs ) {
    if( this != &in_rhs )
    {
        m_service_name = in_rhs.m_service_name;
        m_host_info_list = in_rhs.m_host_info_list;
    }
    return *this;
}

const std::string &
ServiceInfo::get_service_name() const {
    return m_service_name;
}

void
ServiceInfo::set_service_name( const std::string &in_service_name ) {
    m_service_name = in_service_name;
}

void
ServiceInfo::get_host_info_list( HostInfoList &out_host_info_list ) {
    copy( m_host_info_list.begin(), m_host_info_list.end(),
                back_inserter( out_host_info_list ) );
}

void
ServiceInfo::add_host_info( const HostInfoPtr &in_host_info ) {
    m_host_info_list.push_back( in_host_info );
}

void
ServiceInfo::erase_host_info( const HostInfoPtr &in_host_info ) {
    auto it = std::find_if( m_host_info_list.begin(), m_host_info_list.end(),
                            Finder( in_host_info ) );
    if( it != m_host_info_list.end() ) {
        m_host_info_list.erase( it );
    }
}

bool
ServiceInfo::has_host_info() const {
    return !m_host_info_list.empty();
}

template<typename _Archive>
void
ServiceInfo::serialize( _Archive &out_ar, uint32_t) {
    out_ar & m_service_name;
    out_ar & m_host_info_list;
}

template void
ServiceInfo::serialize<boost::archive::text_iarchive>(
                boost::archive::text_iarchive & ar, const unsigned int);

template void
ServiceInfo::serialize<boost::archive::text_oarchive>(
                boost::archive::text_oarchive & ar, const unsigned int);


} //namespace Hpcl
