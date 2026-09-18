#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define SHM_NAME "/number_shm"
#define DATA_READY_SEM "/number_ready"
#define BUFFER_FREE_SEM "/number_buffer_free"

#define SIZE (1 << 20) // Test with this many bytes of data

int main () {

  srand(0x1234);
  int max = SIZE / sizeof(int);
  int n;
  int checksum;

  // Start a fresh shared-memory session.  Do this only when no old client is
  // still using these names.
  shm_unlink(SHM_NAME);
  sem_unlink(DATA_READY_SEM);
  sem_unlink(BUFFER_FREE_SEM);

  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open");
    return EXIT_FAILURE;
  }
  if (ftruncate(fd, SIZE) == -1) {
    perror("ftruncate");
    return EXIT_FAILURE;
  }

  int *shared = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared == MAP_FAILED) {
    perror("mmap");
    return EXIT_FAILURE;
  }
  close(fd);

  sem_t *data_ready = sem_open(DATA_READY_SEM, O_CREAT, 0666, 0);
  sem_t *buffer_free = sem_open(BUFFER_FREE_SEM, O_CREAT, 0666, 1);
  if (data_ready == SEM_FAILED || buffer_free == SEM_FAILED) {
    perror("sem_open");
    return EXIT_FAILURE;
  }
  for (;;) {
    // Do not overwrite the buffer until the client has read the last batch.
    sem_wait(buffer_free);

    checksum = 0;
    for (int i = 0; i < max; i++) {
      n = rand();
      checksum ^= n;
      shared[i] = n;
      
    }

    printf("Sent %d random ints to client, checksum %d\n", max, checksum);
    sem_post(data_ready);
  }

}

