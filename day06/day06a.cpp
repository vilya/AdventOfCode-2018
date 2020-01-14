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
  for (ivec2& p : points) {
    p -= lo;
  }
  hi -= lo;

  const int w = hi.x + 1;
  const int h = hi.y + 1;
  const int n = w * h;
  const int num_points = int(points.size());
  std::vector<int> nearest(size_t(n), std::numeric_limits<int>::max());
  for (int i = 0; i < n; i++) {
    ivec2 p0 = { i % w, i / w };
    int best_j = -1;
    int best_dist = std::numeric_limits<int>::max();
    for (int j = 0; j < num_points; j++) {
      int dist = distance(p0, points[j]);
      if (dist < best_dist) {
        best_j = j;
        best_dist = dist;
      }
      else if (dist == best_dist) {
        best_j = -1;
      }
    }
    nearest[i] = best_j;
  }

  std::vector<bool> infinite_area(points.size(), false);
  for (int y = 0; y < h; y++) {
    int i0 = y * w;
    int i1 = i0 + (w - 1);
    if (nearest[i0] >= 0)
      infinite_area[nearest[i0]] = true;
    if (nearest[i1] >= 0)
      infinite_area[nearest[i1]] = true;
  }
  for (int x = 0; x < w; x++) {
    int i0 = x;
    int i1 = i0 + h * (w - 1);
    if (nearest[i0] >= 0)
      infinite_area[nearest[i0]] = true;
    if (nearest[i1] >= 0)
      infinite_area[nearest[i1]] = true;
  }

  std::vector<int> area(points.size(), 0);
  for (int i = 0; i < n; i++) {
    if (nearest[i] >= 0)
      area[nearest[i]]++;
  }

  int largest_area = 0;
  for (int i = 0; i < num_points; i++) {
    if (!infinite_area[i] && area[i] > largest_area) {
      largest_area = area[i];
    }
  }

  printf("Largest area = %d\n", largest_area);
}
