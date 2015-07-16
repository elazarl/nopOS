#include <unistd.h>
#include "stubbing.hh"
#include "../libc.hh"

int execve(const char *path, char *const argv[], char *const envp[])
{
    WARN_STUBBED();
    return libc_error(ENOEXEC);
}
