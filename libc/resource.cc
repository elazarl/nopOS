/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>


#include "libc.hh"

int getrusage(int who, struct rusage *usage)
{
    return 0;
}
LFS64(getrusage);
