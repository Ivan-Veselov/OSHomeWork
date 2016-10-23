#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <stdint.h>
#include <stddef.h>

void* malloc(uint64_t size);
void free(void *addr);

#endif /*__MALLOC__*/
