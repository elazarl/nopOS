/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#include <cassert>
#include "clock.hh"

clock* clock::_c;

clock::~clock()
{
}

void clock::register_clock(clock* c)
{
    assert(!_c);
    _c = c;
}

clock* clock::get()
{
    return _c;
}
