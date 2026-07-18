#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>

bool ispangram(char *s) {
  int length = strlen(s);

  bool used[26] = { false };
  for (int i = 0; i < length; i++) {
    char c = tolower(s[i]);
    if (c >= 'a' && c <= 'z') {
      used[c - 'a'] = 1; 
    }
  }

    int sum = 0;
    for(int i = 0; i < 26; i++) {
      sum += used[i];
    }

    return sum == 26;
}

int main() {
  size_t len;
  size_t read;
  char *line = NULL;
  while ((read = getline(&line, &len, stdin)) != -1) {
    if (ispangram(line))
      printf("%s", line);
  }

  if (ferror(stdin))
    fprintf(stderr, "Error reading from stdin");

  free(line);
  fprintf(stderr, "ok\n");
}
