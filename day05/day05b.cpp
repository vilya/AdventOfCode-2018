#include <cstdio>
#include <limits>

static constexpr int kSize = 50000;

int main(int argc, char** argv)
{
  char line[kSize + 1];
  FILE* f = fopen(argv[1], "r");
  fread(line, sizeof(char), kSize, f);
  fclose(f);
  line[kSize] = '\0';

  char best_polymer = '\0';
  int best_result = std::numeric_limits<int>::max();
  int first;
  int next[kSize];
  for (char polymer = 'a'; polymer <= 'z'; polymer++) {
    first = 0;
    for (int i = 0; i < kSize; i++) {
      next[i] = i + 1;
    }

    int n = kSize;
    for (int *prev = &first, i = 0; i < kSize; i = next[i]) {
      if ((line[i] | 32) == polymer) {
        *prev = next[i];
        n--;
      }
      else {
        prev = &next[i];
      }
    }

    bool reacted = true;
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

    if (n < best_result) {
      best_polymer = polymer;
      best_result = n;
    }
  }

  printf("Result = %d\n", best_result);
}
