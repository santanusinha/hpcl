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

#include "child_process.h"
#include "local_communicator.h"
#include "print.h"

namespace Hpcl {

ChildProcess::ChildProcess()
    :m_pid(),
    m_communicator(),
    m_stdin(),
    m_stdout(),
    m_stderr() {
}

ChildProcess::~ChildProcess() {
}

bool
ChildProcess::operator < (const ChildProcess &in_rhs ) const {
    return m_pid < in_rhs.get_pid();
}
        
pid_t
ChildProcess::get_pid() const {
    return m_pid;
}

LocalCommunicatorPtr
ChildProcess::get_communicator() const {
    return m_communicator;
}

int32_t
ChildProcess::get_stdin() const {
    return m_stdin;
}

int32_t
ChildProcess::get_stdout() const {
    return m_stdout;
}

int32_t
ChildProcess::get_stderr() const {
    return m_stderr;
}

void
ChildProcess::set_pid( pid_t in_pid ) {
    m_pid = in_pid;
}

void
ChildProcess::set_communicator(
        const LocalCommunicatorPtr &in_communicator ) {
    m_communicator = in_communicator;
}

void
ChildProcess::set_stdin( int32_t in_fd ) {
    m_stdin = in_fd;
}

void
ChildProcess::set_stdout( int32_t in_fd ) {
    m_stdout = in_fd;
}

void
ChildProcess::set_stderr( int32_t in_fd ) {
    m_stderr = in_fd;
}

} //nsmaespace Hpcl
