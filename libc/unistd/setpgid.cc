#include <unistd.h>
#include "stubbing.hh"
#include "../libc.hh"

int setpgid(pid_t pid, pid_t pgid)
{
    WARN_STUBBED();
    return libc_error(EINVAL);
}
