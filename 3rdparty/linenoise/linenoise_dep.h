#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *_malloc(size_t sz);
void _free(void *ptr);
void *_realloc(void *ptr, size_t sz);

#ifdef __cplusplus
}
#endif
