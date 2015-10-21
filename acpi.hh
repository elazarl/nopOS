/*
 * Copyright (C) 2013 Nodalink, SARL.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */
#ifndef _OSV_DRIVER_ACPI_HH_
#define _OSV_DRIVER_ACPI_HH_

namespace acpi {

void early_init();
void init();
void poweroff(void);
void parse_madt();

}

#endif //!_OSV_DRIVER_ACPI_HH_
