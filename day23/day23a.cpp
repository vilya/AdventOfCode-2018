#include <cstdio>
#include <vector>

struct ivec3 {
  int x, y, z;
};
inline static ivec3 operator - (ivec3 a, ivec3 b) { return ivec3{ a.x - b.x, a.y - b.y, a.z - b.z }; }
inline static ivec3 abs(ivec3 a) { return ivec3{ std::abs(a.x), std::abs(a.y), std::abs(a.z) }; }
inline static int sum(ivec3 a) { return a.x + a.y + a.z; }
inline static int distance(ivec3 a, ivec3 b) { return sum(abs(a - b)); }


int main(int argc, char** argv)
{
  std::vector<ivec3> pos;
  std::vector<int> radius;

  pos.reserve(1000);
  radius.reserve(1000);

  FILE* f = fopen(argv[1], "r");
  ivec3 tmp_pos;
  int tmp_r;
  while (fscanf(f, "pos=<%d,%d,%d>, r=%d\n", &tmp_pos.x, &tmp_pos.y, &tmp_pos.z, &tmp_r) == 4) {
    pos.push_back(tmp_pos);
    radius.push_back(tmp_r);
  }
  fclose(f);

  size_t max_radius_i = 0;
  for (size_t i = 1, endi = radius.size(); i < endi; i++) {
    if (radius[i] > radius[max_radius_i])
      max_radius_i = i;
  }

  ivec3 max_pos = pos[max_radius_i];
  int max_r = radius[max_radius_i];
  printf("The strongest nanobot is %d at {%d,%d,%d} with radius=%d\n",
         int(max_radius_i), max_pos.x, max_pos.y, max_pos.z, max_r);

  int within_range = 0;
  for (ivec3 p : pos) {
    if (distance(p, max_pos) <= max_r)
      ++within_range;
  }

  printf("There are %d nanobots within range of %d\n", within_range, int(max_radius_i));
}
