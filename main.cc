#include <cassert>
#include <iostream>
#include "pagetable.hh"

using namespace std;

int main() {
    mmu::vaddr v{nullptr};
    v._4k.PML4 = 511;
    cout << hex << (v.to_u64()&(1ull<<47)) << endl;
}
