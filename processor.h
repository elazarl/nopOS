#pragma once
#include "types.h"

static inline u8 inb(u16 port)
{
    u8 r;
    asm volatile ("inb %1, %0":"=a" (r):"dN" (port));
    return r;
}

static inline u16 inw(u16 port)
{
    u16 r;
    asm volatile ("inw %1, %0":"=a" (r):"dN" (port));
    return r;
}

static inline u32 inl(u16 port)
{
    u32 r;
    asm volatile ("inl %1, %0":"=a" (r):"dN" (port));
    return r;
}

/* cnt is not bytes, 4byte counter */
static inline void insl(void *addr, int cnt, u16 port)
{
    asm volatile ("rep insl"
        :"+D" (addr), "+c" (cnt)
        : "d" (port)
        : "memory", "cc");
}

static inline void outb(u8 val, u16 port)
{
    asm volatile ("outb %0, %1"::"a" (val), "dN" (port));
}

static inline void outw(u16 val, u16 port)
{
    asm volatile ("outw %0, %1"::"a" (val), "dN" (port));
}

static inline void outl(u32 val, u16 port)
{
    asm volatile ("outl %0, %1"::"a" (val), "dN" (port));
}

/* cnt is not bytes, 4byte counter */
static inline void outsl(void *addr, int cnt, u16 port)
{
    asm volatile ("rep outsl"
        :"+S" (addr), "+c" (cnt)
        : "d" (port)
        : "cc");
}

static inline void sti()
{
    asm volatile ("sti" : : : "memory");
}

static inline void cli()
{
    asm volatile ("cli" : : : "memory");
}

