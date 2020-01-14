#include <algorithm>
#include <cstdio>
#include <string>
#include <map>
#include <vector>

struct Guard {
  int id = 0;
  int minutes_asleep = 0;
  int by_minute[60] = {};
};

int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  char line[256];
  std::vector<std::string> lines;
  while (fgets(line, sizeof(line), f) != nullptr) {
    lines.push_back(line);
  }
  fclose(f);
  std::sort(lines.begin(), lines.end());

  std::map<int, Guard*> guards;
  int y, m, d, hour, min, id;
  Guard* current = nullptr;
  int asleep_at = 60;
  for (const std::string& str : lines) {
    sscanf(str.data(), "[%d-%d-%d %d:%d]", &y, &m, &d, &hour, &min);
    switch (str[19]) {
    case 'G':
      sscanf(str.data() + 19, "Guard #%d", &id);
      if (guards.count(id) == 0) {
        current = new Guard{};
        current->id = id;
        guards[id] = current;
      }
      else {
        current = guards[id];
      }
      break;
    case 'f':
      asleep_at = min;
      break;
    case 'w':
      for (int i = asleep_at; i < min; i++) {
        current->by_minute[i]++;
      }
      current->minutes_asleep += min - asleep_at;
      asleep_at = 60;
      break;
    default:
      fprintf(stderr, "oops, char = '%c'\n", str[19]);
      return 1;
    }
  }

  // Find the guard asleep for the most minutes.
  const Guard* sleepiest = current;
  for (const std::pair<int, Guard*> entry : guards) {
    const Guard* guard = entry.second;
    if (guard->minutes_asleep > sleepiest->minutes_asleep) {
      sleepiest = guard;
    }
  }

  // Find the minute that sleepiest guard is most commonly asleep on.
  int max_minute = 0;
  for (int i = 0; i < 60; i++) {
    if (sleepiest->by_minute[i] > sleepiest->by_minute[max_minute]) {
      max_minute = i;
    }
  }

  printf("Guard %d, minute %d, result = %d\n", sleepiest->id, max_minute, sleepiest->id * max_minute);
}
