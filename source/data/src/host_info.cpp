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

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>

#include "host_info.h"

namespace Hpcl {

HostInfo::HostInfo()
    :m_hostname(),
    m_port() {
}

HostInfo::HostInfo( const std::string &in_hostname,
                    int32_t in_port )
    :m_hostname( in_hostname ),
    m_port( in_port ) {
}

HostInfo::HostInfo( const HostInfo &in_rhs )
    :m_hostname( in_rhs.m_hostname ),
    m_port( in_rhs.m_port ) {
}

HostInfo &
HostInfo::operator =( const HostInfo &in_rhs ) {
    if( this != &in_rhs ) {
        m_hostname = in_rhs.m_hostname;
        m_port = in_rhs.m_port;
    }
    return *this;
}

HostInfo::~HostInfo() {
}

const std::string &
HostInfo::get_hostname() const {
    return m_hostname;
}

void
HostInfo::set_hostname( const std::string &in_hostname ) {
    m_hostname = in_hostname;
}

int32_t
HostInfo::get_port() {
    return m_port;
}

void
HostInfo::set_port( int32_t in_port ) {
    m_port = in_port;
}

template<typename _Archive>
void
HostInfo::serialize( _Archive &out_ar, uint32_t ) {
    out_ar & m_hostname;
    out_ar & m_port;
}

template void
HostInfo::serialize<boost::archive::text_iarchive>(
                boost::archive::text_iarchive & ar, const unsigned int);

template void
HostInfo::serialize<boost::archive::text_oarchive>(
                boost::archive::text_oarchive & ar, const unsigned int);


} //namespace Hpcl

