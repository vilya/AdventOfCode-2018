#include <cstdio>
#include <vector>

struct ivec2 {
  int x, y;

  ivec2& operator -= (ivec2 p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }
};

int distance(ivec2 a, ivec2 b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

ivec2 min(ivec2 a, ivec2 b) {
  return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) };
}

ivec2 max(ivec2 a, ivec2 b) {
  return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) };
}

int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  ivec2 p;
  std::vector<ivec2> points;
  while (fscanf(f, "%d, %d", &p.x, &p.y) == 2) {
    points.push_back(p);
  }
  fclose(f);

  ivec2 lo = points.front();
  ivec2 hi = points.front();
  for (ivec2 p : points) {
    lo = min(lo, p);
    hi = max(hi, p);
  }
  lo -= {  10000,  10000 };
  hi -= { -10000, -10000 };
  for (ivec2& p : points) {
    p -= lo;
  }
  hi -= lo;

  const int w = hi.x + 1;
  const int h = hi.y + 1;
  const int n = w * h;
  const int num_points = int(points.size());
  int area = 0;
  for (int i = 0; i < n; i++) {
    ivec2 p0 = { i % w, i / w };
    int total_dist = 0;
    for (int j = 0; j < num_points && total_dist < 10000; j++) {
      total_dist += distance(p0, points[j]);
    }
    if (total_dist < 10000) {
      ++area;
    }
  }

  printf("Area = %d\n", area);
}
