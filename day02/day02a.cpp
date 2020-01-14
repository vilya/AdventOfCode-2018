#include <algorithm>
#include <cstdio>
#include <cstring>

int main(int argc, char** argv)
{
  int twos = 0;
  int threes = 0;

  FILE* f = fopen(argv[1], "r");
  char line[256] = {};
  while (fgets(line, sizeof(line), f) != nullptr) {
    int n = strlen(line);
    std::sort(line, line + n);

    bool has_double = false;
    bool has_triple = false;
    int i = 0;
    while (i < n) {
      int j = i + 1;
      while (j < n && line[j] == line[i]) {
        ++j;
      }
      int len = j - i;

      if (len == 2) {
        has_double = true;
      }
      else if (len == 3) {
        has_triple = true;
      }

      i = j;
    }

    twos += has_double;
    threes += has_triple;
  }

  printf("Checksum = %d\n", twos * threes);
}
