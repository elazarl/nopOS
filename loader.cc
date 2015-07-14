/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <cctype>
#include "types.h"
#include "console.hh"

extern "C" {
    void premain();
}

/*void arch_init_early_console()
{
    console::isa_serial_console::early_init();
}*/


void premain()
{
    isa_serial_console_early_init();
    debug_early("miniOSV\n");
    debug_early("woo hoo!\n");
    //arch_init_early_console();
#define OSV_VERSION "0"
    /* besides reporting the OSV version, this string has the function
       to check if the early console really works early enough,
       without depending on prior initialization. */
    //debug_early("OSv " OSV_VERSION "\n");


    /*auto inittab = elf::get_init(elf_header);
    setup_tls(inittab);
    boot_time.event("TLS initialization");
    for (auto init = inittab.start; init < inittab.start + inittab.count; ++init) {
        (*init)();
    }
    boot_time.event(".init functions");*/
}

int main(int ac, char **av)
{
    return 0;
}

extern "C" { void smp_main(void) {} }


void *elf_header;
int __argc;
char** __argv;

struct multiboot_info_type {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    u32 syms[4];
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
} __attribute__((packed));
struct osv_multiboot_info_type {
    struct multiboot_info_type mb;
    u32 tsc_init, tsc_init_hi;
    u32 tsc_disk_done, tsc_disk_done_hi;
} __attribute__((packed));

struct e820ent {
    u32 ent_size;
    u64 addr;
    u64 size;
    u32 type;
} __attribute__((packed));

osv_multiboot_info_type* osv_multiboot_info;
