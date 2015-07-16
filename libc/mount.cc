#include "libc.hh"
#include <sys/mount.h>

int mount(const char *source, const char *target,
         const char *filesystemtype, unsigned long mountflags,
         const void *data)
{
        return 0;
}

int umount(const char *path)
{
        return 0;
}

int umount2(const char *path, int flags)
{
        return 0;
}

