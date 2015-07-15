/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include "libc.hh"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <boost/algorithm/string/split.hpp>
#include <type_traits>
#include <limits>
#include <sys/resource.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "stubbing.hh"

// FIXME: If we ever support multiple different executables we will have to maybe put those
// on a shared library
char *program_invocation_name;
char *program_invocation_short_name;

int libc_error(int err)
{
    errno = err;
    return -1;
}

#undef errno

int __thread errno;

int* __errno_location()
{
    return &errno;
}

int getrlimit(int resource, struct rlimit *rlim)
{
	return 0;
}

int setrlimit(int resource, const struct rlimit *rlim)
{
    // osv - no limits
    return 0;
}
//LFS64(getrlimit);
//LFS64(setrlimit);

uid_t geteuid()
{
    return 0;
}

int sched_yield()
{
    return 0;
}

extern "C"
int getloadavg(double loadavg[], int nelem)
{
    int i;

    for (i = 0; i < nelem; i++)
        loadavg[i] = 0.5;

    return 0;
}

extern "C" int sysinfo(struct sysinfo *info)
{
    return 0;
}

int tcgetattr(int fd, termios *p)
{
    return ioctl(fd, TCGETS, p);
}

int tcsetattr(int fd, int action, const termios *p)
{
    switch (action) {
    case TCSANOW:
        break;
    case TCSADRAIN:
        tcdrain(fd);
        break;
    case TCSAFLUSH:
        tcdrain(fd);
        tcflush(fd,TCIFLUSH);
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    return ioctl(fd, TCSETS, p);
}

int tcdrain(int fd)
{
    // The archaic TCSBRK is customary on Linux for draining output.
    // BSD would have used TIOCDRAIN.
    return ioctl(fd, TCSBRK, 1);
}

int tcflush(int fd, int what)
{
    // Linux uses TCFLSH. BSD would have used TIOCFLUSH (and different
    // argument).
    return ioctl(fd, TCFLSH, what);
}

speed_t cfgetospeed(const termios *p)
{
    speed_t speed{};
    return speed;
}

int cfsetospeed(struct termios *tio, speed_t speed)
{
    return 0;
}

speed_t cfgetispeed(const termios *p)
{
    speed_t speed{};
    return speed;
}

int cfsetispeed(struct termios *tio, speed_t speed)
{
    return 0;
}

int cfsetspeed(struct termios *tio, speed_t speed)
{
    cfsetispeed(tio, speed);
    return cfsetospeed(tio, speed);
}

int tcsendbreak(int fd, int dur)
{
	return ioctl(fd, TCSBRK, 0);
}

void cfmakeraw(struct termios *t)
{
	t->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	t->c_oflag &= ~OPOST;
	t->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	t->c_cflag &= ~(CSIZE|PARENB);
	t->c_cflag |= CS8;
	t->c_cc[VMIN] = 1;
	t->c_cc[VTIME] = 0;
}

int system(const char *command)
{
    WARN_STUBBED();
    return -1;
}

extern "C" {
    const char *gnu_get_libc_version(void)
    {
        return "miniOSv";
    }

    const char *gnu_get_libc_release(void)
    {
        return "OSv";
    }
}
