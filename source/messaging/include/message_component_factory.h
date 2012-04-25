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

#ifndef HPCL_MESSAGE_COMPONENT_FACTORY_H
#define HPCL_MESSAGE_COMPONENT_FACTORY_H

#include <memory>

#include "communication_pointer_types.h"
#include "messaging_pointer_types.h"

namespace Hpcl {

class MessageComponentFactory
        : public std::enable_shared_from_this<MessageComponentFactory> {
    friend class MessageServer;

    public:
        typedef std::shared_ptr<MessageComponentFactory> Pointer;
        typedef std::weak_ptr<MessageComponentFactory> WeakPointer;

        static Pointer
        create_factory();

        MessageComponentFactory(
                const MessageComponentFactory &) = delete;
        MessageComponentFactory &
        operator = ( const MessageComponentFactory &) = delete;

        virtual
        ~MessageComponentFactory();

        MessageServerPtr
        create_message_server();

        MessageClientPtr
        create_message_client();

    protected:
        explicit MessageComponentFactory(
                            const SocketFactoryPtr &in_factory );

        virtual MessageServerPtr
        create_new_message_server();

        virtual MessageClientPtr
        create_new_client_side_message_client();

        virtual MessageClientPtr
        create_new_server_side_message_client( const SocketPtr &in_socket );

    private:
        SocketFactoryPtr m_factory;
};

typedef MessageComponentFactory::Pointer MessageComponentFactoryPtr;
typedef MessageComponentFactory::WeakPointer MessageComponentFactoryWeakPtr;

} //namespace Hpcl
#endif //HPCL_MESSAGE_COMPONENT_FACTORY_H
