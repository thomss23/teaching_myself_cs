#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <time.h>

#define SHM_NAME "/number_shm"
#define DATA_READY_SEM "/number_ready"
#define BUFFER_FREE_SEM "/number_buffer_free"

#define SIZE (1 << 20) // Test with this many bytes of data

int main () {
  int n, checksum;
  struct timespec start, end;
  int max = SIZE / sizeof(int);

  int fd = shm_open(SHM_NAME, O_RDONLY, 0666);
  if (fd == -1) {
    perror("shm_open (start the server first)");
    return EXIT_FAILURE;
  }
  int *shared = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);
  if (shared == MAP_FAILED) {
    perror("mmap");
    return EXIT_FAILURE;
  }

  sem_t *data_ready = sem_open(DATA_READY_SEM, 0);
  sem_t *buffer_free = sem_open(BUFFER_FREE_SEM, 0);
  if (data_ready == SEM_FAILED || buffer_free == SEM_FAILED) {
    perror("sem_open (start the server first)");
    return EXIT_FAILURE;
  }

  // receive a bunch of data
  clock_gettime(CLOCK_MONOTONIC, &start);
  checksum = 0;

  while(1) {

    sem_wait(data_ready);

    checksum = 0;
    for (int i = 0; i < max; i++) {
      n = shared[i];
      checksum ^= n;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);

    float secs =
        (float)(end.tv_nsec - start.tv_nsec) / 1e9 + (end.tv_sec - start.tv_sec);
    float mibs = (float)SIZE / secs / (1 << 20);

    printf("Received at %.3f MiB/s. Checksum: %d\n", mibs, checksum);

    // The server may now overwrite the buffer with the next batch.
    sem_post(buffer_free);
  }

}
