#ifndef HPCL_EXECSERVER_H
#define HPCL_EXECSERVER_H

#include <cstdint>
#include <exception>
#include <queue>
#include <memory>
#include <set>

#include "child_process.h"
#include "local_communicator.h"
#include "server_socket.h"

namespace Hpcl {

class ExecServer : public std::enable_shared_from_this<ExecServer> {
    public:
        typedef std::shared_ptr<ExecServer> Pointer;
        typedef std::weak_ptr<ExecServer> WeakPointer;

        typedef boost::signals2::signal<
                    void(const ChildProcessPtr &)> SignalChildStarted;
        typedef boost::signals2::signal<
                    void(pid_t,int32_t)> SignalChildCompleted;

        ExecServer();
        virtual ~ExecServer();

        ExecServer(const ExecServer &) = delete;
        ExecServer &
        operator = (const ExecServer &) = delete;

        void
        start(const SocketFactoryPtr &in_factory, int32_t in_port);

        void
        stop();

        SignalChildStarted &
        signal_child_started();

        SignalChildCompleted &
        signal_child_completed();

    protected:
        virtual void
        on_child_start_request( const std::string &in_cmd );

        virtual void
        on_child_started( const ChildProcessPtr &in_child );

        virtual void
        on_child_completed( pid_t in_child_pid, int32_t in_status );

    private:
        typedef std::shared_ptr<std::thread> ThreadPtr;

        void
        on_command_received( const SocketPtr &in_socket,
                                    const MemInfo &in_cmd);

        void
        on_client_connected( const SocketPtr &in_client );

        void
        on_client_remote_disconnect( const SocketPtr &in_client );

        void
        on_handler_process_spawn( const ChildProcessPtr &in_child );

        void
        handle_process_creation( const std::string &in_command,
                                const SocketPtr &in_socket );

        //void
        //join_thread();

        ServerSocketPtr m_server;

        //std::mutex m_exit_mutex;
        //std::condition_variable m_exit_cond;
        //bool m_stop;

        //ThreadPtr m_exit_checker;

        SignalChildStarted m_signal_child_started;
        SignalChildCompleted m_signal_child_completed;
};

typedef ExecServer::Pointer ExecServerPtr;
typedef ExecServer::WeakPointer ExecServerWeakPtr;

} //namespace Hpcl
#endif //HPCL_EXECSERVER_H
