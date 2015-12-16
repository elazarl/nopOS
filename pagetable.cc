#include "pagetable.hh"
#include "memory.hh"

namespace mmu {
const int page_size = 4096;
void break_2m(pde *_pde)
{
    u8 *start = _pde->page();
    _pde->type(pd_type::PD_PT);
    pde_pt *_pde_pt = _pde->to_pt();
    _pde_pt->pt(memory::alloc_page());
    pte *pte = _pde_pt->pt();
    for (int i=0; i<512; i++) {
        pte[i].present   = 1;
        pte[i].readWrite = 1;
        pte[i].page(start);
        start += 1024*4;
    }
}

void map(vaddr addr, u64 phys) {
    cr3 _cr3{processor::read_cr3()};
    pml4e *_pml4 = &_cr3.PML4ptr()[addr.PML4()];
    _pml4->present = 1;
    _pml4->readWrite = 1;
    if (_pml4->PDPTptr() == nullptr) {
        _pml4->PDPTptr(memory::alloc_page());
    }
    pdpte_pd *_pdpte = (&_pml4->PDPTptr()[addr.directoryPtr()])->to_pd();
    _pdpte->present = 1;
    _pdpte->readWrite = 1;
    if (_pdpte->pd() == nullptr) {
        _pdpte->pd(memory::alloc_page());
    }
    if (_pdpte->pd()->type() == pd_type::PD_2M) {
        break_2m(_pdpte->pd());
    }
    pde_pt *_pde = (&_pdpte->pd()[addr._4k.directory])->to_pt();
    _pde->present = 1;
    _pde->readWrite = 1;
    if (_pde->pt() == nullptr) {
        _pde->pt(memory::alloc_page());
    }
    pte *_pte = &(_pde->pt()[addr._4k.table]);
    _pte->present = 1;
    _pte->readWrite = 1;
    _pte->page(reinterpret_cast<u8 *>(phys));
}
}
