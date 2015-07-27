/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <cstring>
#include "memory.hh"
#include "arch-cpu.hh"

namespace sched {

extern "C"
void fpu_state_init_xsave(processor::fpu_state *s) {
    memset(s->xsavehdr, 0, sizeof(s->xsavehdr));
}

extern "C"
void fpu_state_init_fxsave(processor::fpu_state *s) {
}

extern "C"
void fpu_state_save_xsave(processor::fpu_state *s) {
    processor::xsave(s, -1);
}

extern "C"
void fpu_state_save_fxsave(processor::fpu_state *s) {
    processor::fxsave(s);
}

extern "C"
void fpu_state_restore_xsave(processor::fpu_state *s) {
    processor::xrstor(s, -1);
}

extern "C"
void fpu_state_restore_fxsave(processor::fpu_state *s) {
    processor::fxrstor(s);
}

extern "C"
void (*resolve_fpu_state_init())(processor::fpu_state *s) {
    if (processor::features().xsave) {
        return fpu_state_init_xsave;
    } else {
        return fpu_state_init_fxsave;
    }
}
extern "C"
void (*resolve_fpu_state_save())(processor::fpu_state *s) {
    if (processor::features().xsave) {
        return fpu_state_save_xsave;
    } else {
        return fpu_state_save_fxsave;
    }
}

extern "C"
void (*resolve_fpu_state_restore())(processor::fpu_state *s) {
    if (processor::features().xsave) {
        return fpu_state_restore_xsave;
    } else {
        return fpu_state_restore_fxsave;
    }
}

void fpu_state_init(processor::fpu_state *s)
    __attribute__((ifunc("resolve_fpu_state_init")));
void fpu_state_save(processor::fpu_state *s)
    __attribute__((ifunc("resolve_fpu_state_save")));
void fpu_state_restore(processor::fpu_state *s)
    __attribute__((ifunc("resolve_fpu_state_restore")));

}
