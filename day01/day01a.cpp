#include <cstdio>

int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  int total = 0, tmp;
  while (fscanf(f, "%d", &tmp) == 1) {
    total += tmp;
  }
  fclose(f);
  printf("Total = %d\n", total);
}
