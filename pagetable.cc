#include "pagetable.hh"
#include "memory.hh"

namespace mmu {
void map(vaddr addr, u64 phys) {
    cr3 _cr3{processor::read_cr3()};
    pml4e *_pml4 = &_cr3.PML4ptr()[addr.PML4()];
    _pml4->present = 1;
    _pml4->readWrite = 1;
    if (_pml4->PDPTptr() == nullptr) {
        _pml4->PDPTptr(memory::alloc_page());
    }
    pdpte_pd *_pdpte = _pml4->PDPTptr()->to_pd();
    _pdpte->present = 1;
    _pdpte->readWrite = 1;
    if (_pdpte->pd() == nullptr) {
        _pdpte->pd(memory::alloc_page());
    }
    pde_pt *_pde = _pdpte->pd()->to_pt();
    _pde->present = 1;
    _pde->readWrite = 1;
    if (_pde->pt() == nullptr) {
        _pde->pt(memory::alloc_page());
    }
    pte *_pte = _pde->pt();
    _pte->present = 1;
    _pte->readWrite = 1;
    _pte->page(memory::alloc_page());
}
}
