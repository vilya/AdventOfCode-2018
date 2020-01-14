#include <cstdio>

static constexpr int kSize = 50000;

int main(int argc, char** argv)
{
  char line[kSize + 1];
  FILE* f = fopen(argv[1], "r");
  fread(line, sizeof(char), kSize, f);
  fclose(f);
  line[kSize] = '\0';

  int first = 0;
  int next[kSize];
  for (int i = 0; i < kSize; i++) {
    next[i] = i + 1;
  }

  bool reacted = true;
  int n = kSize;
  while (reacted) {
    reacted = false;
    for (int *prev = &first, i = first; next[i] < kSize; prev = &next[i], i = next[i]) {
      char a = line[i];
      char b = line[next[i]];
      if (a != b && (a | 32) == (b | 32)) {
        *prev = next[next[i]];
        n -= 2;
        reacted = true;
        break;
      }
      prev = &next[i];
    }
  }

  printf("Result = %d\n", n);
}
