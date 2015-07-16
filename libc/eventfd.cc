/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <sys/eventfd.h>
#include <libc/libc.hh>


int eventfd(unsigned int initval, int flags)
{
        return 0;
}
weak_alias(eventfd, eventfd2);

int eventfd_read(int fd, eventfd_t *value)
{
    return 0;
}

int eventfd_write(int fd, eventfd_t value)
{
    return 0;
}
