#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    void isa_serial_console_putchar(const char ch);
    char isa_serial_console_readch();
#ifdef __cplusplus
}
void isa_serial_console_early_init();
void debug_early(const char *msg);
#endif
