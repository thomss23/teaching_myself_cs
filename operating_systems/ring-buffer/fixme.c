#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef int job;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;


struct JobQueue {
    job *buffer;
    int capacity;
    int size;  // number of current items (<= capacity)
    int wp;  // locations at which to write
    int rp;  // ... and read
};

struct JobQueue* JQ_init(int capacity) {
    struct JobQueue *jq = malloc(sizeof(struct JobQueue));
    jq->buffer = malloc(capacity * sizeof(job));
    jq->capacity = capacity;
    jq->size = 0;
    jq->wp = 0;
    jq->rp = 0;
    return jq;
}

void JQ_free(struct JobQueue *jq) {
    free(jq->buffer);
    free(jq);
}

void JQ_add(struct JobQueue*jq, job item) {
    pthread_mutex_lock(&mutex);
    while (jq->size == jq->capacity) {
        pthread_cond_wait(&not_full, &mutex);
    }
    jq->buffer[jq->wp] = item;
    printf("%d -> [%d]\n", item, jq->wp);
    jq->wp = (jq->wp + 1) % jq->capacity;
    jq->size++;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&mutex);
}

job JQ_get(struct JobQueue*jq) {
    pthread_mutex_lock(&mutex);
    while (jq->size == 0) {
        pthread_cond_wait(&not_empty, &mutex);
    }
    job item = jq->buffer[jq->rp];
    printf("     [%d] -> %d\n", jq->rp, item);
    jq->rp = (jq->rp + 1) % jq->capacity;
    jq->size--;
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&mutex);
    return item;
}

void *producer(void *arg) {
  struct JobQueue *jq = (struct JobQueue *)arg;
  int i = 0;
  while (1)
    JQ_add(jq, i++ % 100);
}

void *consumer(void *arg) {
  struct JobQueue *jq = (struct JobQueue *)arg;
  while (1)
    JQ_get(jq);
}

int main () {
  printf("Starting basic test\n");
  struct JobQueue *jq = JQ_init(8);
  int i;
  for (i = 0; i < 5; i++)
    JQ_add(jq, i);
  for (i = 0; i < 5; i++)
    JQ_get(jq);
  for (i = 0; i < 5; i++)
    JQ_add(jq, i);
  for (i = 0; i < 5; i++)
    JQ_get(jq);
  JQ_free(jq);
  printf("Starting concurrent test\n");
  // start n producers, m consumers in threads
  // producer just write incrementing integers to jq indefinitely
  // consumers just read/print them
  int n = 1;
  int m = 1;
  pthread_t prod[n], cons[m];
  jq = JQ_init(4);
  for (i = 0; i < n; i++)
    pthread_create(&prod[i], NULL, producer, jq);
  for (i = 0; i < m; i++)
    pthread_create(&cons[i], NULL, consumer, jq);
  for (i = 0; i < n; i++)
    pthread_join(prod[i], NULL);
  for (i = 0; i < m; i++)
    pthread_join(cons[i], NULL);
  JQ_free(jq);
  printf("OK\n");
}
