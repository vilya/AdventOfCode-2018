#include <cstdio>
#include <map>
#include <queue>
#include <set>
#include <utility>


struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
};

bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator != (ivec2 a, ivec2 b) { return a.y != b.y && a.x != b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y <  b.y) : (a.x <  b.x); }
bool operator >  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y >  b.y) : (a.x >  b.x); }

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
static constexpr int kWidth = kTarget.x + 1;
static constexpr int kHeight = kTarget.y + 1;

static constexpr int kRocky   = 0;
static constexpr int kWet     = 1;
static constexpr int kNarrow  = 2;

static constexpr int kNeither = 0;
static constexpr int kTorch = 1;
static constexpr int kClimbingGear = 2;


constexpr int erosion_level(int geologic_index) { return (geologic_index + kDepth) % 20183; }

// terrain == erosion_level % 3
constexpr bool can_enter_with(int terrain, int tool) { return tool != terrain; }


struct Work {
  int time, tool;
  ivec2 pos;

  bool operator == (const Work& w) const { return time == w.time && tool == w.tool && pos == w.pos; }
  bool operator <  (const Work& w) const {
    // Actually returns > for the benefit of the priority queue.
    if (time != w.time)
      return time > w.time;
    if (tool != w.tool)
      return tool > w.tool;
    return pos > w.pos;
  }
};


struct Map {
  int g_cache[kHeight][kWidth];
  std::map<ivec2, int> g_cache_extra;

  Map() {
    g_cache[0][0] = 0;
    for (int x = 1; x < kWidth; x++)
      g_cache[0][x] = x * 16807;
    for (int y = 1; y < kHeight; y++)
      g_cache[y][0] = y * 48271;
    g_cache[kTarget.y][kTarget.x] = 0;

    ivec2 pos;
    for (pos.y = 1; pos.y < kHeight; pos.y++) {
      for (pos.x = 1; pos.x < kWidth; pos.x++) {
        if (pos == kTarget) {
          continue;
        }
        int left = erosion_level(g_cache[pos.y][pos.x - 1]);
        int up   = erosion_level(g_cache[pos.y - 1][pos.x]);
        g_cache[pos.y][pos.x] = left * up;
      }
    }
  }


  int get(ivec2 cell) {
    if (cell.x < kWidth && cell.y < kHeight)
      return g_cache[cell.y][cell.x];
    else if (cell.y == 0)
      return cell.x * 16807;
    else if (cell.x == 0)
      return cell.y * 48271;

    auto it = g_cache_extra.find(cell);
    if (it != g_cache_extra.end())
      return it->second;

    int g_val = erosion_level(get(cell + kWest)) * erosion_level(get(cell + kNorth));
    g_cache_extra[cell] = g_val;
    return g_val;
  }


  int terrain(ivec2 cell) {
    return erosion_level(get(cell)) % 3;
  }


  bool can_enter(ivec2 cell, int tool) {
    return terrain(cell) != tool;
  }


  void print() {
    ivec2 hi = kTarget;
    for (auto it : g_cache_extra) {
      hi = max(hi, it.first);
    }
    const char kErosionLevel[3] = { '.', '=', '|' };
    ivec2 pos;
    for (pos.y = 0; pos.y <= hi.y; pos.y++) {
      for (pos.x = 0; pos.x < hi.x; pos.x++) {
        if (pos == kStart)
          fputc('M', stdout);
        else if (pos == kTarget)
          fputc('T', stdout);
        else
          fputc(kErosionLevel[erosion_level(get(pos)) % 3], stdout);
      }
      fputc('\n', stdout);
    }
    fputc('\n', stdout);
    fflush(stdout);
  }


  int quickest_rescue() {
    // BFS where the state is the current position *and* which tool is equipped.
    std::priority_queue<Work, std::deque<Work>> q;
    std::set<std::pair<int, ivec2>> visited;

    q.push(Work{0, kTorch, kStart});
    while (!q.empty()) {
      Work work = q.top();
      q.pop();
      if (!visited.insert(std::make_pair(work.tool, work.pos)).second)
        continue;

      if (work.pos == kTarget) {
        if (work.tool == kTorch)
          return work.time;
        q.push(Work{work.time + 7, kTorch, kTarget});
        continue;
      }

      if (work.pos.y > 0 && can_enter(work.pos + kNorth, work.tool))
        q.push(Work{work.time + 1, work.tool, work.pos + kNorth});
      if (work.pos.x > 0 && can_enter(work.pos + kWest, work.tool))
        q.push(Work{work.time + 1, work.tool, work.pos + kWest});
      if (can_enter(work.pos + kEast, work.tool))
        q.push(Work{work.time + 1, work.tool, work.pos + kEast});
      if (can_enter(work.pos + kSouth, work.tool))
        q.push(Work{work.time + 1, work.tool, work.pos + kSouth});
      if (can_enter(work.pos, (work.tool + 1) % 3))
        q.push(Work{work.time + 7, (work.tool + 1) % 3, work.pos});
      if (can_enter(work.pos, (work.tool + 2) % 3))
        q.push(Work{work.time + 7, (work.tool + 2) % 3, work.pos});
    }

    return -1;
  }
};


int main()
{
  Map map;
//  map.print();
  printf("Quickest rescue is %d mins\n", map.quickest_rescue());
}
