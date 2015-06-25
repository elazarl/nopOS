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
//#include <osv/elf.hh>
//#include "arch-tls.hh"
//#include <osv/debug.hh>

//#include "smp.hh"

#ifndef AARCH64_PORT_STUB
//#include "drivers/acpi.hh"
#endif /* !AARCH64_PORT_STUB */

#include "debug.h"
#include "arch-setup.hh"

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
    debug_early("OSv " OSV_VERSION "\n");

    arch_init_premain();

    /*auto inittab = elf::get_init(elf_header);
    setup_tls(inittab);
    boot_time.event("TLS initialization");
    for (auto init = inittab.start; init < inittab.start + inittab.count; ++init) {
        (*init)();
    }
    boot_time.event(".init functions");*/
}

void *elf_header;
int __argc;
char** __argv;
