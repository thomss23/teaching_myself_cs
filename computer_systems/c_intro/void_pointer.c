#include <stdio.h>

struct Box {
    char* name;
    void* value;
};

int main() {
    struct Box b1 = {"foo", "box"};
    int n = 5;
    struct Box b2 = {"bar", &n};

    printf("values are: %s and %d\n", (char*) b1.value, *(int*) b2.value);
}
