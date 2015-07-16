/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <api/utime.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "stubbing.hh"
#include "libc.hh"
#include "types.h"

u64 convert(const timespec& ts)
{
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

extern "C"
int gettimeofday(struct timeval* tv, struct timezone* tz)
{
    if (!tv) {
        return 0;
    }
    tv->tv_sec = 0;
    tv->tv_usec = 0;
    return 0;
}

int nanosleep(const struct timespec* req, struct timespec* rem)
{
    return 0;
}

int usleep(useconds_t usec)
{
    return 0;
}

int clock_gettime(clockid_t clk_id, struct timespec* ts)
{
	return 0;
}

extern "C"
int __clock_gettime(clockid_t clk_id, struct timespec* ts) __attribute__((alias("clock_gettime")));

int clock_getres(clockid_t clk_id, struct timespec* ts)
{
    return 0;
}

int clock_getcpuclockid(pid_t pid, clockid_t* clock_id)
{
    return CLOCK_PROCESS_CPUTIME_ID;
}

clock_t clock(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}
