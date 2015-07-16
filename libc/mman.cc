/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <sys/mman.h>
#include <memory>

unsigned libc_flags_to_mmap(int flags)
{
	return 0;
}

unsigned libc_prot_to_perm(int prot)
{
    return 0;
}

unsigned libc_madvise_to_advise(int advice)
{
    return 0;
}

int mprotect(void *addr, size_t len, int prot)
{
    return 0;
}

int mmap_validate(void *addr, size_t length, int flags, off_t offset)
{
    return 0;
}

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset)
{
    return 0;
}

extern "C" void *mmap64(void *addr, size_t length, int prot, int flags,
                      int fd, off64_t offset)
    __attribute__((alias("mmap")));


int munmap_validate(void *addr, size_t length)
{
    return 0;
}

int munmap(void *addr, size_t length)
{
    return 0;
}

int msync(void *addr, size_t length, int flags)
{
    return 0;
}

int mincore(void *addr, size_t length, unsigned char *vec)
{
	return 0;
}

int madvise(void *addr, size_t length, int advice)
{
    return 0;
}
