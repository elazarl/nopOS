#include "mutex.h"
extern "C" {
void access(){}
void bind(){}
void *calloc (size_t, size_t) { return 0; }
void close(){}
void closedir(){}
void dup3(){}
void fcntl(){}
void __fxstat64(){}
void getcwd(){}
void inet_ntoa(){}
void ioctl(){}
void lseek(){}
void lseek64(){}
void lstat(){}
void memcpy_backwards(){}
void mkdir(){}
void open(){}
void opendir(){}
void poll(){}
void pread64(){}
void read(){}
void readdir(){}
void readdir_r(){}
void readlink(){}
void readv(){}
void* realloc(void*, size_t){ return 0; }
void recvfrom(){}
void rmdir(){}
void sendto(){}
void socket(){}
void stat(){}
void __syscall(){}
void syscall(){}
void unlink(){}
void write(){}
void writev(){}
}
void ssse3_unaligned_copy(void*, void const*, unsigned long){}
namespace lockfree {
void mutex::lock(){}
void mutex::unlock(){}
}
namespace processor {
	void features(){}
}
