/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <semaphore.h>
#include <memory>
#include "libc.hh"

// FIXME: smp safety

int sem_init(sem_t* s, int pshared, unsigned val)
{
    return 0;
}

int sem_destroy(sem_t *s)
{
    return 0;
}

int sem_post(sem_t* s)
{
    return 0;
}

int sem_wait(sem_t* s)
{
    return 0;
}

int sem_timedwait(sem_t* s, const struct timespec *abs_timeout)
{
	return 0;
}

int sem_trywait(sem_t* s)
{
    return 0;
}
