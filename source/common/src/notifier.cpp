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
#endif

#include <unistd.h>
#include <fcntl.h>

#include "notifier.h"
#include "syscall_error.h"

namespace Hpcl {
Notifier::Notifier()
    :m_request_fd(),
    m_wait_fd(),
    m_event_type() {
}

void
Notifier::init() {
    int pipefd[2];
    if( -1 == pipe(pipefd) )
    {
        throw SyscallError()<<boost::errinfo_errno(errno)
                            <<boost::errinfo_api_function("pipe");
    }
    m_wait_fd = pipefd[0];
    m_request_fd = pipefd[1];
    if( -1 == fcntl(m_request_fd, F_SETFL,O_NONBLOCK) )
    {
        throw SyscallError()<<boost::errinfo_errno(errno)
                            <<boost::errinfo_api_function("fcntl");
    }
    m_event_type.store(0);
}

Notifier::~Notifier() {
    if( -1 == close( m_request_fd ) )
    {
        perror("close");
    }
    if( -1 == close( m_wait_fd ) )
    {
        perror("close");
    }
}

int32_t
Notifier::get_wait_fd() {
    return m_wait_fd;
}

void
Notifier::notify(int32_t in_event ) {
    m_event_type.store(in_event);
    if( -1 == ::write(m_request_fd, "1", 1) )
    {
        perror("write");
    }
}

void
Notifier::acknowledge() {
    m_event_type.store(0);
    char dummy;
    if( -1 == ::read(m_wait_fd, &dummy, 1) )
    {
        perror("read");
    }
}

int32_t
Notifier::get_event_type() const {
    return m_event_type.load();
}

} //namespace Hpcl
