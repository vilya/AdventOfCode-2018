#include <cstdio>
#include <vector>
#include <set>


void evolve(const std::set<int>& src, const bool rules[32], int lo, int hi, std::set<int>& dst)
{
  dst.clear();
  uint8_t r = ((src.count(lo - 2) > 0) ? 8 : 0) |
              ((src.count(lo - 1) > 0) ? 4 : 0) |
              ((src.count(lo    ) > 0) ? 2 : 0) |
              ((src.count(lo + 1) > 0) ? 1 : 0);
  for (int i = lo; i <= hi; i++) {
    r = ((r & 0xF) << 1) | ((src.count(i + 2) > 0) ? 1 : 0);
    if (rules[r])
      dst.insert(i);
  }
}


void show(const std::set<int>& src, int lo, int hi)
{
  for (int j = lo, endj = hi; j <= endj; j++)
    fputc((src.count(j) > 0) ? '#' : '.', stdout);
  fputc('\n', stdout);
}


void show_rule(uint8_t r, bool outcome)
{
  printf("[%2hhu] ", r);
  fputc(((r >> 4) & 1) ? '#' : '.', stdout);
  fputc(((r >> 3) & 1) ? '#' : '.', stdout);
  fputc(((r >> 2) & 1) ? '#' : '.', stdout);
  fputc(((r >> 1) & 1) ? '#' : '.', stdout);
  fputc(((r >> 0) & 1) ? '#' : '.', stdout);
  printf(" -> %c\n", outcome ? '#' : '.');
}


void show_rules(const bool rules[32])
{
  for (uint8_t i = 0; i < 32; i++) {
    show_rule(i, rules[i]);
  }
}


int main(int argc, char** argv)
{
  std::set<int> states[2];
  bool rules[32] = {};
  int lo = 0, hi = 0;

  char initial_state[256];
  char rule[6] = {};
  char outcome;
  FILE* f = fopen(argv[1], "r");
  fscanf(f, "initial state: %s\n\n", initial_state);
  for (hi = 0; initial_state[hi] != '\0'; hi++) {
    if (initial_state[hi] == '#') {
      states[0].insert(hi);
    }
  }
  while (fscanf(f, "%s => %c\n", rule, &outcome) == 2) {
    uint8_t r = 0;
    for (int i = 0; i < 5; i++) {
      r = (r << 1) | ((rule[i] == '#') ? 1 : 0);
    }
    rules[r] = (outcome == '#');
  }
  fclose(f);

//  show_rules(rules);

  const int generations = 150;
  int src = 0;
//  show(states[src], lo, hi + generations);
  for (int i = 0; i < generations; i++) {
    int dst = src ^ 1;
    evolve(states[src], rules, lo, hi + i, states[dst]);
    src = dst;
//    show(states[src], lo, hi + generations);
  }

  int sum = 0;
  for (int p : states[src]) {
    sum += p;
  }
  printf("sum after %d generations = %d\n", generations, sum);

  int64_t big_generations = 50'000'000'000;
  int64_t big_sum = int64_t(sum) + (big_generations - generations) * int64_t(states[src].size());
  printf("sum after %lld generations = %lld\n", big_generations, big_sum);
}
