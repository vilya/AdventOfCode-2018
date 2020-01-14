#include <algorithm>
#include <cstdio>
#include <queue>
#include <vector>


struct ivec3 {
  int x, y, z;

  int& operator [] (int axis)       { return (&x)[axis]; }
  int  operator [] (int axis) const { return (&x)[axis]; }
};

inline static ivec3 operator + (ivec3 a, ivec3 b) { return ivec3{ a.x + b.x, a.y + b.y, a.z + b.z }; }
inline static ivec3 operator - (ivec3 a, ivec3 b) { return ivec3{ a.x - b.x, a.y - b.y, a.z - b.z }; }

inline static ivec3 operator + (ivec3 a, int k) { return ivec3{ a.x + k, a.y + k, a.z + k }; }
inline static ivec3 operator - (ivec3 a, int k) { return ivec3{ a.x - k, a.y - k, a.z - k }; }
inline static ivec3 operator / (ivec3 a, int k) { return ivec3{ a.x / k, a.y / k, a.z / k }; }

inline static ivec3 abs(ivec3 a) { return ivec3{ std::abs(a.x), std::abs(a.y), std::abs(a.z) }; }
inline static int sum(ivec3 a) { return a.x + a.y + a.z; }
inline static int length(ivec3 a) { return sum(abs(a)); }
inline static int distance(ivec3 a, ivec3 b) { return length(a - b); }

inline static ivec3 min(ivec3 a, ivec3 b) { return ivec3{ std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) }; }
inline static ivec3 max(ivec3 a, ivec3 b) { return ivec3{ std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) }; }

inline static bool operator == (ivec3 a, ivec3 b)         { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline static bool operator != (ivec3 a, ivec3 b)         { return a.x != b.x || a.y != b.y || a.z != b.z; }
inline static bool operator <  (ivec3 a, ivec3 b)         { return (a.z != b.z) ? (a.z < b.z) : (a.y != b.y) ? (a.y < b.y) : (a.x < b.x); }

inline int distance_from_range_to_origin(ivec3 lo, ivec3 hi) {
  ivec3 nearest{
      (lo.x > 0) ? lo.x : (hi.x < 0) ? hi.x : 0,
      (lo.y > 0) ? lo.y : (hi.y < 0) ? hi.y : 0,
      (lo.z > 0) ? lo.z : (hi.z < 0) ? hi.z : 0,
  };
  return length(nearest);
}


struct Nanobot {
  ivec3 pos;
  int radius;

  bool overlaps_range(ivec3 lo, ivec3 hi) const {
    ivec3 nearest{
      (pos.x <= lo.x) ? lo.x : (pos.x >= hi.x) ? (hi.x - 1) : pos.x,
      (pos.y <= lo.y) ? lo.y : (pos.y >= hi.y) ? (hi.y - 1) : pos.y,
      (pos.z <= lo.z) ? lo.z : (pos.z >= hi.z) ? (hi.z - 1) : pos.z,
    };
    return distance(nearest, pos) <= radius;
  }

  bool contains_range(ivec3 lo, ivec3 hi) const {
    lo = lo - pos;
    hi = hi - pos - 1;
    if (length(ivec3{lo.x, lo.y, lo.z}) > radius) return false;
    if (length(ivec3{hi.x, lo.y, lo.z}) > radius) return false;
    if (length(ivec3{lo.x, hi.y, lo.z}) > radius) return false;
    if (length(ivec3{hi.x, hi.y, lo.z}) > radius) return false;
    if (length(ivec3{lo.x, lo.y, hi.z}) > radius) return false;
    if (length(ivec3{hi.x, lo.y, hi.z}) > radius) return false;
    if (length(ivec3{lo.x, hi.y, hi.z}) > radius) return false;
    if (length(ivec3{hi.x, hi.y, hi.z}) > radius) return false;
    return true;
  }
};


struct Node {
  ivec3 lo;
  int size;
  int overlapping;
  int contained_in;
  int dist_from_origin;


  bool operator < (const Node& ch) const {
    if (overlapping != ch.overlapping)
      return overlapping > ch.overlapping;
    if (contained_in != ch.contained_in)
      return contained_in > ch.contained_in;
    if (dist_from_origin != ch.dist_from_origin)
      return dist_from_origin < ch.dist_from_origin;
    return lo < ch.lo;
  }

  void count(const std::vector<Nanobot>& nanobots) {
    overlapping = 0;
    contained_in = 0;
    ivec3 hi = lo + size;
    const int n = int(nanobots.size());
    for (int i = 0; i < n; i++) {
      if (nanobots[i].contains_range(lo, hi))
        ++contained_in;
      else if (nanobots[i].overlaps_range(lo, hi))
        ++overlapping;
    }
    overlapping += contained_in;
  }
};


void solve(const std::vector<Nanobot>& nanobots, const Node& node, int& best_num, int& best_dist)
{
  if (node.overlapping < best_num)
    return;

  if (node.overlapping == best_num && node.dist_from_origin >= best_dist)
    return;

  if (node.contained_in == node.overlapping) {
    best_num = node.contained_in;
    best_dist = node.dist_from_origin;
    return;
  }

  assert(node.size > 1);

  int child_size = node.size / 2;
  ivec3 node_mid = node.lo + child_size;

  Node children[8] = {
    { ivec3{ node.lo.x,  node.lo.y,  node.lo.z  }, child_size, 0, 0, -1 },
    { ivec3{ node_mid.x, node.lo.y,  node.lo.z  }, child_size, 0, 0, -1 },
    { ivec3{ node.lo.x,  node_mid.y, node.lo.z  }, child_size, 0, 0, -1 },
    { ivec3{ node_mid.x, node_mid.y, node.lo.z  }, child_size, 0, 0, -1 },
    { ivec3{ node.lo.x,  node.lo.y,  node_mid.z }, child_size, 0, 0, -1 },
    { ivec3{ node_mid.x, node.lo.y,  node_mid.z }, child_size, 0, 0, -1 },
    { ivec3{ node.lo.x,  node_mid.y, node_mid.z }, child_size, 0, 0, -1 },
    { ivec3{ node_mid.x, node_mid.y, node_mid.z }, child_size, 0, 0, -1 },
  };
  for (int ch = 0; ch < 8; ch++) {
    children[ch].count(nanobots);
    children[ch].dist_from_origin = distance_from_range_to_origin(children[ch].lo, children[ch].lo + child_size);
  }

  std::sort(children, children + 8);

  for (int ch = 0; ch < 8; ch++) {
    solve(nanobots, children[ch], best_num, best_dist);
  }
}


int next_power_of_2(int n)
{
  if (n <= 0)
    return 0;
  int i = 1;
  while (i < n)
    i <<= 1;
  return i;
}


int main(int argc, char** argv)
{
  std::vector<Nanobot> nanobots;
  nanobots.reserve(1000);

  ivec3 lo{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
  ivec3 hi{ std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

  FILE* f = fopen(argv[1], "r");
  Nanobot tmp;
  while (fscanf(f, "pos=<%d,%d,%d>, r=%d\n", &tmp.pos.x, &tmp.pos.y, &tmp.pos.z, &tmp.radius) == 4) {
    nanobots.push_back(tmp);
    lo = min(lo, tmp.pos);
    hi = max(hi, tmp.pos);
  }
  fclose(f);

  ivec3 dims = hi - lo + 1;
  int size = next_power_of_2(std::max(dims.x, dims.y));

  int best_num = 1, best_dist = std::numeric_limits<int>::max();
  solve(nanobots, Node{lo, size, int(nanobots.size()), 0, distance_from_range_to_origin(lo, hi)}, best_num, best_dist);

  printf("Answer = %d, with %d bots in range\n", best_dist, best_num);
}
