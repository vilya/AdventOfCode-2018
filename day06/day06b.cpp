#include <cstdio>
#include <vector>

struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 p) {
    x += p.x;
    y += p.y;
    return *this;
  }

  ivec2& operator -= (ivec2 p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }  
};

inline int clamp(int i, int lo, int hi) {
  return (i <= lo) ? lo : (i >= hi) ? hi : i;
}

int distance(ivec2 a, ivec2 b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

int distance_from_bounds(ivec2 lo, ivec2 hi, ivec2 p) {
  return distance(ivec2{clamp(p.x, lo.x, hi.x), clamp(p.y, lo.y, hi.y)}, p);
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

  const int num_points = int(points.size());

  ivec2 lo = points.front();
  ivec2 hi = points.front();
  for (ivec2 p : points) {
    lo = min(lo, p);
    hi = max(hi, p);
  }
  ivec2 inner_lo = lo;
  ivec2 inner_hi = hi;
  int expand_by = 10000 / num_points;
  lo -= ivec2{ expand_by, expand_by };
  hi += ivec2{ expand_by, expand_by };
  for (ivec2& p : points) {
    p -= lo;
  }
  hi -= lo;

  const int w = hi.x + 1;
  const int h = hi.y + 1;
  const int n = w * h;

  int area = 0;
  for (int i = 0; i < n; i++) {
    ivec2 p0 = { i % w, i / w };
    if (distance_from_bounds(inner_lo, inner_hi, p0) > expand_by) {
      continue;
    }
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
