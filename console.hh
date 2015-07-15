#pragma once

void isa_serial_console_early_init();
void isa_serial_console_putchar(const char ch);
char isa_serial_console_readch();
void debug_early(const char *msg);
