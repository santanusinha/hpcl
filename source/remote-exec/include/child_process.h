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

#ifndef HPCL_CHILD_PROCESS_H
#define HPCL_CHILD_PROCESS_H

#include <sys/types.h>
#include "local_communicator.h"

namespace Hpcl {

class ChildProcess {
    public:
        typedef std::shared_ptr<ChildProcess> Pointer;
        typedef std::weak_ptr<ChildProcess> WeakPointer;

        ChildProcess();
        ~ChildProcess();

        ChildProcess( const ChildProcess &)= default;
        ChildProcess &
        operator = ( const ChildProcess &)= default;

        bool
        operator < (const ChildProcess &in_rhs ) const;
        
        pid_t
        get_pid() const;

        LocalCommunicatorPtr
        get_communicator() const;

        int32_t
        get_stdin() const;

        int32_t
        get_stdout() const;

        int32_t
        get_stderr() const;

        void
        set_pid( pid_t in_pid );

        void
        set_communicator(
                const LocalCommunicatorPtr &in_communicator );

        void
        set_stdin ( int32_t in_fd );

        void
        set_stdout( int32_t in_fd );

        void
        set_stderr( int32_t in_fd );

    private:

        pid_t m_pid;
        LocalCommunicatorPtr m_communicator;
        int32_t m_stdin;
        int32_t m_stdout;
        int32_t m_stderr;

        friend class ExecServer;
};

typedef ChildProcess::Pointer ChildProcessPtr;

} //namespace Hpcl

#endif //HPCL_CHILD_PROCESS_H


