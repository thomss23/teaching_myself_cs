#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

volatile int n = 0;
int message_ready = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t fizzbuzz_cond = PTHREAD_COND_INITIALIZER;

unsigned long generation = 0;

void *fizz(void *arg) {
  unsigned long last_generation = 0;

  while (1) {
    pthread_mutex_lock(&lock);
    while (generation == last_generation) {
        pthread_cond_wait(&ready_cond, &lock);
    }

    int current_n = n;
    last_generation = generation;
    pthread_mutex_unlock(&lock);

    if (current_n % 3 == 0) {
      printf("\tfizz");
    }
  }
}

void *buzz(void *arg) {
  unsigned long last_generation = 0;
  while (1) {
    pthread_mutex_lock(&lock);
    while (generation == last_generation) {
        pthread_cond_wait(&ready_cond, &lock);
    }

    int current_n = n;
    last_generation = generation;
    pthread_mutex_unlock(&lock);

    if (current_n % 5 == 0) {
      printf("\tbuzz");
    }
  }
}

int main () {
  // start two threads, running fizz and buzz respectively
  pthread_t t1, t2;
  pthread_create(&t1, NULL, fizz, NULL);
  pthread_create(&t2, NULL, buzz, NULL);
  // every 100ms, update n randomly from the range [0, 16), indefinitely
  while (1) {
    pthread_mutex_lock(&lock);
    n = rand() & 0xf;
    generation++;
    pthread_cond_broadcast(&ready_cond);
    printf("\n%d:", n);
    pthread_mutex_unlock(&lock);
    usleep(100000);
  }
}
