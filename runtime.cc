/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <atomic>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <exception>
#include <cxxabi.h>
#include <sys/mman.h>
#include <unistd.h>
#include <link.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/uio.h>
#include <wchar.h>
#include <locale.h>
#include <libintl.h>
#include <ctype.h>
#include <wctype.h>
#include <langinfo.h>
#include <stdarg.h>
#include <xlocale.h>
#include <cassert>
#include <sys/sysinfo.h>
#include <boost/format.hpp>
#include <pwd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <map>
#include <boost/range/adaptor/reversed.hpp>
#include <grp.h>
#include <unordered_map>
#include "stubbing.hh"

#undef weak_alias
#define __weak_alias(old, new) \
	__typeof(old) new __attribute__((weak, alias(#old)))
#ifdef __cplusplus
#define weak_alias(old, new) extern "C" __weak_alias(old, new)
#else
#define weak_alias(old, new) extern __weak_alias(old, new)
#endif

#undef LFS64_2
#define LFS64_2(x, y) weak_alias(x, y)

#undef LFS64
#define LFS64(x) LFS64_2(x, x##64)

#define __LC_LAST 13

#define __ALIAS(old, new) \
        __typeof(old) new __attribute__((alias(#old)))
#define ALIAS(old, new) extern "C" __ALIAS(old, new)

void *__dso_handle;

static std::atomic<bool> aborting { false };

void abort()
{
	__builtin_abort();
}

void abort(const char *fmt, ...)
{
	__builtin_abort();
}

// __assert_fail() is used by the assert() macros
void __assert_fail(const char *expr, const char *file, unsigned int line, const char *func)
{
    abort("Assertion failed: %s (%s: %s: %d)\n", expr, file, func, line);
    __builtin_abort();
}


// __cxa_atexit and __cxa_finalize:
// Gcc implements static constructors and destructors in shared-objects (DSO)
// in the following way: Static constructors are added to a list DT_INIT_ARRAY
// in the object, and we run these functions after loading the object. Gcc's
// code for each constructor calls a function __cxxabiv1::__cxa_atexit (which
// we need to implement here) to register a destructor, linked to this DSO.
// Gcc also adds a single finalization function to DT_FINI_ARRAY (which we
// call when unloading the DSO), which calls __cxxabiv1::__cxa_finalize
// (which we need to implement here) - this function is supposed to call all
// the destructors previously registered for the given DSO.
//
// This implementation is greatly simplified by the assumption that the kernel
// never exits, so our code doesn't need to work during early initialization,
// nor does __cxa_finalize(0) need to work.
typedef void (*destructor_t)(void *);
static std::map<void *, std::vector<std::pair<destructor_t,void*>>> destructors;
namespace __cxxabiv1 {
int __cxa_atexit(destructor_t destructor, void *arg, void *dso)
{
    // As explained above, don't remember the kernel's own destructors.
    if (dso == &__dso_handle)
        return 0;
    destructors[dso].push_back(std::make_pair(destructor, arg));
    return 0;
}

int __cxa_finalize(void *dso)
{
    if (!dso || dso == &__dso_handle) {
        //debug("__cxa_finalize() running kernel's destructors not supported\n");
        return 0;
    }
    for (auto d : boost::adaptors::reverse(destructors[dso])) {
        d.first(d.second);
    }
    destructors.erase(dso);
    return 0;
}
}

int getpagesize()
{
    return 4096;
}

int vfork()
{
    WARN_STUBBED();
    return -1;
}

int fork()
{
    WARN_STUBBED();
    return -1;
}

pid_t setsid(void)
{
    WARN_STUBBED();
    return -1;
}

NO_SYS(int execvp(const char *, char *const []));

int mlockall(int flags)
{
    return 0;
}

int munlockall(void)
{
    return 0;
}

int posix_fadvise(int fd, off_t offset, off_t len, int advice)
{
	return 0;
}
LFS64(posix_fadvise);

int posix_fallocate(int fd, off_t offset, off_t len)
{
    return ENOSYS;
}
LFS64(posix_fallocate);

int getpid()
{
    return 0;
}

//    WCTDEF(alnum), WCTDEF(alpha), WCTDEF(blank), WCTDEF(cntrl),
//    WCTDEF(digit), WCTDEF(graph), WCTDEF(lower), WCTDEF(print),
//    WCTDEF(punct), WCTDEF(space), WCTDEF(upper), WCTDEF(xdigit),

static unsigned short c_locale_array[384] = {
};

static struct __locale_struct c_locale = {
    { }, // __locales_data
    c_locale_array + 128, // __ctype_b
};

locale_t __c_locale_ptr = &c_locale;

void* __stack_chk_guard = reinterpret_cast<void*>(0x12345678abcdefull);
extern "C" void __stack_chk_fail(void) {
    abort("__stack_chk_fail(): Stack overflow detected. Aborting.\n");
}

namespace {
    bool all_categories(int category_mask)
    {
	return (category_mask | (1 << LC_ALL)) == (1 << __LC_LAST) - 1;
    }
}

struct __locale_data {
    const void *values[0];
};

#define _NL_ITEM(category, index)   (((category) << 16) | (index))
#define _NL_ITEM_CATEGORY(item)     ((int) (item) >> 16)
#define _NL_ITEM_INDEX(item)        ((int) (item) & 0xffff)

#define _NL_CTYPE_CLASS  0
#define _NL_CTYPE_TOUPPER 1
#define _NL_CTYPE_TOLOWER 3

extern "C"
__locale_t __newlocale(int category_mask, const char *locale, locale_t base)
    __THROW
{
	return nullptr;
}

ALIAS(__newlocale, newlocale);

long sysconf(int name)
{
	return 0;
}

long pathconf(const char *, int)
{
    WARN_STUBBED();
    return -1;
}

size_t confstr(int name, char* buf, size_t len)
{
	return 0;
}

FILE *popen(const char *command, const char *type)
{
    WARN_STUBBED();
    return NULL;
}

int pclose(FILE *stream)
{
	return 0;
}

void exit(int status)
{
	__builtin_abort();
}

// "The function _exit() is like exit(3), but does not call any functions
// registered with atexit(3) or on_exit(3)."
//
// Since we do nothing for those anyway, they are equal.
ALIAS(exit, _exit);
ALIAS(exit, _Exit);

int atexit(void (*func)())
{
    // nothing to do
    return 0;
}

int get_nprocs()
{
    return 1;
}

clock_t times(struct tms *buffer)
{
	clock_t c{};
	return c;
}

// Our priority formula is osv_prio = e^(prio * k), where k is a constant.
// We want osv_prio(20) = 86, and osv_prio(-20) = 1/86, as this gives the
// best agreement with Linux's current interpretation of the nice values
// (see tests/misc-setpriority.cc).
//
// So e^(20 * prio_k) = 86
//    20 * prio_k = ln(86)
//    prio_k = ln(86) / 20
//
// When we are given OSv prio, obviously, the inverse formula applies:
//
//    e^(prio * prio_k) = osv_prio
//    prio * prio_k = ln(osv_prio)
//    prio = ln(osv_prio) / prio_k
//
static constexpr float prio_k = log(86) / 20;

int getpriority(int which, int id)
{
	return 0;
}

int setpriority(int which, int id, int prio)
{
	return 0;
}

int initgroups(const char *user, gid_t group)
{
    WARN_STUBBED();
    return -1;
}

int prctl(int option, ...)
{
	return 0;
}

int daemon(int nochdir, int noclose)
{
    WARN_STUBBED();
    return -1;
}

extern "C"
int sysctl(int *, int, void *, size_t *, void *, size_t)
{
    WARN_STUBBED();
    errno = ENOTDIR;
    return -1;
}

extern "C" {

void free(void *){}
void *realoc(void *ptr, size_t sz){ return NULL; }
void *malloc(size_t sz) { return NULL; }

}
