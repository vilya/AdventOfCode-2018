#include <cstdio>

static constexpr int kGridSerial = 5468;


int power(int x, int y)
{
  int rack_id = x + 10;
  int power_level = ((rack_id * y + kGridSerial) * rack_id / 100) % 10 - 5;
  return power_level;
}


int main()
{
  // Build a summed area table of the grid power values.
  int sat[301][301];
  sat[0][0] = 0;
  for (int x = 1; x <= 300; x++) {
    sat[0][x] = 0;
  }
  for (int y = 1; y <= 300; y++) {
    sat[y][0] = 0;
  }
  for (int y = 1; y <= 300; y++) {
    for (int x = 1; x <= 300; x++) {
      sat[y][x] = power(x, y) + sat[y-1][x] + sat[y][x-1] - sat[y-1][x-1];
    }
  }

  int best_x = -1, best_y = -1, best_power = 0;
  for (int y = 3; y <= 300; y++) {
    for (int x = 3; x <= 300; x++) {
      int power3x3 = sat[y][x] - sat[y-3][x] - sat[y][x-3] + sat[y-3][x-3];
      if (power3x3 > best_power) {
        best_x = x;
        best_y = y;
        best_power = power3x3;
      }
    }
  }

  best_x -= 2;
  best_y -= 2;
  printf("%d,%d\n", best_x, best_y);
}
