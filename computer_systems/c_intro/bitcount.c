#include <assert.h>
#include <stdio.h>


int main() {
    assert(bitcount(0) == 0);
    assert(bitcount(1) == 1);
    assert(bitcount(3) == 2);
    assert(bitcount(8) == 1);
    // harder case:
    assert(bitcount(0xffffffff) == 32);
    printf("OK\n");
}

int bitcount(unsigned int n) {
    unsigned int mask = 1;
    int count = 0;

    while(n != 0) {
        count += n & 0x01;
        // if ((n & mask) == 1) {
        //     count++;
        // }

        n = n >> 1;
    }

    // separate solution
    // while (x != 0) {
    //     x &= (x - 1);
    //     count++;
    // }

    return count;
}