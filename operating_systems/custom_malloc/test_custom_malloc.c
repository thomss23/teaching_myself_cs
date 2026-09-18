#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "custom_malloc.h"

int main(void) {
    int *numbers = myalloc(3 * sizeof(*numbers));
    char *message = myalloc(32);

    assert(numbers != NULL);
    assert(message != NULL);

    numbers[0] = 10;
    numbers[1] = 20;
    numbers[2] = 30;
    strcpy(message, "allocator test");

    assert(numbers[0] + numbers[1] + numbers[2] == 60);
    assert(strcmp(message, "allocator test") == 0);

    myfree(numbers);

    /* This request fits in numbers' freed block, so it should reuse it. */
    int *reused = myalloc(sizeof(*reused));
    assert(reused == numbers);
    *reused = 42;
    assert(*reused == 42);

    myfree(reused);
    myfree(message);
    myfree(NULL);

    puts("custom allocator test passed");
    return 0;
}
