#include <cstdio>
#include <vector>
#include <string>


static constexpr char kOpen = '.';
static constexpr char kTree = '|';
static constexpr char kYard = '#';

static constexpr int kOpenCount = 0;
static constexpr int kTreeCount = 1;
static constexpr int kYardCount = 2;

//static constexpr int kGridW = 10;
//static constexpr int kGridH = 10;
static constexpr int kGridW = 50;
static constexpr int kGridH = 50;


struct Grid {
  char** data;

  Grid() {
    data = new char*[kGridH + 2];
    for (int i = 0; i < kGridH + 2; i++) {
      data[i] = new char[kGridW + 2];
      std::memset(data[i], 0, sizeof(char) * (kGridW + 2));
      ++data[i];
    }
    ++data;
  }

  ~Grid() {
    --data;
    for (int i = 0; i < kGridH + 2; i++) {
      --data[i];
      delete[] data[i];
    }
    delete[] data;
  }

  char get(int x, int y) const { return data[y][x]; }
  void set(int x, int y, char val) { data[y][x] = val; }

  void load(FILE* f) {
    char line[64] = {};
    for (int i = 0; i < kGridH; i++) {
      fgets(line, sizeof(line), f);
      std::memcpy(data[i], line, sizeof(char) * kGridW);
    }
  }

  void print() {
    for (int y = 0; y < kGridH; y++) {
      printf("%s\n", data[y]);
    }
  }

  int resource_value() const {
    int num_trees = 0;
    int num_yards = 0;
    for (size_t y = 0; y < kGridH; y++) {
      for (size_t x = 0; x < kGridW; x++) {
        char ch = get(x, y);
        if (ch == kTree)
          ++num_trees;
        else if (ch == kYard)
          ++num_yards;
      }
    }
    return num_trees * num_yards;
  }

  void neighbours_by_type(int x, int y, int& num_open, int& num_trees, int& num_yards) const {
    num_open = 0;
    num_trees = 0;
    num_yards = 0;
    for (int ny = y - 1; ny <= y + 1; ny++) {
      for (int nx = x - 1; nx <= x + 1; nx++) {
        if (nx == x && ny == y)
          continue;
        switch (get(nx, ny)) {
          case kOpen: num_open++; break;
          case kTree: num_trees++; break;
          case kYard: num_yards++; break;
          default: break;
        }
      }
    }
  }

  void evolve_to(Grid& dst) {
    int num_open, num_trees, num_yards;
    for (int y = 0; y < kGridH; y++) {
      for (int x = 0; x < kGridW; x++) {
        neighbours_by_type(x, y, num_open, num_trees, num_yards);
        switch (get(x, y)) {
        case kOpen:
          dst.set(x, y, (num_trees >= 3) ? kTree : kOpen);
          break;
        case kTree:
          dst.set(x, y, (num_yards >= 3) ? kYard : kTree);
          break;
        case kYard:
          dst.set(x, y, (num_yards >= 1 && num_trees >= 1) ? kYard : kOpen);
          break;
        default:
          fprintf(stderr, "Invalid cell type '%c' at %d,%d\n", get(x, y), x, y);
          exit(1);
          break;
        }
      }
    }
  }
};


int main(int argc, char** argv)
{
  Grid grids[2];

  FILE* f = fopen(argv[1], "r");
  grids[0].load(f);
  fclose(f);

  printf("Initial state:\n");
  grids[0].print();
  printf("\n");

  int src = 0;
  for (int i = 1; i <= 10; i++) {
    int dst = src ^ 1;
    grids[src].evolve_to(grids[dst]);
    src = dst;

    printf("After %d minutes:\n", i);
    grids[src].print();
    printf("\n");
  }

  printf("Result = %d\n", grids[src].resource_value());
}
