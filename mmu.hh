/*
 * Copyright (C) 2013 Cloudius Systems, Ltd.
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#ifndef MMU_HH
#define MMU_HH

#include <stdint.h>
#include <boost/intrusive/set.hpp>
#include "types.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include "addr_range.hh"
#include "error.h"
#include "align.hh"
#include "mmu-defs.hh"

struct exception_frame;
class balloon;
typedef std::shared_ptr<balloon> balloon_ptr;

/**
 * MMU namespace
 */
namespace mmu {

// when we know it was dynamically allocated
inline phys virt_to_phys_dynamic_phys(void* virt)
{
    return static_cast<char*>(virt) - phys_mem;
}

constexpr inline unsigned pt_index(void *virt, unsigned level)
{
    return (reinterpret_cast<ulong>(virt) >> (page_size_shift + level * pte_per_page_shift)) & (pte_per_page - 1);
}

struct page_allocator;

class vma {
public:
    vma(addr_range range, unsigned perm, unsigned flags, bool map_dirty, page_allocator *page_ops = nullptr);
    virtual ~vma();
    void set(uintptr_t start, uintptr_t end);
    void protect(unsigned perm);
    uintptr_t start() const;
    uintptr_t end() const;
    void* addr() const;
    uintptr_t size() const;
    unsigned perm() const;
    unsigned flags() const;
    virtual void fault(uintptr_t addr, exception_frame *ef);
    virtual void split(uintptr_t edge) = 0;
    virtual error sync(uintptr_t start, uintptr_t end) = 0;
    virtual int validate_perm(unsigned perm) { return 0; }
    virtual page_allocator* page_ops();
    void update_flags(unsigned flag);
    bool has_flags(unsigned flag);
    template<typename T> ulong operate_range(T mapper, void *start, size_t size);
    template<typename T> ulong operate_range(T mapper);
    bool map_dirty();
    class addr_compare;
protected:
    addr_range _range;
    unsigned _perm;
    unsigned _flags;
    bool _map_dirty;
    page_allocator *_page_ops;
public:
    boost::intrusive::set_member_hook<> _vma_list_hook;
};

// compare object for searching the vma list
// defines a partial ordering: if a range intersects a vma,
// it is considered equal, if it is completely before it is less
// than the vma, if it is completely after it is after the vma.
//
// this partial ordering is compatible with vma_list_type.
class vma::addr_compare {
public:
    bool operator()(const vma& x, addr_range y) const { return x.end() <= y.start(); }
    bool operator()(addr_range x, const vma& y) const { return x.end() <= y.start(); }
};

class anon_vma : public vma {
public:
    anon_vma(addr_range range, unsigned perm, unsigned flags);
    virtual void split(uintptr_t edge) override;
    virtual error sync(uintptr_t start, uintptr_t end) override;
};
void* map_anon(const void* addr, size_t size, unsigned flags, unsigned perm);

error munmap(const void* addr, size_t size);
error mprotect(const void *addr, size_t size, unsigned int perm);
error msync(const void* addr, size_t length, int flags);
error mincore(const void *addr, size_t length, unsigned char *vec);
bool is_linear_mapped(const void *addr, size_t size);
bool ismapped(const void *addr, size_t size);
bool isreadable(void *addr, size_t size);

template<int N>
inline bool pte_is_cow(pt_element<N> pte)
{
    return false;
}

template<>
inline bool pte_is_cow(pt_element<0> pte)
{
    return pte.sw_bit(pte_cow); // only 4k pages can be cow for now
}

template<int N>
inline pt_element<N> clear_pte(hw_ptep<N> ptep)
{
    auto old = ptep.exchange(make_empty_pte<N>());
    trace_clear_pte(ptep.release(), pte_is_cow(old), old.addr());
    return old;
}

template<int N>
inline bool clear_accessed(hw_ptep<N> ptep)
{
    pt_element<N> pte = ptep.read();
    bool accessed = pte.accessed();
    if (accessed) {
        pt_element<N> clear = pte;
        clear.set_accessed(false);
        ptep.compare_exchange(pte, clear);
    }
    return accessed;
}

template<int N>
inline bool clear_dirty(hw_ptep<N> ptep)
{
    static_assert(pt_level_traits<N>::leaf_capable::value, "non leaf pte");
    pt_element<N> pte = ptep.read();
    bool dirty = pte.dirty();
    if (dirty) {
        pt_element<N> clear = pte;
        clear.set_dirty(false);
        ptep.compare_exchange(pte, clear);
    }
    return dirty;
}

template<int N>
inline pt_element<N> make_intermediate_pte(hw_ptep<N> ptep, phys addr)
{
    static_assert(pt_level_traits<N>::intermediate_capable::value, "level 0 pte cannot be intermediate");
    return make_pte<N>(addr, false);
}

template<int N>
inline pt_element<N> make_leaf_pte(hw_ptep<N> ptep, phys addr,
                                   unsigned perm = perm_rwx,
                                   mattr mem_attr = mattr_default)
{   
    static_assert(pt_level_traits<N>::leaf_capable::value, "non leaf pte");
    return make_pte<N>(addr, true, perm, mem_attr);
}

class virt_pte_visitor {
public:
    virtual void pte(pt_element<0>) = 0;
    virtual void pte(pt_element<1>) = 0;
};

void virt_visit_pte_rcu(uintptr_t virt, virt_pte_visitor& visitor);

template<int N>
inline bool write_pte(void *addr, hw_ptep<N> ptep, pt_element<N> old_pte, pt_element<N> new_pte)
{
    new_pte.mod_addr(virt_to_phys(addr));
    return ptep.compare_exchange(old_pte, new_pte);
}

template<int N>
inline bool write_pte(void *addr, hw_ptep<N> ptep, pt_element<N> pte)
{
    pte.mod_addr(virt_to_phys(addr));
    return ptep.compare_exchange(ptep.read(), pte);
}

pt_element<0> pte_mark_cow(pt_element<0> pte, bool cow);

#define CONF_debug_memory true

template <typename OutputFunc>
inline
void virt_to_phys(void* vaddr, size_t len, OutputFunc out)
{
    if (CONF_debug_memory && vaddr >= debug_base) {
        while (len) {
            auto next = std::min(align_down(vaddr + page_size, page_size), vaddr + len);
            size_t delta = static_cast<char*>(next) - static_cast<char*>(vaddr);
            out(virt_to_phys(vaddr), delta);
            vaddr = next;
            len -= delta;
        }
    } else {
        out(virt_to_phys(vaddr), len);
    }
}

void* phys_to_virt(phys pa);

template <typename T>
T* phys_cast(phys pa)
{
    return static_cast<T*>(phys_to_virt(pa));
}

inline
bool is_page_aligned(intptr_t addr)
{
    return !(addr & (page_size-1));
}

inline
bool is_page_aligned(void* addr)
{
    return is_page_aligned(reinterpret_cast<intptr_t>(addr));
}

// The mattr type is defined differently for each architecture
// and interpreted by the architecture-specific code, and has
// an architecture-specific meaning.
// Currently mem_attr is ignored on x86_64. For aarch64 specifics see
// definitions in arch/aarch64/arch-mmu.hh
void linear_map(void* virt, phys addr, size_t size,
                size_t slop = mmu::page_size,
                mattr mem_attr = mmu::mattr_default);

void free_initial_memory_range(uintptr_t addr, size_t size);
void switch_to_runtime_page_tables();

void set_nr_page_sizes(unsigned nr);

void vpopulate(void* addr, size_t size);
void vdepopulate(void* addr, size_t size);
void vcleanup(void* addr, size_t size);

error  advise(void* addr, size_t size, int advice);

void vm_fault(uintptr_t addr, exception_frame* ef);

std::string procfs_maps();

unsigned long all_vmas_size();

}

#endif /* MMU_HH */
