#include <cstring>
#include "memory.hh"
#include <cstddef>
#include "logger.hh"
extern "C" void *_memset(void *p, int val, size_t sz);
namespace memory {
u8 *max_page_addr;

extern logger::module memory;

u8 *alloc_page() {
    logger::debug(memory, "had %x now %x\n", max_page_addr, max_page_addr-4096);
    max_page_addr -= 4096;
    memset(max_page_addr, 0, 4096);
    return max_page_addr;
}

void free_page(void *page) {}

}
