#include <stdio.h>
#include "console.hh"

extern "C" {

ssize_t write(int fd, const void *buf, size_t count) {
    size_t i;
    const char *b = reinterpret_cast<const char *>(buf);
    for (i=0; i<count; i++) {
        isa_serial_console_putchar(b[i]);
    }
    return count;
}

ssize_t read(int fd, void *buf, size_t count) {
    size_t i;
    char *b = reinterpret_cast<char *>(buf);
    for (i=0; i<count; i++) {
        b[i] = isa_serial_console_readch();
    }
    return count;
}

}
