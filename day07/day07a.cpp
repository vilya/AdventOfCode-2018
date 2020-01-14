#include <algorithm>
#include <cstdio>
#include <vector>

int main(int argc, char** argv)
{
  bool has[26] = {};
  bool done[26] = {};
  bool requires[26][26] = {};

//  printf("digraph {\n");
  FILE* f = fopen(argv[1], "r");
  char src, dst;
  while (fscanf(f, "Step %c must be finished before step %c can begin.\n", &src, &dst) == 2) {
    requires[dst - 'A'][src - 'A'] = true;
    has[src - 'A'] = true;
    has[dst - 'A'] = true;
//    printf("%c -> %c;\n", src, dst);
  }
  fclose(f);
//  printf("}\n");

  for (int i = 0; i < 26; i++) {
    if (!has[i]) {
      done[i] = true;
    }
  }

  int i = 0;
  while (i < 26) {
    if (!done[i]) {
      bool all_requirements_met = true;
      for (int j = 0; j < 26; j++) {
        if (requires[i][j] && !done[j]) {
          all_requirements_met = false;
          break;
        }
      }
      if (all_requirements_met) {
        fputc(i + 'A', stdout);
        done[i] = true;
        i = 0;
        continue;
      }
    }
    ++i;
  }
  fputc('\n', stdout);
}
