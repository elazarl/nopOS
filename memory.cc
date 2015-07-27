#include <cstring>
#include "memory.hh"
#include <cstddef>
extern "C" void *_memset(void *p, int val, size_t sz);
namespace memory {
u8 *max_page_addr;

u8 *alloc_page() {
    max_page_addr -= 4096;
    memset(max_page_addr, 0, 4096);
    return max_page_addr;
}

void free_page(void *page) {}

}
