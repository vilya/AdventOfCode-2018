#include <cstdio>
#include <queue>
#include <vector>

struct ivec2 {
  int16_t x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
};

bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator != (ivec2 a, ivec2 b) { return a.y != b.y && a.x != b.x; }

constexpr ivec2 operator + (ivec2 a, ivec2 b) { return ivec2{ int16_t(a.x + b.x), int16_t(a.y + b.y) }; }
constexpr ivec2 operator - (ivec2 a, ivec2 b) { return ivec2{ int16_t(a.x - b.x), int16_t(a.y - b.y) }; }

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

static constexpr int kWidth = kTarget.x * 2;
static constexpr int kHeight = kTarget.y * 2;

static constexpr int kRocky   = 0;
static constexpr int kWet     = 1;
static constexpr int kNarrow  = 2;

static constexpr int kNeither = 0;
static constexpr int kTorch = 1;
static constexpr int kClimbingGear = 2;

static constexpr int kToolPriority[3] = { 2, 0, 1 };


struct Work {
  uint16_t time, tool, dist;
  ivec2 pos;

  bool operator >  (const Work& w) const {
    if (time != w.time)
      return time > w.time;
    if (dist != w.dist)
      return dist > w.dist;
    return kToolPriority[tool] > kToolPriority[w.tool];
  }

  static inline Work from(int time, int tool, ivec2 pos) {
    return Work{ uint16_t(time), uint16_t(tool), uint16_t(std::abs(pos.x - kTarget.x) + std::abs(pos.y - kTarget.y)), pos};
  }
};


struct Map {
  uint8_t cache[kHeight][kWidth];

  Map() {
    int erosion[2][kWidth];
    for (int x = 0; x < kWidth; x++) {
      erosion[0][x] = (x * 16807 + kDepth) % 20183;
      cache[0][x] = uint8_t((erosion[0][x] % 3) << 6);
    }
    for (int y = 1; y < kHeight; y++) {
      int curr = y & 1;
      int prev = curr ^ 1;
      erosion[curr][0] = (y * 48271 + kDepth) % 20183;
      cache[y][0] = uint8_t((erosion[curr][0] % 3) << 6);
      for (int x = 1; x < kWidth; x++) {
        if (x == kTarget.x && y == kTarget.y)
          erosion[curr][x] = kDepth;
        else
          erosion[curr][x] = (erosion[curr][x-1] * erosion[prev][x] + kDepth) % 20183;
        cache[y][x] = uint8_t((erosion[curr][x] % 3) << 6);
      }
    }
  }


  inline bool visited(ivec2 cell, int tool) const {
    return cache[cell.y][cell.x] & (1u << tool);
  }


  inline void set_visited(ivec2 cell, int tool) {
    cache[cell.y][cell.x] |= (1u << tool);
  }


  inline bool can_enter(ivec2 cell, int tool) const {
    int val = cache[cell.y][cell.x];
    return ((val >> 6) != tool) && ((val & (1 << tool)) == 0);
  }


  void print() {
    ivec2 hi = kTarget;
    const char kErosionLevel[3] = { '.', '=', '|' };
    ivec2 pos;
    for (pos.y = 0; pos.y <= hi.y + 5; pos.y++) {
      for (pos.x = 0; pos.x <= hi.x + 5; pos.x++) {
        if (pos == kStart)
          fputc('X', stdout);
        else if (pos == kTarget)
          fputc('T', stdout);
        else
          fputc(kErosionLevel[cache[pos.y][pos.x] >> 6], stdout);
      }
      fputc('\n', stdout);
    }
    fputc('\n', stdout);
    fflush(stdout);
  }


  int quickest_rescue() {
    // BFS where the state is the current position *and* which tool is equipped.
    std::priority_queue<Work, std::vector<Work>, std::greater<Work>> q;

    q.push(Work::from(0, kTorch, kStart));
    while (!q.empty()) {
      Work work = q.top();
      q.pop();

      if (visited(work.pos, work.tool))
        continue;
      set_visited(work.pos, work.tool);

      if (work.pos == kTarget) {
        if (work.tool == kTorch)
          return work.time;
        q.push(Work::from(work.time + 7, kTorch, kTarget));
        continue;
      }

      if (work.pos.y > 0 && can_enter(work.pos + kNorth, work.tool))
        q.push(Work::from(work.time + 1, work.tool, work.pos + kNorth));
      if (work.pos.x > 0 && can_enter(work.pos + kWest, work.tool))
        q.push(Work::from(work.time + 1, work.tool, work.pos + kWest));
      if (work.pos.x < kTarget.x * 2 && can_enter(work.pos + kEast, work.tool))
        q.push(Work::from(work.time + 1, work.tool, work.pos + kEast));
      if (work.pos.y < kTarget.y * 2 && can_enter(work.pos + kSouth, work.tool))
        q.push(Work::from(work.time + 1, work.tool, work.pos + kSouth));

      int alt_tool = 3 - (cache[work.pos.y][work.pos.x] >> 6) - work.tool;
      if (!visited(work.pos, alt_tool))
        q.push(Work::from(work.time + 7, alt_tool, work.pos));
    }

    return -1;
  }
};


int main()
{
  Map* map = new Map();
//  map->print();
  printf("Quickest rescue is %d mins\n", map->quickest_rescue());
}
