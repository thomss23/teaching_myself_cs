#ifndef CUSTOM_MALLOC_H
#define CUSTOM_MALLOC_H

#include <stddef.h>

void *myalloc(size_t bytes);
void myfree(void *ptr);

#endif
