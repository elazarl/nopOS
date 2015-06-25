/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

/*#include "fs/fs.hh"
#include <bsd/net.hh>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>*/
#include <cctype>
#include "types.h"
//#include <osv/elf.hh>
//#include "arch-tls.hh"
//#include <osv/debug.hh>

//#include "smp.hh"

#ifndef AARCH64_PORT_STUB
//#include "drivers/acpi.hh"
#endif /* !AARCH64_PORT_STUB */


//#include <osv/sched.hh>
//#include <osv/barrier.hh>
//#include "arch.hh"
//#include "arch-setup.hh"
//#include "osv/trace.hh"
//#include <osv/power.hh>
//#include <osv/rcu.hh>
//#include <osv/mempool.hh>
//#include <bsd/porting/networking.hh>
//#include <bsd/porting/shrinker.h>
//#include <osv/dhcp.hh>
//#include <osv/version.h>
//#include <osv/run.hh>
//#include <osv/shutdown.hh>
//#include <osv/commands.hh>
//#include <osv/boot.hh>
//#include <osv/sampler.hh>

//#include "drivers/zfs.hh"
//#include "drivers/random.hh"
//#include "drivers/console.hh"
//#include "drivers/null.hh"

//using namespace osv;

asm(".pushsection \".debug_gdb_scripts\", \"MS\",@progbits,1 \n"
    ".byte 1 \n"
    ".asciz \"scripts/loader.py\" \n"
    ".popsection \n");


extern "C" {
    void premain();
}

void premain()
{
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
