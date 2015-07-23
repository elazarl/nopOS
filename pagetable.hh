#pragma once
#include "types.h"

namespace mmu {
// we assume no pcide
struct u64_entry {
    u64_entry(const u64 &u) { *reinterpret_cast<u64 *>(this) = u; }
};

struct vaddr_4k {
    u64 offset : 12;
    unsigned table : 9;
    unsigned directory : 9;
    unsigned directoryPtr : 9;
    unsigned PML4 : 9;
};
static_assert(sizeof(vaddr_4k) == 8, "vaddr is 64 bit");

struct vaddr_2m {
    u64 offset : 31;
    unsigned directory : 9;
    unsigned directoryPtr : 9;
    unsigned PML4 : 9;
};
static_assert(sizeof(vaddr_2m) == 8, "vaddr is 64 bit");

struct vaddr_1g {
    u64 offset : 40;
    unsigned directoryPtr : 9;
    unsigned PML4 : 9;
};
static_assert(sizeof(vaddr_1g) == 8, "vaddr is 64 bit");

struct vaddr {
    vaddr(u64 u) { *reinterpret_cast<u64 *>(this) = u; }
    vaddr(void *u) { *reinterpret_cast<u64 *>(this) = reinterpret_cast<u64>(u); }
    union {
        vaddr_4k _4k;
        vaddr_2m _2m;
        vaddr_1g _1g;
    };
    u8 *ptr() { return reinterpret_cast<u8 *>(*reinterpret_cast<u64 *>(this)); }
    u64 directoryPtr() { return _4k.directoryPtr; }
    u64 PML4() { return _4k.PML4; }
};

struct page_prefix {
    // Present; must be 1 to reference a page-directory-pointer table
    unsigned present : 1;
    // Read/write; if 0, writes may not be allowed to the 512-GByte region
    // controlled by this entry (see Section 4.6)
    unsigned readWrite : 1;
    // User/supervisor; if 0, user-mode accesses are not allowed to the
    // 512-GByte region controlled by this entry
    unsigned userSupervisor : 1;
    // Page-level write-through; indirectly determines the memory type used
    // to
    // access the page-directory-pointer table
    // referenced by this entry
    unsigned PWT : 1;
    // Page-level cache disable; indirectly determines the memory type used to
    // access the page-directory-pointer table referenced by this entry
    unsigned PCD : 1;
    // Accessed; indicates whether this entry has been used for
    // linear-address
    // translation (see Section 4.8)
    unsigned accessed : 1;
    // Dirty; indicates whether software has written to the 1-GByte page
    // referenced by this entry (see Section 4.8)
    // dirty is ignored when referring PD
    unsigned dirty : 1;
    // Page size; must be 1 (otherwise, this entry references a page
    // directory;
    // see Table 4-16)
    unsigned PS : 1;
} __attribute__((packed));
static_assert(sizeof(page_prefix) == 1, "page_prefix should be 8 bit");

struct pte {
    // Present; must be 1 to reference a page-directory-pointer table
    unsigned present : 1;
    // Read/write; if 0, writes may not be allowed to the 512-GByte region
    // controlled by this entry (see Section 4.6)
    unsigned readWrite : 1;
    // User/supervisor; if 0, user-mode accesses are not allowed to the
    // 512-GByte region controlled by this entry
    unsigned userSupervisor : 1;
    // Page-level write-through; indirectly determines the memory type used
    // to
    // access the page-directory-pointer table
    // referenced by this entry
    unsigned PWT : 1;
    // Page-level cache disable; indirectly determines the memory type used
    // to
    // access the page-directory-pointer table
    // referenced by this entry
    unsigned PCD : 1;
    // Accessed; indicates whether this entry has been used for
    // linear-address
    // translation (see Section 4.8)
    unsigned accessed : 1;
    // Dirty; indicates whether software has written to the 1-GByte page
    // referenced by this entry (see Section 4.8)
    // dirty is ignored when referring PD
    unsigned dirty : 1;
    // Page size; must be 1 (otherwise, this entry references a page directory;
    // see Table 4-16)
    unsigned PAT : 1;
    // Global; if CR4.PGE = 1, determines whether the translation is global (see
    // Section 4.10); ignored otherwise
    unsigned global : 1;
    unsigned ignored : 3;
    // Physical address of the 4-KByte referenced by this entry
    u64 addr : 40;
    unsigned ignored2 : 7;
    // Protection key; if CR4.PKE = 1, determines the protection key of the
    // page (see Section 4.6.2); ignored otherwise
    unsigned protectionKey : 4;
    // If IA32_EFER.NXE = 1, execute-disable (if 1, instruction fetches are
    // not allowed from the 512-GByte region controlled by this entry; see
    // Section 4.6); otherwise, reserved (must be 0)
    unsigned XD : 1;
    u8 *page() { return reinterpret_cast<u8 *>(addr << 12); }
    template <typename T> void print(T p) {
        p((char *)"pte{P:%d RW:%d US:%d PWT:%d PCD:%d A:%d D:%d PAT:%d page:%x "
                  "XD:%d}",
          present, readWrite, userSupervisor, PWT, PCD, accessed, dirty, PAT,
          page(), XD);
    }
} __attribute__((packed));
static_assert(sizeof(pte) == 8, "pte should be 64 bit");

struct pde_pt {
    page_prefix prefix;
    unsigned ignored : 4;
    u64 addr : 40;
    unsigned ignored2 : 11;
    // If IA32_EFER.NXE = 1, execute-disable (if 1, instruction fetches are not
    // allowed from the 512-GByte region controlled by this entry; see Section
    // 4.6); otherwise, reserved (must be 0)
    unsigned XD : 1;
    pte *pt() { return reinterpret_cast<pte *>(addr << 12); }
    template <typename T> void print(T p) {
        p((char *)"pde(pt){P:%d RW:%d US:%d PWT:%d PCD:%d A:%d pte:%x XD:%d}",
          prefix.present, prefix.readWrite, prefix.userSupervisor, prefix.PWT,
          prefix.PCD, prefix.accessed, pte(), XD);
    }
} __attribute__((packed));
static_assert(sizeof(pde_pt) == 8, "pde2m should be 64 bit");

struct pde2m {
    page_prefix prefix;
    // Global; if CR4.PGE = 1, determines whether the translation is global
    // (see Section 4.10); ignored otherwise
    unsigned global : 1;
    unsigned ignored : 3;
    // Indirectly determines the memory type used to access the 1-GByte page
    // referenced by this entry (see Section 4.9.2)
    unsigned PAT : 1;
    unsigned reserved : 8; // must be 0
    // Physical address of the 2-MByte page referenced by this entry
    u64 addr : 31;
    unsigned ignored2 : 7;
    // Protection key; if CR4.PKE = 1, determines the protection key of the
    // page (see Section 4.6.2); ignored otherwise
    unsigned protectionKey : 4;
    // If IA32_EFER.NXE = 1, execute-disable (if 1, instruction fetches are
    // not allowed from the 512-GByte region controlled by this entry; see
    // Section 4.6); otherwise, reserved (must be 0)
    unsigned XD : 1;
    u8 *page() { return reinterpret_cast<u8 *>(addr << 20); }
    template <typename T> void print(T p) {
        p((char *)"pde(2m){P:%d RW:%d US:%d PWT:%d PCD:%d A:%d D:%d page:%x "
                  "XD:%d}",
          prefix.present, prefix.readWrite, prefix.userSupervisor, prefix.PWT,
          prefix.PCD, prefix.accessed, prefix.dirty, page(), XD);
    }
} __attribute__((packed));
static_assert(sizeof(pde2m) == 8, "page_prefix should be 8 bit");

enum class pde_type {
    PDE_2M,
    PDE_PT,
};

struct pde {
    union {
        pde2m _2m;
        pde_pt pt;
    };
    bool present() { return _2m.prefix.present; }
    pde_type type() {
        return _2m.prefix.PS ? pde_type::PDE_2M : pde_type::PDE_PT;
    }
    pde2m to_2m() {
        assert(type() == pde_type::PDE_2M);
        return _2m;
    }
    pde_pt to_pt() {
        assert(type() == pde_type::PDE_PT);
        return pt;
    }
    template <typename T> void print(T p) {
        switch (type()) {
        case pde_type::PDE_2M:
            _2m.print(p);
            break;
        case pde_type::PDE_PT:
            pt.print(p);
            break;
        }
    }
} __attribute__((packed));

struct pdpte_pd {
    page_prefix prefix;
    unsigned ignored : 4;
    u64 addr : 40;
    unsigned ignored2 : 11;
    // If IA32_EFER.NXE = 1, execute-disable (if 1, instruction fetches are not
    // allowed from the 512-GByte region controlled by this entry; see Section
    // 4.6); otherwise, reserved (must be 0)
    unsigned XD : 1;
    pde *pd() { return reinterpret_cast<pde *>(addr << 12); }
    template <typename T> void print(T p) {
        p((char *)"pdpte(pd){P:%d RW:%d US:%d PWT:%d PCD:%d A:%d "
                  "PD:%x XD:%d}",
          prefix.present, prefix.readWrite, prefix.userSupervisor, prefix.PWT,
          prefix.PCD, prefix.accessed, pd(), XD);
    }
} __attribute__((packed));
static_assert(sizeof(pdpte_pd) == 8, "pdpte should be 64 bit");

struct pdpte1g {
    page_prefix prefix;
    // Global; if CR4.PGE = 1, determines whether the translation is global
    // (see
    // Section 4.10); ignored otherwise
    unsigned global : 1;
    unsigned ignored : 3;
    // Indirectly determines the memory type used to access the 1-GByte page
    // referenced by this entry (see Section 4.9.2)
    unsigned PAT : 1;
    unsigned reserved : 17; // must be 0
    // Physical address of the 1-GByte page referenced by this entry
    u64 addr : 22;
    unsigned ignored2 : 7;
    // Protection key; if CR4.PKE = 1, determines the protection key of the
    // page (see Section 4.6.2); ignored otherwise
    unsigned protectionKey : 4;
    // If IA32_EFER.NXE = 1, execute-disable (if 1, instruction fetches are
    // not allowed from the 512-GByte region controlled by this entry; see
    // Section 4.6); otherwise, reserved (must be 0)
    unsigned XD : 1;
    u8 *page() { return reinterpret_cast<u8 *>(addr << 30); }
    template <typename T> void print(T p) {
        p((char *)"pdpte(1g){P:%d RW:%d US:%d PWT:%d PCD:%d A:%d D:%d "
                  "addr:%x XD:%d}",
          prefix.present, prefix.readWrite, prefix.userSupervisor, prefix.PWT,
          prefix.PCD, prefix.accessed, prefix.dirty, page(), XD);
    }
} __attribute__((packed));
static_assert(sizeof(pdpte1g) == 8, "pdpte should be 64 bit");

enum class pdpt_type {
    PDPT_1G,
    PDPT_PD
};

struct pdpte {
    union {
        pdpte1g _1g;
        pdpte_pd pd;
    };
    bool present() { return _1g.prefix.present; }
    pdpt_type type() {
        return _1g.prefix.PS ? pdpt_type::PDPT_1G : pdpt_type::PDPT_PD;
    }
    pdpte1g to_1g() {
        assert(type() == pdpt_type::PDPT_1G);
        return _1g;
    }
    pdpte_pd to_pd() {
        assert(type() == pdpt_type::PDPT_PD);
        return pd;
    }
    template <typename T> void print(T p) {
        switch (type()) {
        case pdpt_type::PDPT_1G:
            _1g.print(p);
            break;
        case pdpt_type::PDPT_PD:
            pd.print(p);
            break;
        }
    }
} __attribute__((packed));

struct pml4e {
    // Present; must be 1 to reference a page-directory-pointer table
    unsigned present : 1;
    // Read/write; if 0, writes may not be allowed to the 512-GByte region
    // controlled by this entry (see Section 4.6)
    unsigned readWrite : 1;
    // User/supervisor; if 0, user-mode accesses are not allowed to the
    // 512-GByte region controlled by this entry
    unsigned userSupervisor : 1;
    // Page-level write-through; indirectly determines the memory type used
    // to
    // access the page-directory-pointer table
    // referenced by this entry
    unsigned PWT : 1;
    // Page-level cache disable; indirectly determines the memory type used to
    // access the page-directory-pointer table referenced by this entry
    unsigned PCD : 1;
    // Accessed; indicates whether this entry has been used for linear-address
    // translation (see Section 4.8)
    unsigned accessed : 1;
    unsigned ignored : 1;
    unsigned PS : 1; // reserved, must be 0
    unsigned ignored2 : 4;
    // Physical address of 4-KByte aligned page-directory-pointer table
    // referenced by this entry
    u64 PDPT : 40;
    unsigned ignored3 : 11;
    // If IA32_EFER.NXE = 1, execute-disable (if 1, instruction fetches are not
    // allowed from the 512-GByte region controlled by this entry; see Section
    // 4.6); otherwise, reserved (must be 0)
    unsigned XD : 1;
    u64 PDPTu64() { return PDPT << 12; }
    pdpte *PDPTptr() { return reinterpret_cast<pdpte *>(PDPTu64()); }
    template <typename T> void print(T p) {
        p((char *)"pml4e{P:%d RW:%d US:%d PWT:%d PCD:%d A:%d PDPT:%x XD:%d}",
          present, readWrite, userSupervisor, PWT, PCD, accessed, PDPTu64(),
          XD);
    }
} __attribute__((packed));

struct cr3 : u64_entry {
    cr3(const u64 &u) : u64_entry(u) {}
    /* assuming 1:1 mapping, ptr to the physical addr could be useful */
    pml4e *PML4ptr() { return reinterpret_cast<pml4e *>(PML4u64()); }
    u64 PML4u64() { return PML4 << 12; }
    template <typename T> void print(T p) {
        p((char *)"cr3{PWT:%d, PCD:%d PML4: %x}", PWT, PCD, PML4u64());
    }

  private:
    unsigned ignored : 3;
    /* Page-level write-through; indirectly determines the memory type used to
     * access the PML4 table during linearaddress translation
     */
    unsigned PWT : 1;
    /* Page-level cache disable; indirectly determines the memory type used to
     * access the PML4 table during linear-address translation
     */
    unsigned PCD : 1;
    unsigned ignored2 : 7;
    u64 PML4 : 40;
    u64 ignored3 : 2;
} __attribute__((packed));
}
