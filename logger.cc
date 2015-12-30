#include "logger.hh"
#include "console.hh"
#include "stack_container.hh"
#include "printf.h"

namespace memory {
logger::module memory;
}

namespace logger {

struct module_impl {
    const char *name;
    level lvl;
    module_impl(const char *_name, level _lvl) : name(_name), lvl(_lvl) {}
};
const int max_modules = 100;
StackVector<module_impl, max_modules> modules;

level lvl;
void set(module m, level lvl)
{
    modules[m.id].lvl = lvl;
}

static void raw_putc(void *v, char c)
{
    isa_serial_console_putchar(c);
}

module boot;
module acpi;
module exception;

void init()
{
    isa_serial_console_early_init();
    new (&modules) StackVector<module_impl, max_modules>();
    boot              = add_module("boot", level::INFO);
    acpi              = add_module("acpi", level::INFO);
    exception         = add_module("exception", level::INFO);
    memory::memory    = add_module("memory", level::INFO);
}

module add_module(const char *name, level lvl)
{
    modules->push_back(module_impl{name, lvl});
    return module(modules->size()-1);
}

void vdebug(module m, const char *fmt, va_list va)
{
    if (modules[m.id].lvl != level::DEBUG)
        return;
    tfp_format(nullptr, raw_putc, fmt, va);
}
void debug(module m, const char *fmt, ...)
{
    va_list va;
    va_start(va,fmt);
    vdebug(m, fmt, va);
    va_end(va);
}
void vinfo(module m, const char *fmt, va_list va)
{
    tfp_format(nullptr, raw_putc, fmt, va);
}
void info(module m, const char *fmt, ...)
{
    va_list va;
    va_start(va,fmt);
    tfp_format(nullptr, raw_putc, fmt, va);
    va_end(va);
}

}
