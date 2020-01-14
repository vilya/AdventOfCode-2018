#include <cstdio>
#include <cstring>
#include <set>
#include <utility>
#include <vector>

struct Rect {
  int id;
  int x0, y0;
  int w, h;
};

int main(int argc, char** argv)
{
  std::set<std::pair<int,int>> claims;
  std::set<std::pair<int,int>> overlapping_claims;
  std::vector<Rect> rects;

  FILE* f = fopen(argv[1], "r");
  char line[256] = {};
  while (fgets(line, sizeof(line), f) != nullptr) {
    Rect r;
    sscanf(line, "#%d @ %d,%d: %dx%d", &r.id, &r.x0, &r.y0, &r.w, &r.h);
    rects.push_back(r);

    const int x1 = r.x0 + r.w;
    const int y1 = r.y0 + r.h;
    for (int y = r.y0; y < y1; y++) {
      for (int x = r.x0; x < x1; x++) {
        std::pair<int,int> pos = std::make_pair(y, x);
        if (!claims.insert(pos).second) {
          overlapping_claims.insert(pos);
        }
      }
    }
  }
  fclose(f);

  for (const Rect& r : rects) {
    const int x1 = r.x0 + r.w;
    const int y1 = r.y0 + r.h;
    bool has_overlap = false;
    for (int y = r.y0; y < y1 && !has_overlap; y++) {
      for (int x = r.x0; x < x1 && !has_overlap; x++) {
        std::pair<int,int> pos = std::make_pair(y, x);
        has_overlap = (overlapping_claims.find(pos) != overlapping_claims.end());
      }
    }

    if (!has_overlap) {
      printf("Result = %d\n", r.id);
      return 0;
    }
  }

  printf("Failed.\n");
  return 1;
}
