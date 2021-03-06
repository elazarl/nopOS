#include <cstddef>
#include "processor.hh"

void *elf_header;
int __argc;
char** __argv;

namespace memory {
size_t phys_mem_size;
}

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

void for_each_e820_entry(void* e820_buffer, unsigned size, void (*f)(e820ent e))
{
    auto p = e820_buffer;
    while (p < e820_buffer + size) {
        auto ent = static_cast<e820ent*>(p);
        if (ent->type == 1) {
            f(*ent);
        }
        p += ent->ent_size + 4;
    }
}

bool intersects(const e820ent& ent, u64 a)
{
    return a > ent.addr && a < ent.addr + ent.size;
}

e820ent truncate_below(e820ent ent, u64 a)
{
    u64 delta = a - ent.addr;
    ent.addr += delta;
    ent.size -= delta;
    return ent;
}

e820ent truncate_above(e820ent ent, u64 a)
{
    u64 delta = ent.addr + ent.size - a;
    ent.size -= delta;
    return ent;
}


/*void arch_setup_free_memory()
{
    static ulong edata;
    asm ("movl $.edata, %0" : "=rm"(edata));
    // copy to stack so we don't free it now
    auto omb = *osv_multiboot_info;
    auto mb = omb.mb;
    auto e820_buffer = alloca(mb.mmap_length);
    auto e820_size = mb.mmap_length;
    memcpy(e820_buffer, reinterpret_cast<void*>(mb.mmap_addr), e820_size);
    for_each_e820_entry(e820_buffer, e820_size, [] (e820ent ent) {
        memory::phys_mem_size += ent.size;
    });
    constexpr u64 initial_map = 1 << 30; // 1GB mapped by startup code

    / *u64 time;
    time = omb.tsc_init_hi;
    time = (time << 32) | omb.tsc_init;
    boot_time.arrays[0] = { "", time };

    time = omb.tsc_disk_done_hi;
    time = (time << 32) | omb.tsc_disk_done;
    boot_time.arrays[1] = { "disk read (real mode)", time };* /

    auto c = processor::cpuid(0x80000000);
    if (c.a >= 0x80000008) {
        c = processor::cpuid(0x80000008);
        mmu::phys_bits = c.a & 0xff;
        mmu::virt_bits = (c.a >> 8) & 0xff;
        assert(mmu::phys_bits <= mmu::max_phys_bits);
    }

    setup_temporary_phys_map();

    // setup all memory up to 1GB.  We can't free any more, because no
    // page tables have been set up, so we can't reference the memory being
    // freed.
    for_each_e820_entry(e820_buffer, e820_size, [] (e820ent ent) {
        // can't free anything below edata, it's core code.
        // FIXME: can free below 2MB.
        if (ent.addr + ent.size <= edata) {
            return;
        }
        if (intersects(ent, edata)) {
            ent = truncate_below(ent, edata);
        }
        // ignore anything above 1GB, we haven't mapped it yet
        if (intersects(ent, initial_map)) {
            ent = truncate_above(ent, initial_map);
        } else if (ent.addr >= initial_map) {
            return;
        }
        mmu::free_initial_memory_range(ent.addr, ent.size);
    });
    for (auto&& area : mmu::identity_mapped_areas) {
        auto base = reinterpret_cast<void*>(get_mem_area_base(area));
        mmu::linear_map(base, 0, initial_map, initial_map);
    }
    // map the core, loaded 1:1 by the boot loader
    / *mmu::phys elf_phys = reinterpret_cast<mmu::phys>(elf_header);
    elf_start = reinterpret_cast<void*>(elf_header);
    elf_size = edata - elf_phys;
    mmu::linear_map(elf_start, elf_phys, elf_size, OSV_KERNEL_BASE);
    // get rid of the command line, before low memory is unmapped
    parse_cmdline(mb);* /
    // now that we have some free memory, we can start mapping the rest
    mmu::switch_to_runtime_page_tables();
    for_each_e820_entry(e820_buffer, e820_size, [] (e820ent ent) {
        // Ignore memory already freed above
        if (ent.addr + ent.size <= initial_map) {
            return;
        }
        if (intersects(ent, initial_map)) {
            ent = truncate_below(ent, initial_map);
        }
        for (auto&& area : mmu::identity_mapped_areas) {
        auto base = reinterpret_cast<void*>(get_mem_area_base(area));
            mmu::linear_map(base + ent.addr, ent.addr, ent.size, ~0);
        }
        mmu::free_initial_memory_range(ent.addr, ent.size);
    });
}*/
