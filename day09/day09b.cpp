#include <cstdio>
#include <vector>

int get_nth_prev(const std::vector<int>& prev, int curr, int n)
{
  while (n > 0) {
    curr = prev[curr];
    --n;
  }
  return curr;
}


int insert_after(std::vector<int>& prev, std::vector<int>& next, int p, int marble)
{
  int n = next[p];
  next[p] = marble;
  prev[n] = marble;
  next[marble] = n;
  prev[marble] = p;
  return marble;
}


int remove(std::vector<int>& prev, std::vector<int>& next, int i)
{
  int p = prev[i];
  int n = next[i];
  prev[n] = p;
  next[p] = n;
  return n;
}


int main(int argc, char** argv)
{
  int num_players, last_score;

  FILE* f = fopen(argv[1], "r");
  fscanf(f, "%d players; last marble is worth %d points", &num_players, &last_score);
  fclose(f);

  last_score *= 100;

  std::vector<int64_t> score(size_t(num_players), 0);
  std::vector<int> next(size_t(last_score + 1), 0);
  std::vector<int> prev(size_t(last_score + 1), 0);
  for (size_t i = 0; i <= last_score; i++) {
    next[i] = i;
    prev[i] = i;
  }
  int current = 0;

  int player = 1;
  for (int marble = 1; marble <= last_score; marble++) {
    if (marble % 23 == 0) {
      int tmp = get_nth_prev(prev, current, 7);
      score[player] += (marble + tmp);
      current = remove(prev, next, tmp);
    }
    else {
      current = insert_after(prev, next, next[current], marble);
    }
    player = (player + 1) % num_players;
  }

  int best = 0;
  for (int i = 1; i < num_players; i++) {
    if (score[i] > score[best]) {
      best = i;
    }
  }

  printf("Best score is player %d with %lld\n", best, score[best]);
}
