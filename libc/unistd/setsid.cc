#include <unistd.h>
#include "stubbing.hh"
#include "../libc.hh"

int setpgid(void)
{
    WARN_STUBBED();
    return libc_error(EPERM);
}
