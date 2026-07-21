#include <assert.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>

#define STARTING_BUCKETS 8
#define MAX_KEY_SIZE 50

typedef struct Node {
  char *key;
  void *value;
  struct Node *next;
} Node;


typedef struct Hashmap {
  Node **buckets;
  int capacity;
} Hashmap;


int hash(char* key, int size) {
  int sum = 0;
  for(int i = 0; i < strlen(key); i++) {
    sum += (int) key[i];
  }

  return sum % size;
}

Hashmap* Hashmap_new(void) {
  Hashmap* h = malloc(sizeof(Hashmap));
  h->buckets = calloc(STARTING_BUCKETS, sizeof(Node*));
  h->capacity = STARTING_BUCKETS;

  return h;
}

void Hashmap_set(Hashmap *h, char *key, void *value) {
  int index = hash(key, h->capacity);
  Node *node = h->buckets[index];

  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      node->value = value; 
      return;
    }

    node = node->next;
  }

  Node *new_node = malloc(sizeof *new_node);
  new_node->key = strdup(key);
  new_node->value = value;
  new_node->next = h->buckets[index];
  h->buckets[index] = new_node;
}

void *Hashmap_get(Hashmap* h, char* key) {
  int index = hash(key, h->capacity);
  Node *node = h->buckets[index];

  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }

    node = node->next;
  }

  return NULL;
}

void Hashmap_delete(Hashmap* h, char* key) {
  int index = hash(key, h->capacity);
  Node *node = h->buckets[index];
  Node* prev = NULL;

 while (node != NULL) {

    if (strcmp(node->key, key) == 0) {

      if (prev == NULL) {
        h->buckets[index] = node->next;
      } else {
        prev->next = node->next;
      }

      free(node->key);
      free(node);
      return;
    }
    prev = node;
    node = node->next;
  }

}

void Hashmap_free(Hashmap *h) {
  for (int i = 0; i < h->capacity; i++) {
    Node *node = h->buckets[i];

    while (node != NULL) {
      Node *next = node->next;

      free(node->key);
      free(node);

      node = next;
    }
  }

  free(h->buckets);
  free(h);
}

int main() {
  Hashmap *h = Hashmap_new();

  // basic get/set functionality
  int a = 5;
  float b = 7.2;
  Hashmap_set(h, "item a", &a);
  Hashmap_set(h, "item b", &b);
  assert(Hashmap_get(h, "item a") == &a);
  assert(Hashmap_get(h, "item b") == &b);

  // using the same key should override the previous value
  int c = 20;
  Hashmap_set(h, "item a", &c);
  assert(Hashmap_get(h, "item a") == &c);

  // basic delete functionality
  Hashmap_delete(h, "item a");
  assert(Hashmap_get(h, "item a") == NULL);

  // handle collisions correctly
  // note: this doesn't necessarily test expansion
  int i, n = STARTING_BUCKETS * 10, ns[n];
  char key[MAX_KEY_SIZE];
  for (i = 0; i < n; i++) {
    ns[i] = i;
    sprintf(key, "item %d", i);
    Hashmap_set(h, key, &ns[i]);
  }
  for (i = 0; i < n; i++) {
    sprintf(key, "item %d", i);
    assert(Hashmap_get(h, key) == &ns[i]);
  }

  Hashmap_free(h);
  /*
     stretch goals:
     - expand the underlying array if we start to get a lot of collisions
     - support non-string keys
     - try different hash functions
     - switch from chaining to open addressing
     - use a sophisticated rehashing scheme to avoid clustered collisions
     - implement some features from Python dicts, such as reducing space use,
     maintaing key ordering etc. see https://www.youtube.com/watch?v=npw4s1QTmPg
     for ideas
     */
  printf("ok\n");
}
