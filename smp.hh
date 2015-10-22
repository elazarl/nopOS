#pragma once

#include "types.h"
#include "stack_container.hh"

namespace smp {
constexpr int max_ncpu = 24;
struct cpu {
	u32 apic_id;
	u32 acpi_id;
	int id;
	cpu(int _id) : id(_id) {}
};
void parse_madt();
extern StackVector<cpu, max_ncpu> cpus;
}
