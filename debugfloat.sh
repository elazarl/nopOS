#!/bin/bash
gcc -std=gnu99 -MD -MT libc/internal/floatscan.o -MP -g -Wall -Wno-pointer-arith -Werror -Wformat=0 -Wno-format-security -D __BSD_VISIBLE=1 -U _FORTIFY_SOURCE -fno-stack-protector -Iarch/x64 -I. -Iinclude -Iarch/common -isystem include/glibc-compat -isystem external/x64/acpica/source/include -isystem external/x64/misc.bin/usr/include -isystem include/api -isystem include/api/x64 -isystem build/release.x64/gen/include -isystem bsd/sys -isystem bsd/ -isystem bsd/x64 -D_KERNEL -fno-omit-frame-pointer -DHAVE_ATTR_COLD_LABEL -include compiler/include/intrinsics.hh -isystem /include -isystem /include/linux -msse2 -O2 -DNDEBUG -DACPI_MACHINE_WIDTH=64 -DACPI_USE_LOCAL_CACHE -DCONF_preempt=1 -DCONF_debug_memory=0 -DCONF_logger_debug=0 -D__OSV__ -D__XEN_INTERFACE_VERSION__="0x00030207" -DARCH_STRING=x86_64 -D__OSV_CORE__ -DOSV_KERNEL_BASE=0x200000 -I libc/stdio -I libc/internal -I libc/arch/x64 -Wno-missing-braces -Wno-parentheses -Wno-unused-but-set-variable -c -o libc/internal/floatscan.o libc/internal/floatscan.c 