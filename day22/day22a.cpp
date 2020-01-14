#include <cstdio>
#include <map>

struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
};

bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y <  b.y) : (a.x <  b.x); }

constexpr ivec2 operator + (ivec2 a, ivec2 b) { return ivec2{ a.x + b.x, a.y + b.y }; }
constexpr ivec2 operator - (ivec2 a, ivec2 b) { return ivec2{ a.x - b.x, a.y - b.y }; }

ivec2 min(ivec2 a, ivec2 b) { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
ivec2 max(ivec2 a, ivec2 b) { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }


static constexpr ivec2 kNorth{0, -1};
static constexpr ivec2 kEast{1, 0};
static constexpr ivec2 kSouth{0, 1};
static constexpr ivec2 kWest{-1, 0};


static constexpr ivec2 kStart = ivec2{ 0, 0 };
static constexpr int kDepth = 7740;
static constexpr ivec2 kTarget{ 12, 763 };
//static constexpr int kDepth = 510;
//static constexpr ivec2 kTarget{ 10, 10 };

static constexpr int kRocky   = 0;
static constexpr int kWet     = 1;
static constexpr int kNarrow  = 2;

constexpr int erosion_level(int geologic_index) { return (geologic_index + kDepth) % 20183; }

struct Map {
  static constexpr int kWidth = kTarget.x - kStart.x + 1;
  static constexpr int kHeight = kTarget.y - kStart.y + 1;
  int g_cache[kHeight][kWidth];
  ivec2 lo = kStart;
  ivec2 hi = kTarget;

  Map() {
    g_cache[0][0] = 0;
    for (int x = 1; x < kWidth; x++)
      g_cache[0][x] = x * 16807;
    for (int y = 1; y < kHeight; y++)
      g_cache[y][0] = y * 48271;
    g_cache[kStart.y - lo.y][kStart.x - lo.x] = 0;
    g_cache[kTarget.y - lo.y][kTarget.x - lo.x] = 0;

    ivec2 pos;
    for (pos.y = lo.y + 1; pos.y <= hi.y; pos.y++) {
      for (pos.x = lo.x + 1; pos.x <= hi.x; pos.x++) {
        if (pos == kStart || pos == kTarget) {
          continue;
        }
        ivec2 p = pos - lo;
        int left = erosion_level(g_cache[p.y][p.x - 1]);
        int up   = erosion_level(g_cache[p.y - 1][p.x]);
        g_cache[p.y][p.x] = left * up;
      }
    }
  }


  void print() {
    const char kErosionLevel[3] = { '.', '=', '|' };
    ivec2 pos;
    for (pos.y = lo.y; pos.y <= hi.y; pos.y++) {
      for (pos.x = lo.x; pos.x <= hi.x; pos.x++) {
        if (pos == kStart) {
          fputc('M', stdout);
        }
        else if (pos == kTarget) {
          fputc('T', stdout);
        }
        else {
          ivec2 p = pos - lo;
          fputc(kErosionLevel[erosion_level(g_cache[p.y][p.x]) % 3], stdout);
        }
      }
      fputc('\n', stdout);
    }
    fputc('\n', stdout);
    fflush(stdout);
  }


  int risk_level() const {
    int sum = 0;
    for (int y = 0; y < kHeight; y++) {
      for (int x = 0; x < kWidth; x++) {
        sum += erosion_level(g_cache[y][x]) % 3;
      }
    }
    return sum;
  }
};


int main()
{
  Map map;
  map.print();
  printf("Risk level = %d\n", map.risk_level());
}
