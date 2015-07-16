/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <libc/libc.hh>

#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>

int pipe2(int pipefd[2], int flags) {
	return 0;
}

int pipe(int pipefd[2])
{
    return pipe2(pipefd, 0);
}
