#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE 4096
#define SHM_NAME "/my_shared_mem"

int main(void) {
  int status;
  srand(time(NULL));

  int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (fd == -1) {
    perror("shm_open");
    return EXIT_FAILURE;
  }

  if (ftruncate(fd, SIZE) == -1) {
    perror("ftruncate");
    close(fd);
    shm_unlink(SHM_NAME);
    return EXIT_FAILURE;
  }

  int *shared = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shared == MAP_FAILED) {
    perror("mmap");
    close(fd);
    shm_unlink(SHM_NAME);
    return EXIT_FAILURE;
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    munmap(shared, SIZE);
    close(fd);
    shm_unlink(SHM_NAME);
    return EXIT_FAILURE;
  }

  if (pid == 0) {
    int n = rand();
    shared[0] = n;
    printf("Child wrote %d to shared address %p\n", n, (void *)shared);

    munmap(shared, SIZE);
    close(fd);
    return EXIT_SUCCESS;
  }

  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
  } else if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) {
    printf("Parent read %d from shared address %p\n", shared[0],
           (void *)shared);
  }

  munmap(shared, SIZE);
  close(fd);
  shm_unlink(SHM_NAME);
  return EXIT_SUCCESS;
}
