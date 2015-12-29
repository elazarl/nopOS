#pragma once
#include <cstdarg>

namespace logger {
enum class level {
    DEBUG,
    INFO
};

struct module {
    int id;
    explicit module(int _id) : id(_id){}
    module() : id(-1) {} /* illegal module, just to make the compiler happy */
};

void set(module m, level lvl);
module add_module(const char *name, level lvl);

void debug(module m, const char *fmt, ...);
void vdebug(module m, const char *fmt, va_list va);
void info(module m, const char *fmt, ...);
void vinfo(module m, const char *fmt, va_list va);
void fatal(module m, const char *fmt, ...);
void init();

extern module boot;
extern module exception;

}
