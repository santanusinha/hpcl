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

#ifndef HPCL_ERROR_ID_H
#define HPCL_ERROR_ID_H

namespace Hpcl {

enum ErrorID {
    SEMANTIC_ERR_INVALID_PARAMETER,
    SEMANTIC_ERR_INIT_NOT_DONE,
    SEMANTIC_ERR_ARRAY_INDEX_OUT_OF_BOUNDS,
    SEMANTIC_ERR_HOST_NOT_FOUND,
    ERROR_ID_COUNT
};

typedef boost::error_info<struct tag_error_id, ErrorID> errinfo_errorid;

std::string
get_error_message( ErrorID in_error_id );

} //namespace Hpcl

#endif //HPCL_ERROR_ID_H
