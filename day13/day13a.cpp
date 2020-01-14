#include <cstdio>
#include <string>
#include <vector>

struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
  ivec2& operator -= (ivec2 v) { x -= v.x; y -= v.y; return *this; }
};
bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator != (ivec2 a, ivec2 b) { return a.y != b.y || a.x != b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y < b.y) : (a.x < b.x); }


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


struct Cart {
  ivec2 pos;
  int dir;
  int next_turn = -1;

  void turn() {
    dir = (dir + 4 + next_turn) & 0x3;
    next_turn = ((next_turn + 2) % 3) - 1;
  }

  void move() {
    pos += kDir[dir];
  }
};


bool order_by_pos(const Cart& a, const Cart& b)
{
  if (a.pos != b.pos)
    return (a.pos < b.pos);
  return (a.dir != b.dir) ? (a.dir < b.dir) : (a.next_turn < b.next_turn);
}


struct Map {
  std::vector<std::string> grid;
  std::vector<Cart> carts;

  // Returns index of the first cart which crashed, or -1 if no crashes yet.
  int tick() {
    std::sort(carts.begin(), carts.end(), order_by_pos);
    for (int i = 0; i < carts.size(); i++) {
      carts[i].move();
      switch (grid[carts[i].pos.y][carts[i].pos.x]) {
      case '/':  carts[i].dir ^= 1; break;
      case '\\': carts[i].dir ^= 3; break;
      case '+':  carts[i].turn(); break;
      default: break;
      }
      for (int j = 0; j < carts.size(); j++) {
        if (j != i && carts[j].pos == carts[i].pos) {
          return i;
        }
      }
    }
    return -1;
  }
};


int main(int argc, char** argv)
{
  Map map;

  FILE* f = fopen(argv[1], "r");
  char line[256];
  while (fgets(line, sizeof(line), f) != nullptr) {
    map.grid.push_back(line);
  }
  fclose(f);

  ivec2 pos;
  for (pos.y = 0; pos.y < map.grid.size(); pos.y++) {
    for (pos.x = 0; pos.x < map.grid[pos.y].size(); pos.x++) {
      switch (map.grid[pos.y][pos.x]) {
      case '^':
        map.carts.push_back(Cart{ pos, kNorth, -1 });
        map.grid[pos.y][pos.x] = '|';
        break;
      case '>':
        map.carts.push_back(Cart{ pos, kEast, -1 }); break;
        map.grid[pos.y][pos.x] = '-';
        break;
      case 'v':
        map.carts.push_back(Cart{ pos, kSouth, -1 }); break;
        map.grid[pos.y][pos.x] = '|';
        break;
      case '<':
        map.carts.push_back(Cart{ pos, kWest, -1 }); break;
        map.grid[pos.y][pos.x] = '-';
        break;
      default:
        break;
      }
    }
  }

  int crashed = map.tick();
  while (crashed == -1) {
    crashed = map.tick();
  }
  printf("%d,%d\n", map.carts[crashed].pos.x, map.carts[crashed].pos.y);
}
