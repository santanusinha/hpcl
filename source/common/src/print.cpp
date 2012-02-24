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

#include <cstdio>
#include <cstdarg>
#include "print.h"

namespace {
void
print( const char *extra, const char *in_fmt, va_list &in_args ) {
    printf("%s",extra);
    vprintf(in_fmt, in_args);
    return;
}
}

namespace Hpcl {

void
hpcl_info( const char *in_fmt, ... ) {
    va_list args;
    va_start( args, in_fmt );
    print("HPCL::INFO::", in_fmt, args );
    va_end(args);
    return;
}

void
hpcl_debug( const char *in_fmt, ... ) {
    va_list args;
    va_start( args, in_fmt );
    print("HPCL::DEBUG::", in_fmt, args );
    va_end(args);
    return;
}

void
hpcl_print( const char *in_fmt, ... ) {
    va_list args;
    va_start( args, in_fmt );
    print("HPCL::ERROR::", in_fmt, args );
    va_end(args);
    return;
}

} //namespace Hpcl
