#include <cctype>
#include <cstdio>
#include <limits>
#include <string>

static constexpr int kSize = 50000;


std::string remove_polymer(const std::string& src, char polymer, std::string& dst)
{
  polymer |= 32;

  dst.clear();
  dst.reserve(src.size());

  for (char ch : src) {
    if ((ch | 32) != polymer)
      dst.push_back(ch);
  }

  return dst;
}


void react(std::string& chain)
{
  size_t i = 1;
  while (i < chain.size()) {
    char ch0 = chain[i-1];
    char ch1 = chain[i];
    if (ch0 != ch1 && (ch0 | 32) == (ch1 | 32)) {
      chain.erase(i - 1, 2);
      i--;
      if (i == 0)
        i++;
    }
    else {
      i++;
    }
  }
}


int main(int argc, char** argv)
{
  char line[kSize + 1];
  FILE* f = fopen(argv[1], "r");
  fread(line, sizeof(char), kSize, f);
  fclose(f);
  line[kSize] = '\0';

  std::string src = line;
  std::string dst;
  dst.reserve(src.size());

  char best_polymer = '\0';
  size_t best_result = std::numeric_limits<size_t>::max();
  for (char polymer = 'a'; polymer <= 'z'; polymer++) {
    remove_polymer(src, polymer, dst);
    react(dst);
    if (dst.size() < best_result) {
      best_polymer = polymer;
      best_result = dst.size();
    }
  }

  printf("Result = %d\n", int(best_result));
}
