/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <dlfcn.h>
#include <link.h>

static __thread char *dlerror_ptr;

static char *dlerror_set(char *val)
{
    char *old = dlerror_ptr;

    dlerror_ptr = val;

    return old;
}

void* dlopen(const char* filename, int flags)
{
	return 0;
}

int dlclose(void* handle)
{
	return 0;
}

void* dlsym(void* handle, const char* name)
{
	return 0;
}

extern "C"
int dl_iterate_phdr(int (*callback)(struct dl_phdr_info *info,
                                    size_t size, void *data),
                    void *data)
{
	return 0;
}

extern "C" int dladdr(void *addr, Dl_info *info)
{
    return 1;
}

extern "C" char *dlerror(void)
{
    return dlerror_set(nullptr);
}
