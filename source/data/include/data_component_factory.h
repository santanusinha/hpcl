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

#ifndef HPCL_DATA_COMPONENT_FACTORY_H
#define HPCL_DATA_COMPONENT_FACTORY_H

#include "communication_pointer_types.h"
#include "data_pointer_types.h"

namespace Hpcl {

class DataComponentFactory {
    public:
        typedef DataComponentFactoryPtr Pointer;
        typedef DataComponentFactoryWeakPtr WeakPointer;

        static Pointer
        create_factory();

        DataComponentFactory( const DataComponentFactory &) = delete;
        DataComponentFactory &
        operator =( const DataComponentFactory &) = delete;

        virtual ~DataComponentFactory();

        NameServiceClientPtr
        create_name_service_client();

    protected:
        explicit DataComponentFactory(
                    const SocketFactoryPtr &in_socket_factory );

    private:
        virtual NameServiceClientPtr
        create_new_name_service_client();

        SocketFactoryPtr m_socket_factory;
};

} //namespace Hpcl

#endif //HPCL_DATA_COMPONENT_FACTORY_H
