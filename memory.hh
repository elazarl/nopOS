#include "types.h"
namespace memory {
u8 *alloc_page();
void free_page(void *p);
extern u8 *max_page_addr;
}
