/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <pthread.h>
#include <errno.h>
#include <mutex>
#include <vector>
#include <algorithm>
#include <string.h>
#include <list>
#include <stdio.h>
#include "stubbing.hh"


int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
        void *(*start_routine) (void *), void *arg)
{
	return 0;
}

int pthread_join(pthread_t thread, void** retval)
{
	return 0;
}

int pthread_key_create(pthread_key_t* key, void (*dtor)(void*))
{
	return 0;
}

int pthread_atfork(void (*prepare)(void), void (*parent)(void),
                   void (*child)(void))
{
    return 0;
}

extern "C" int register_atfork(void (*prepare)(void), void (*parent)(void),
                                void (*child)(void), void *__dso_handle)
{
    return 0;
}

extern "C" {
    int __register_atfork(void (*prepare)(void), void (*parent)(void),
                          void (*child)(void), void *__dso_handle) __attribute__((alias("register_atfork")));
    int __pthread_key_create(pthread_key_t* key, void (*dtor)(void*))
        __attribute__((alias("pthread_key_create")));
}


int pthread_key_delete(pthread_key_t key)
{
    WARN_STUBBED();
    return EINVAL;
}

void* pthread_getspecific(pthread_key_t key)
{
    return NULL;
}

int pthread_setspecific(pthread_key_t key, const void* value)
{
    return 0;
}

pthread_t pthread_self()
{
    pthread_t t{};
    return t;
}

int pthread_getcpuclockid(pthread_t thread, clockid_t *clock_id)
{
    return 0;
}

// pthread_spinlock_t and spinlock_t aren't really the same type. But since
// spinlock_t is a boolean and pthread_spinlock_t is defined to be an integer,
// just casting it like this is fine. As long as we are never operating more
// than sizeof(int) at a time, we should be fine.
int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
    return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *lock)
{
    return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock)
{
    return 0; // We can't really do deadlock detection
}

int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    return 0;
}

int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    return 0;
}


int pthread_mutex_lock(pthread_mutex_t *m)
{
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *m)
{
    return 0;
}

int pthread_mutex_timedlock(pthread_mutex_t *m,
        const struct timespec *abs_timeout)
{
    WARN_STUBBED();
    return EINVAL;
}


int pthread_mutex_unlock(pthread_mutex_t *m)
{
    return 0;
}

extern "C" int pthread_yield()
{
    return 0;
}


int pthread_rwlock_init(pthread_rwlock_t *rw, const pthread_rwlockattr_t *attr)
{
    return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
	return 0;
}
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr)
{
	return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
    return 0;
}

int pthread_sigmask(int how, const sigset_t* set, sigset_t* oldset)
{
    return 0;
}


int pthread_cond_broadcast(pthread_cond_t *cond)
{
    return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    return 0;
}

int pthread_cond_wait(pthread_cond_t *__restrict cond,
       pthread_mutex_t *__restrict mutex)
{
    return 0;
}

int pthread_cond_timedwait(pthread_cond_t *__restrict cond,
                           pthread_mutex_t *__restrict mutex,
                           const struct timespec* __restrict ts)
{
	return 0;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr)
{
    return 0;
}

int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr)
{
    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *__restrict attr,
                                  size_t * stacksize)
{
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize)
{
    return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
    return 0;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
    return 0;
}

int pthread_attr_getstack(const pthread_attr_t * __restrict attr,
                                void **stackaddr, size_t *stacksize)
{
    return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr,
        void *stackaddr, size_t stacksize)
{
    return 0;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
    return 0;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    return 0;
}

int pthread_attr_getscope(pthread_attr_t *attr, int *scope)
{
    return 0;
}

int pthread_setcancelstate(int state, int *oldstate)
{
    WARN_STUBBED();
    return 0;
}

int pthread_setcanceltype(int state, int *oldstate)
{
    WARN_STUBBED();
    return 0;
}

void pthread_testcancel(void)
{
   WARN_STUBBED();
}

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    return 0;
}

// libstdc++ checks whether threads are compiled in using its
// __gthread_active_p(), which (when compiled on Linux) just verifies
// that pthread_cancel() is available. So we need it available, even
// we don't intend to actually use it.
int pthread_cancel(pthread_t thread)
{
    WARN_STUBBED();
    return ESRCH;
}

int pthread_detach(pthread_t thread)
{
    return 0;
}

int pthread_equal(pthread_t t1, pthread_t t2)
{
    return t1 == t2;
}

int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type)
{
    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared)
{
    return 0;
}

int pthread_condattr_init(pthread_condattr_t *attr)
{
    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *)
{
    return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clockid)
{
    return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *__restrict attr,
        clockid_t *__restrict clockid)
{
    return 0;
}

int pthread_condattr_setpshared(pthread_condattr_t *, int)
{
    WARN_STUBBED();
    return EINVAL;
}


int pthread_condattr_getpshared(const pthread_condattr_t *__restrict, int *__restrict)
{
    WARN_STUBBED();
    return EINVAL;
}

void pthread_exit(void *retval)
{
	__builtin_abort();
}

int sched_get_priority_max(int policy)
{
    WARN_STUBBED();
    return EINVAL;
}

int sched_get_priority_min(int policy)
{
    WARN_STUBBED();
    return EINVAL;
}

int pthread_setschedparam(pthread_t thread, int policy,
        const struct sched_param *param)
{
    WARN_STUBBED();
    return EINVAL;
}

int pthread_getschedparam(pthread_t thread, int *policy,
        struct sched_param *param)
{
    WARN_STUBBED();
    return EINVAL;
}

int pthread_kill(pthread_t thread, int sig)
{
    WARN_STUBBED();

    return EINVAL;
}

int raise(int sig)
{
    return pthread_kill(pthread_self(), sig);
}

int pthread_setname_np(pthread_t p, const char* name)
{
    return 0;
}

int pthread_attr_setaffinity_np(pthread_attr_t *attr, size_t cpusetsize,
        const cpu_set_t *cpuset)
{
    return 0;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize,
        const cpu_set_t *cpuset)
{
    return 0;
}

int sched_setaffinity(pid_t pid, size_t cpusetsize,
        cpu_set_t *cpuset)
{
    return 0;
}

int pthread_getaffinity_np(const pthread_t thread, size_t cpusetsize,
        cpu_set_t *cpuset)
{
    return 0;
}

int pthread_attr_getaffinity_np(const pthread_attr_t *attr, size_t cpusetsize,
        cpu_set_t *cpuset)
{
    return 0;
}

int sched_getaffinity(pid_t pid, size_t cpusetsize,
        cpu_set_t *cpuset)
{
    WARN_STUBBED();
    return pthread_getaffinity_np(pthread_self(), cpusetsize, cpuset);
}
