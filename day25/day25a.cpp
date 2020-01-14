#include <algorithm>
#include <cstdio>
#include <vector>


struct ivec4 {
  int x, y, z, w;

  int  operator [] (int i) const { return (&x)[i]; }
  int& operator [] (int i)       { return (&x)[i]; }

  static ivec4 from_scalar(int k) { return ivec4{k, k, k, k}; }
};


int distance(const ivec4& a, const ivec4& b) {
  return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z) + std::abs(a.w - b.w);
}

ivec4 min(const ivec4& a, const ivec4& b) { return ivec4{std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z), std::min(a.w, b.w)}; }
ivec4 max(const ivec4& a, const ivec4& b) { return ivec4{std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z), std::max(a.w, b.w)}; }


struct UnionFind {
  std::vector<int> parents;
  int n;

  UnionFind(int nn) : parents(nn, -1), n(nn) {
    for (int i = 0; i < n; i++)
      parents[i] = i;
  }

  int root(int i) const {
    while (parents[i] != i)
      i = parents[i];
    return i;
  }

  void merge(int i, int j) {
    i = root(i);
    j = root(j);
    if (i != j)
      parents[j] = i;
  }

//  void simplify() {
//    for (int i = 0; i < n; i++)
//      parents[i] = root(i);
//  }

  int count_groups() const {
    int num = 0;
    for (int i = 0; i < n; i++) {
      if (parents[i] == i)
        ++num;
    }
    return num;
  }
};


int main(int argc, char** argv)
{
  std::vector<ivec4> points;
  ivec4 lo = ivec4::from_scalar(std::numeric_limits<int>::max());
  ivec4 hi = ivec4::from_scalar(std::numeric_limits<int>::lowest());

  FILE* f = fopen(argv[1], "r");
  ivec4 tmp;
  while (fscanf(f, "%d,%d,%d,%d\n", &tmp.x, &tmp.y, &tmp.z, &tmp.w) == 4) {
    points.push_back(tmp);
    lo = min(lo, tmp);
    hi = max(hi, tmp);
  }
  fclose(f);

//  for (const ivec4& p : points) {
//    printf("x: %d, y: %d, z: %d, w: %d\n", p.x, p.y, p.z, p.w);
//  }
//  printf("%d points\n", int(points.size()));

  const int n = int(points.size());

  UnionFind ufind(n);
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      if (distance(points[i], points[j]) <= 3)
        ufind.merge(i, j);
    }
  }
//  ufind.simplify();

  int answer = ufind.count_groups();
  printf("Answer = %d\n", answer);
}
