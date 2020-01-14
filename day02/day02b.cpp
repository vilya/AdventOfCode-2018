#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

size_t find_difference(size_t len, const std::string& a, const std::string& b)
{
  size_t k = 0;
  while (k < len && a[k] == b[k]) {
    k++;
  }
  size_t differs_at = k++;
  while (k < len && a[k] == b[k]) {
    k++;
  }
  return (differs_at < len && k >= len) ? differs_at : len;
}


int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  char line[256] = {};
  std::vector<std::string> lines;
  while (fgets(line, sizeof(line), f) != nullptr) {
    lines.push_back(line);
  }

  const size_t n = lines.size();
  const size_t len = lines[0].size(); // assuming all IDs are the same length.
  for (size_t i = 0; i < n; i++) {
    for (size_t j = 0; j < n; j++) {
      size_t diff = find_difference(len, lines[i], lines[j]);
      if (diff >= len) {
        continue;
      }
      for (size_t k = 0; k < len; k++) {
        if (k == diff) {
          continue;
        }
        fputc(lines[i][k], stdout);
      }
      fputc('\n', stdout);
      return 0;
    }
  }
}
