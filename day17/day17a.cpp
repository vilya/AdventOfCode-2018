#include <cstdio>
#include <map>


struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
};

bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y <  b.y) : (a.x <  b.x); }

ivec2 operator + (ivec2 a, ivec2 b) { return ivec2{ a.x + b.x, a.y + b.y }; }

ivec2 min(ivec2 a, ivec2 b) { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
ivec2 max(ivec2 a, ivec2 b) { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }


static constexpr ivec2 kSpring{ 500, 0 };
static constexpr ivec2 kDown  {   0, 1 };
static constexpr ivec2 kLeft  {  -1, 0 };
static constexpr ivec2 kRight {   1, 0 };


struct Map {
  std::map<ivec2, char> grid;
  ivec2 lo = ivec2{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
  ivec2 hi = ivec2{ std::numeric_limits<int>::lowest(), std::numeric_limits<int>::lowest() };
  ivec2 start;

  void load(FILE* f) {
    while (true) {
      ivec2 a, b;
      if (fscanf(f, "x=%d, y=%d..%d\n", &a.x, &a.y, &b.y) == 3) {
        b.x = a.x;
      }
      else if (fscanf(f, "y=%d, x=%d..%d\n", &a.y, &a.x, &b.x) == 3) {
        b.y = a.y;
      }
      else {
        break;
      }

      for (int y = a.y; y <= b.y; y++) {
        for (int x = a.x; x <= b.x; x++) {
          set(ivec2{x, y}, '#');
        }
      }

      lo = min(lo, a);
      hi = max(hi, b);
    }
    start = ivec2{ kSpring.x, lo.y };
  }


  char get(ivec2 pos) const {
    auto it = grid.find(pos);
    return (it != grid.end()) ? it->second : ' ';
  }


  void set(ivec2 pos, char val) {
    grid[pos] = val;
  }


  bool is_supported(ivec2 pos) const {
    char below = get(pos + kDown);
    return below == '#' || below == '.';
  }


  void print() const {
    printf("[Sp.] %*s+\n", kSpring.x - lo.x, "");

    for (int y = lo.y; y <= hi.y; y++) {
      printf("[%4d] ", y);
      for (int x = lo.x - 1; x <= hi.x + 1; x++) {
        fputc(get(ivec2{ x, y }), stdout);
      }
      fputc('\n', stdout);
    }
  }


  // return value is whether the water is supported below and on both sides
  bool add_water_down(ivec2 pos) {
    if (pos.y > hi.y) {
      return false;
    }

    if (get(pos) != ' ') {
      return get(pos) == '.';
    }

    // If there's nothing already underneath us, add some water there.
    bool supported = is_supported(pos) || add_water_down(pos + kDown);
    if (supported) {
      bool supported_left  = (get(pos + kLeft)  == '#') || add_water_horizontal(pos + kLeft, kLeft);
      bool supported_right = (get(pos + kRight) == '#') || add_water_horizontal(pos + kRight, kRight);
      supported = supported_left && supported_right;
    }

    if (supported) {
      set(pos, '.');
      ivec2 horiz_pos = pos + kLeft;
      while (get(horiz_pos) == '~') {
        set(horiz_pos, '.');
        horiz_pos += kLeft;
      }
      horiz_pos = pos + kRight;
      while (get(horiz_pos) == '~') {
        set(horiz_pos, '.');
        horiz_pos += kRight;
      }
    }
    else {
      set(pos, '~');
    }

    return supported;
  }


  // return value is whether the water is supported below and in the direction of travel.
  bool add_water_horizontal(ivec2 pos, ivec2 horiz_step) {
    bool supported = is_supported(pos) || add_water_down(pos + kDown);
    if (supported)
      supported = (get(pos + horiz_step) == '#') || add_water_horizontal(pos + horiz_step, horiz_step);
    set(pos, '~');
    return supported;
  }


  int wet_cells() const {
    int num = 0;
    for (const auto& it : grid) {
      if (it.second == '.' || it.second == '~')
        ++num;
    }
    return num;
  }
};


int main(int argc, char** argv)
{
  Map map;

  FILE* f = fopen(argv[1], "r");
  map.load(f);
  fclose(f);

  map.add_water_down(map.start);
  map.print();

  printf("Water reaches %d cells\n", map.wet_cells());
}
