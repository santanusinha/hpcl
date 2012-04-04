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

#ifndef HPCL_REMOTE_PROCESS_H
#define HPCL_REMOTE_PROCESS_H

#include <cstdint>
#include <memory>
#include <boost/signals2.hpp>
#include <sys/types.h>
#include <thread>

#include "communication_pointer_types.h"
#include "meminfo.h"
#include "remote_exec_pointer_types.h"

namespace Hpcl {

class RemoteProcess : public std::enable_shared_from_this<RemoteProcess> {
    friend class RemoteExecComponentFactory;

    public:
        typedef std::shared_ptr<RemoteProcess> Pointer;
        typedef std::weak_ptr<RemoteProcess> WeakPointer;

        typedef boost::signals2::signal<
                    void(pid_t, const Pointer &)> SignalProcessStarted;
        typedef boost::signals2::signal<
                            void(pid_t, int32_t)> SignalProcessCompleted;

        virtual
        ~RemoteProcess();

        RemoteProcess( const RemoteProcess &) = delete;
        RemoteProcess &
        operator = ( const RemoteProcess &) = delete;

        SignalProcessStarted &
        signal_process_started();

        SignalProcessCompleted &
        signal_process_completed();

        pid_t
        get_pid() const;

        void
        connect(const std::string &in_server, int32_t in_port);

        void
        disconnect();

        void
        execute( const std::string &in_command );

    protected:
        explicit
        RemoteProcess( const SocketFactoryPtr &in_factory );

        virtual void
        on_process_started( pid_t in_pid, const Pointer &in_process );

        virtual void
        on_process_completed( pid_t in_pid, int32_t in_status );

    private:
        void
        handle_server_response( const SocketPtr &in_socket,
                                const MemInfo &in_data ); 

        void
        handle_remote_disconnect( const SocketPtr &in_socket );

        pid_t m_pid;
        SocketFactoryPtr m_factory;
        SocketPtr m_socket;
        SignalProcessStarted m_signal_process_started;
        SignalProcessCompleted m_signal_process_completed;
        std::mutex m_proc_complete_mutex;
        std::condition_variable m_proc_end_event;
        bool m_process_is_active;
};

} //namespace Hpcl

#endif //HPCL_REMOTE_PROCESS_H
