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

#ifndef HPCL_NOTIFIER_H
#define HPCL_NOTIFIER_H

#include <atomic>
#include <cstdint>

namespace Hpcl {

class Notifier {
    public:
        Notifier();
        ~Notifier();

        void
        init();

        int32_t
        get_wait_fd();

        void
        notify(int32_t in_event);

        void
        acknowledge();

        int32_t
        get_event_type() const;

    public:
        int32_t m_request_fd;
        int32_t m_wait_fd;
        std::atomic_int m_event_type;
};

} //namespace Hpcl

#endif //HPCL_NOTIFIER_H
