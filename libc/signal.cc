/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <setjmp.h>
#include "stubbing.hh"


int sigemptyset(sigset_t* sigset)
{
    return 0;
}

int sigfillset(sigset_t *sigset)
{
    return 0;
}

int sigaddset(sigset_t *sigset, int signum)
{
    return 0;
}

int sigdelset(sigset_t *sigset, int signum)
{
    return 0;
}

int sigismember(const sigset_t *sigset, int signum)
{
    return 0;
}

int sigprocmask(int how, const sigset_t* _set, sigset_t* _oldset)
{
    return 0;
}

int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact)
{
    return 0;
}

// using signal() is not recommended (use sigaction instead!), but some
// programs like to call to do simple things, like ignoring a certain signal.
static sighandler_t signal(int signum, sighandler_t handler, int sa_flags)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handler;
    act.sa_flags = sa_flags;
    struct sigaction old;
    if (sigaction(signum, &act, &old) < 0) {
        return SIG_ERR;
    }
    if (old.sa_flags & SA_SIGINFO) {
        // TODO: Is there anything sane to do here?
        return nullptr;
    } else {
        return old.sa_handler;
    }
}

sighandler_t signal(int signum, sighandler_t handler)
{
    return signal(signum, handler, SA_RESTART);
}

extern "C"
sighandler_t __sysv_signal(int signum, sighandler_t handler)
{
    return signal(signum, handler, SA_RESETHAND | SA_NODEFER);
}

// using sigignore() and friends is not recommended as it is obsolete System V
// APIs. Nevertheless, some programs use it.
int sigignore(int signum)
{
    return 0;
}

int sigwait(const sigset_t *set, int *sig)
{
    return 0;
}

// Partially-Linux-compatible support for kill(2).
// Note that this is different from our generate_signal() - the latter is only
// suitable for delivering SIGFPE and SIGSEGV to the same thread that called
// this function.
//
// Handling kill(2)/signal(2) exactly like Linux, where one of the existing
// threads runs the signal handler, is difficult in OSv because it requires
// tracking of when we're in kernel code (to delay the signal handling until
// it returns to "user" code), and also to interrupt sleeping kernel code and
// have it return sooner.
// Instead, we provide a simple "approximation" of the signal handling -
// on each kill(), a *new* thread is created to run the signal handler code.
//
// This approximation will work in programs that do not care about the signal
// being delivered to a specific thread, and that do not intend that the
// signal should interrupt a system call (e.g., sleep() or hung read()).
// FIXME: think if our handling of nested signals is ok (right now while
// handling a signal, we can get another one of the same signal and start
// another handler thread. We should probably block this signal while
// handling it.

int kill(pid_t pid, int sig)
{
    return 0;
}

int pause(void) {
    return -1;
}

void cancel_this_thread_alarm()
{
}

unsigned int alarm(unsigned int seconds)
{
    return 0;
}

extern "C" int setitimer(int which, const struct itimerval *new_value,
    struct itimerval *old_value)
{
	return 0;
}

extern "C" int getitimer(int which, struct itimerval *curr_value)
{
	return 0;
}

int sigaltstack(const stack_t *ss, stack_t *oss)
{
    WARN_STUBBED();
    return 0;
}

extern "C" int __sigsetjmp(sigjmp_buf env, int savemask)
{
    WARN_STUBBED();
    return 0;
}

extern "C" int signalfd(int fd, const sigset_t *mask, int flags)
{
    WARN_STUBBED();
    errno = ENOSYS;
    return -1;
}
