// implicit free list without coalescing and always allocating (no splitting)
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "custom_malloc.h"

#define ARENA_SIZE (1024 * 1024)


static void *arena;
static unsigned char *heap_start;
static unsigned char *heap_end;
static unsigned char *heap_limit;

typedef struct block_header {
    size_t size;
    int is_free;
} block_header;


void* myalloc(size_t bytes) {
    if (arena == NULL) {
        arena = mmap(
        NULL,
        ARENA_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0);

        if (arena == MAP_FAILED) {
            perror("mmap");
            arena = NULL;
            return NULL;
        }    

        heap_start = arena;
        heap_end   = arena;
        heap_limit = heap_start + ARENA_SIZE;
    }

    if (bytes > SIZE_MAX - sizeof(block_header)) {
        return NULL;
    }

    unsigned char* current = heap_start;

    while(current < heap_end) {
        block_header* block = (block_header *) current;

        if(block->is_free == 1 && block->size >= bytes) {
            block->is_free = 0;
            void* payload = (unsigned char *) block + sizeof(block_header);
            return payload;
        }

        current += sizeof(block_header) + block->size;
    }

    size_t required_size = sizeof(block_header) + bytes;

    if (heap_end + required_size > heap_limit) {
        return NULL;
    }

    block_header *block = (block_header *) heap_end;

    /* size is payload capacity, not the total header-plus-payload size. */
    block->size = bytes;
    block->is_free = 0;

    void* payload = (unsigned char *) block + sizeof(block_header);

    heap_end += required_size;

    return payload;

}



void myfree(void* p) {

    if (p == NULL || arena == NULL) {
        return;
    }

    unsigned char *payload = p;
    
    if (payload < heap_start + sizeof(block_header) || payload >= heap_end) {
        return;
    }

    block_header *block = (block_header *)(payload - sizeof(block_header));

    block->is_free = 1;
}
