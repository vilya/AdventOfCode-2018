#include <cstdio>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) { x += v.x; y += v.y; return *this; }
  ivec2& operator -= (ivec2 v) { x -= v.x; y -= v.y; return *this; }
};

bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator != (ivec2 a, ivec2 b) { return a.y != b.y || a.x != b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y <  b.y) : (a.x <  b.x); }
bool operator <= (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y <= b.y) : (a.x <= b.x); }
bool operator >  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y >  b.y) : (a.x >  b.x); }
bool operator >= (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y >= b.y) : (a.x >= b.x); }

ivec2 operator + (ivec2 a, ivec2 b) { return ivec2{ a.x + b.x, a.y + b.y }; }


static constexpr ivec2 kDir[4] = {
  ivec2{  0, -1 }, // north
  ivec2{ -1,  0 }, // west
  ivec2{  1,  0 }, // east
  ivec2{  0,  1 }, // south
};
static constexpr int kNorth = 0;
static constexpr int kWest  = 1;
static constexpr int kEast  = 2;
static constexpr int kSouth = 3;


struct Unit {
  ivec2 pos;
  char type;
  int hit_points;
};


struct Map {
  std::vector<std::string> grid;
  std::vector<Unit> units;
  std::map<ivec2, size_t> units_by_pos;
  int num_elves = 0;
  int num_goblins = 0;
  int elf_power = 3;
  int goblin_power = 3;

  void load(FILE* f) {
    grid.clear();
    units.clear();
    // Fill the grid
    char line[256];
    while (fgets(line, sizeof(line), f) != nullptr) {
      grid.push_back(line);
    }
    // Find the initial units.
    for (int y = 0, endy = int(grid.size()); y < endy; y++) {
      for (int x = 0, endx = int(grid[y].size()); x < endx; x++) {
        char type = grid[y][x];
        if (type == 'E' || type == 'G') {
          ivec2 pos{ x, y };
          units_by_pos[pos] = units.size();
          units.push_back(Unit{ pos, type, 200 });
          if (type == 'E')
            ++num_elves;
          else
            ++num_goblins;
        }
      }
    }
  }


  void print_map() {
    for (const std::string& row : grid) {
      printf("%s", row.c_str());
    }
  }


  void print_units() {
    printf("%u units:\n", uint32_t(units.size()));
    for (size_t i = 0; i < units.size(); i++) {
      const Unit& u = units[i];
      printf("[%2u] %6s with %3d hp at {%2d,%2d}\n", uint32_t(i), (u.type == 'E') ? "Elf" : "Goblin", u.hit_points, u.pos.x, u.pos.y);
    }
  }


  char cell_type(ivec2 pos) const { return grid[pos.y][pos.x]; }
  void set_cell_type(ivec2 pos, char type) { grid[pos.y][pos.x] = type; }

  bool adjacent_to_type(ivec2 pos, char type)
  {
    return cell_type(pos + kDir[kNorth]) == type ||
           cell_type(pos + kDir[kWest]) == type ||
           cell_type(pos + kDir[kEast]) == type ||
           cell_type(pos + kDir[kSouth]) == type;
  }


  bool find_target_pos(ivec2 start_pos, char enemy_type, ivec2& target) const {
    // BFS using a priority queue, where priority is determined by distance then reading order.
    // This should mean that the first cell we reach that is adjacent to an enemy will be the cell we should travel towards.
    typedef std::pair<int, ivec2> Work;
    std::priority_queue<Work, std::deque<Work>, std::greater<Work>> q;
    std::set<ivec2> visited;
    q.push(std::make_pair(0, start_pos));
    while (!q.empty()) {
      Work work = q.top();
      q.pop();
      ivec2 pos = work.second;
      int dist = work.first + 1;
      for (int dir = 0; dir < 4; dir++) {
        ivec2 adj_pos = pos + kDir[dir];
        char type = cell_type(adj_pos);
        if (type == enemy_type) {
          target = pos;
          return true;
        }
        else if (type == '.' && visited.insert(adj_pos).second) {
          q.push(std::make_pair(dist, adj_pos));
        }
      }
    }
    return false;
  }


  ivec2 step_towards_target(ivec2 unit_pos, ivec2 target) const {
    // BFS using a priority queue, where priority is determined by distance then reading order.
    // This should mean that the first cell we reach that is adjacent to our unit will be the cell we should step into.
    typedef std::pair<int, ivec2> Work;
    std::priority_queue<Work, std::deque<Work>, std::greater<Work>> q;
    std::set<ivec2> visited;
    q.push(std::make_pair(0, target));
    while (!q.empty()) {
      Work work = q.top();
      q.pop();
      ivec2 pos = work.second;
      int dist = work.first + 1;
      for (int dir = 0; dir < 4; dir++) {
        ivec2 adj_pos = pos + kDir[dir];
        if (adj_pos == unit_pos)
          return pos;
        else if (cell_type(adj_pos) == '.' && visited.insert(adj_pos).second)
          q.push(std::make_pair(dist, adj_pos));
      }
    }
    // If we get here, something's gone terribly wrong.
    return ivec2{-1, -1};
  }


  size_t weakest_enemy(ivec2 unit_pos, char enemy_type) const {
    int min_hp = std::numeric_limits<int>::max();
    size_t min_enemy = std::numeric_limits<size_t>::max();
    for (int dir = 0; dir < 4; dir++) {
      ivec2 enemy_pos = unit_pos + kDir[dir];
      if (cell_type(enemy_pos) != enemy_type)
        continue;
      size_t enemy = units_by_pos.at(enemy_pos);
      if (units[enemy].hit_points < min_hp) {
        min_hp = units[enemy].hit_points;
        min_enemy = enemy;
      }
    }
    return min_enemy;
  }

  // Return values:
  // * 1 = completed full iteration, game still in progress
  // * 2 = completed full iteration, game over.
  // * 0 = game finished partway through this iteration
  // * -1 = game finished because an elf died
  int tick() {
    std::vector<size_t> order;
    order.reserve(units_by_pos.size());
    for (const auto& it : units_by_pos) {
      order.push_back(it.second);
    }

    for (size_t i : order) {
      Unit& unit = units[i];
      const char enemy_type = (unit.type == 'E') ? 'G' : 'E';
      if (num_goblins <= 0) {
        return 0;
      }
      if (unit.hit_points <= 0) {
        continue;
      }

      // Move if necessary
      if (!adjacent_to_type(unit.pos, enemy_type)) {
        ivec2 target_pos;
        if (!find_target_pos(unit.pos, enemy_type, target_pos))
          continue;
        ivec2 next_pos = step_towards_target(unit.pos, target_pos);
        set_cell_type(unit.pos, '.');
        set_cell_type(next_pos, unit.type);
        units_by_pos.erase(unit.pos);
        unit.pos = next_pos;
        units_by_pos[unit.pos] = i;
      }

      // Attack if possible
      size_t j = weakest_enemy(unit.pos, enemy_type);
      if (j == std::numeric_limits<size_t>::max()) {
        continue;
      }
      units[j].hit_points -= (unit.type == 'E') ? elf_power : goblin_power;
      if (units[j].hit_points <= 0) {
        set_cell_type(units[j].pos, '.');
        units_by_pos.erase(units[j].pos);
        if (enemy_type == 'E')
          return -1;
        else
          --num_goblins;
      }
    }

    return (num_goblins <= 0) ? 2 : 1;
  }


  int sum_of_remaining_hit_points() const {
    int sum = 0;
    for (const Unit& unit : units) {
      if (unit.hit_points > 0) {
        sum += unit.hit_points;
      }
    }
    return sum;
  }
};


int main(int argc, char** argv)
{
  Map map;

  FILE* f = fopen(argv[1], "r");
  map.load(f);
  fclose(f);

  int elf_attack_power = 4;
  int outcome = 0;
  while (true) {
    printf("Trying elf attack power = %d\n", elf_attack_power);

    Map localMap = map;
    localMap.elf_power = elf_attack_power;
    int result, rounds = 0;
    do {
      result = localMap.tick();
      if (result > 0)
        rounds++;
    } while (result == 1);
    if (result != -1) {
      outcome = rounds * localMap.sum_of_remaining_hit_points();
      break;
    }
    ++elf_attack_power;
  }

  printf("Answer = %d\n", outcome);
}
