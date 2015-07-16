/*
 * Copyright (C) 2013-2014 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <libc/libc.hh>
#include "types.h"
#include "stubbing.hh"

#include <sys/timerfd.h>

#include <atomic>


// After this long introduction, without further ado, let's implement Linux's
// three <sys/timerfd.h> functions:

int timerfd_create(int clockid, int flags) {
        return 0;
}

static constexpr s64 second = 1000000000;

int timerfd_settime(int fd, int flags, const itimerspec *newval,
        itimerspec *oldval)
{
	return 0;
}

int timerfd_gettime(int fd, itimerspec *val)
{
	return 0;
}
