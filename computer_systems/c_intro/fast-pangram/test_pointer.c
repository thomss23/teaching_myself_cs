#include<stdio.h>>


int main() {
    int arr[10];

    arr[3] = 42;

    printf("arr[3] = %d\n", *(arr + 3)); // basically for pointers arr[3] is like derefferencing *(arr + 3)
}