#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "cputime.h"

typedef struct {
  int *arr;
  int n;
} msort_args;

void merge(int *arr, int n, int mid) {
  int left = 0, right = mid, i;
  int *x = malloc(n * sizeof(int));
  // copy the ith item from either the left or right part
  for (i = 0; i < n; i++) {
    if (right == n)
      x[i] = arr[left++];
    else if (left == mid)
      x[i] = arr[right++];
    else if (arr[right] < arr[left])
      x[i] = arr[right++];
    else
      x[i] = arr[left++];
  }
  // transfer from temporary array back to given one
  for (i = 0; i < n; i++)
    arr[i] = x[i];
  free(x);
}

void msort(int *arr, int n) {
  if (n < 2)
    return;
  int mid = n / 2;
  msort(arr, mid);
  msort(arr + mid, n - mid);
  merge(arr, n, mid);
}

void* worker(void *arg) {
  msort_args *args = (msort_args *)arg;
  msort(args->arr, args->n);
  return NULL;
}

int main () {
  int n = 1 << 24;
  int *arr = malloc(n * sizeof(int));
  // populate array with n many random integers
  srand(1234);
  for (int i = 0; i < n; i++)
    arr[i] = rand();

  fprintf(stderr, "Sorting %d random integers\n", n);

  // actually sort, and time cpu use
  struct profile_times t;
  profile_start(&t);
  
  pthread_t p1, p2;
  msort_args args1 = {arr, n / 2};
  msort_args args2 = {arr + n / 2, n - n / 2};
  pthread_create(&p1, NULL, worker, &args1);
  pthread_create(&p2, NULL, worker, &args2);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  merge(arr, n, n / 2);
  // msort(arr, n);
  profile_log(&t);

  // assert that the output is sorted
  for (int i = 0; i < n - 1; i++)
    if (arr[i] > arr[i + 1]) {
      printf("error: arr[%d] = %d > arr[%d] = %d", i, arr[i], i + 1,
             arr[i + 1]);
      return 1;
    }
    return 0;
}
