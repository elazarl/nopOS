
// adapted from musl's version, just writes to stdio

#include <syslog.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <libc.h>

void openlog(const char *ident, int opt, int facility)
{
}

void closelog(void)
{
}

void __syslog_chk(int priority, int flag, const char *message, ...)
{
}

