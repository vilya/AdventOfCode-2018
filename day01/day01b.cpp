#include <cstdio>
#include <unordered_set>
#include <vector>

int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  int tmp;
  std::vector<int> in;
  while (fscanf(f, "%d", &tmp) == 1) {
    in.push_back(tmp);
  }
  fclose(f);

  std::unordered_set<int> found;
  found.insert(0);

  size_t i = 0;
  int total = in[i];
  while (found.insert(total).second) {
    i = (i + 1) % in.size();
//    printf("Total = %d + %d = %d\n", total, in[i], total + in[i]);
    total += in[i];
  }

  printf("Total = %d\n", total);
}
