/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <sys/mman.h>
#include <sys/shm.h>
#include <unordered_map>
#include <libc/libc.hh>

static mutex shm_lock;

void *shmat(int shmid, const void *shmaddr, int shmflg)
{
	return 0;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
    return 0;
}

int shmdt(const void *shmaddr)
{
    return 0;
}

/*
 * shm is implemented on top of shared memory file.
 * shmget returns shm file descriptor as shmid.
 */
int shmget(key_t key, size_t size, int shmflg)
{
    return 0;
}
