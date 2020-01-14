#include <cstdio>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>


struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
};

bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y <  b.y) : (a.x <  b.x); }

ivec2 operator + (ivec2 a, ivec2 b) { return ivec2{ a.x + b.x, a.y + b.y }; }
ivec2 operator - (ivec2 a, ivec2 b) { return ivec2{ a.x - b.x, a.y - b.y }; }

ivec2 min(ivec2 a, ivec2 b) { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
ivec2 max(ivec2 a, ivec2 b) { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }


static constexpr ivec2 kDir[4] = {
  ivec2{  0, -1 }, // north
  ivec2{  1,  0 }, // east
  ivec2{  0,  1 }, // south
  ivec2{ -1,  0 }, // west
};
static constexpr int kNorth = 0;
static constexpr int kEast  = 1;
static constexpr int kSouth = 2;
static constexpr int kWest  = 3;

int opposite_dir(int dir){ return dir ^ 2; }

static constexpr ivec2 kStart = ivec2{ 0, 0 };


struct Room {
  ivec2 pos;
  bool has_door[4];

  Room() : pos(kStart), has_door{ false, false, false, false } {}
  Room(ivec2 p) : pos(p), has_door{ false, false, false, false } {}
  Room(ivec2 p, int dir) : pos(p), has_door{ false, false, false, false } {
    has_door[dir] = true;
  }
};


struct Map {
  std::map<ivec2, Room> grid;
  ivec2 lo = kStart;
  ivec2 hi = kStart;


  void add_door(ivec2 pos, int dir) {
    auto it = grid.find(pos);
    if (it == grid.end())
      grid[pos] = Room(pos, dir);
    else
      it->second.has_door[dir] = true;
  }

  ivec2 move_in_dir(ivec2 pos, int dir) {
    add_door(pos, dir);
    pos += kDir[dir];
    add_door(pos, opposite_dir(dir));
    lo = min(lo, pos);
    hi = max(hi, pos);

//    print();
//    printf("\n");
//    fflush(stdout);

    return pos;
  }


  const char* skip_to_closing_paren(const char* re) const {
    int depth = 0;
    while (true) {
      switch (*re) {
      case '(': ++depth; break;
      case ')': if (depth == 0) return re; --depth; break;
      default: break;
      }
      ++re;
    }
    return re;
  }


  const char* skip_to_next_choice(const char* re) const {
    int depth = 0;
    while (true) {
      switch (*re) {
      case '(': ++depth; break;
      case ')': if (depth == 0) return re; --depth; break;
      case '|': if (depth == 0) return re; break;
      default: break;
      }
      ++re;
    }
    return re;
  }


  void load(FILE* f) {
    const int kMaxRegexLen = 16 * 1024;
    char* regex = new char[kMaxRegexLen];
    fgets(regex, kMaxRegexLen, f);

    grid.clear();
    grid[kStart] = Room(kStart);

    ivec2 pos = kStart;
    std::vector<ivec2> bookmarks;
    const char* re = regex + 1;
    while (true) {
      switch (*re) {
      case '(': bookmarks.push_back(pos); re++; break;
      case ')': bookmarks.pop_back(); re++; break;
      case '|': pos = bookmarks.back(); re++; break;
      case '$': return;
      case 'N': pos = move_in_dir(pos, kNorth); re++; break;
      case 'E': pos = move_in_dir(pos, kEast);  re++; break;
      case 'S': pos = move_in_dir(pos, kSouth); re++; break;
      case 'W': pos = move_in_dir(pos, kWest);  re++; break;
      default: fprintf(stderr, "Invalid char in regex: '%c'\n", *re); return;
      }
    }

    delete[] regex;
  }


  void print() {
    size_t w = size_t(hi.x - lo.x + 1) * 2 + 1;
    size_t h = size_t(hi.y - lo.y + 1) * 2 + 1;
    std::vector<std::string> output(h, std::string(w, '#'));

    for (auto it : grid) {
      ivec2 pos = it.first - lo;
      const Room& room = it.second;
      ivec2 center = ivec2{ pos.x * 2 + 1, pos.y * 2 + 1 };
      output[center.y][center.x] = '.';
      for (int dir = 0; dir < 4; dir++) {
        if (room.has_door[dir]) {
          ivec2 door = center + kDir[dir];
          output[door.y][door.x] = (dir == kNorth || dir == kSouth) ? '-' : '|';
        }
      }
    }

    ivec2 start = kStart - lo;
    start.x = start.x * 2 + 1;
    start.y = start.y * 2 + 1;
    output[start.y][start.x] = 'X';

    for (const std::string& row : output) {
      printf("%s\n", row.c_str());
    }
  }


  int solve() {
    std::set<ivec2> visited;

    typedef std::pair<int, ivec2> Work;
    std::queue<Work> q;
    q.push(std::make_pair(0, kStart));

    while (!q.empty()) {
      Work work = q.front();
      q.pop();

      int dist = work.first;
      if (dist >= 1000)
        continue;

      ivec2 pos = work.second;
      if (!visited.insert(pos).second)
        continue;

      ++dist;
      const Room& room = grid.at(pos);
      for (int dir = 0; dir < 4; dir++) {
        if (room.has_door[dir]) {
          q.push(std::make_pair(dist, pos + kDir[dir]));
        }
      }
    }

    int rooms = 0;
    for (auto it : grid) {
      if (visited.count(it.first) == 0) {
        ++rooms;
      }
    }

    return rooms;
  }
};


int main(int argc, char** argv)
{
  Map map;

  FILE* f = fopen(argv[1], "r");
  map.load(f);
  fclose(f);

  printf("%d rooms which require 1000 or more doors\n", map.solve());
}
