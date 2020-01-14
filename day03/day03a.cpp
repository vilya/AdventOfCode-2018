#include <cstdio>
#include <cstring>
#include <set>
#include <utility>

int main(int argc, char** argv)
{
  std::set<std::pair<int,int>> claims;
  std::set<std::pair<int,int>> overlapping_claims;

  FILE* f = fopen(argv[1], "r");
  char line[256] = {};
  while (fgets(line, sizeof(line), f) != nullptr) {
    int i, x0, y0, w, h;
    sscanf(line, "#%d @ %d,%d: %dx%d", &i, &x0, &y0, &w, &h);

    const int x1 = x0 + w;
    const int y1 = y0 + h;
    for (int y = y0; y < y1; y++) {
      for (int x = x0; x < x1; x++) {
        std::pair<int,int> pos = std::make_pair(y, x);
        if (!claims.insert(pos).second) {
          overlapping_claims.insert(pos);
        }
      }
    }
  }
  fclose(f);

  printf("Result = %d\n", int(overlapping_claims.size()));
}
