#include <cstring>
#include "memory.hh"
#include <cstddef>
#include "logger.hh"
extern "C" void *_memset(void *p, int val, size_t sz);
namespace memory {
u8 *max_page_addr;
u8 *allocation_addr = reinterpret_cast<u8 *>(1024*1024*10);

extern logger::module memory;

u8 *alloc_page() {
    logger::debug(memory, "had %x now %x\n", allocation_addr, allocation_addr-4096);
    allocation_addr -= 4096;
    memset(allocation_addr, 0, 4096);
    return allocation_addr;
}

void free_page(void *page) {}

}
