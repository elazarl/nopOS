#include "stdio_impl.h"
#include <termios.h>
#include <sys/ioctl.h>

extern int vfs_initialized;

extern "C"
size_t __stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
	return 0;
}
