#include <cstdio>
#include <vector>


int main(int argc, char** argv)
{
  int num_players, last_score, high_score;

  FILE* f = fopen(argv[1], "r");
  bool has_high_score = fscanf(f, "%d players; last marble is worth %d points: high score is %d", &num_players, &last_score, &high_score) == 3;
  fclose(f);

  std::vector<int> score(size_t(num_players), 0);
  std::vector<int> marbles = { 0, 2, 1 };

  int player = 3;
  int64_t marble = 3;
  int64_t pos = 1;
  while (true) {
    int64_t size = int64_t(marbles.size());
    int64_t next_pos, new_score = 0;
    if (marble % 23 == 0) {
      next_pos = (pos + size - 7) % size;
      new_score = marble + marbles[next_pos];;
      marbles.erase(marbles.begin() + next_pos);
    }
    else {
      next_pos = (pos + 2) % size;
      marbles.insert(marbles.begin() + next_pos, marble);
    }
    score[player] += new_score;
    pos = next_pos;
    player = (player + 1) % num_players;
    marble++;
    if (new_score == last_score) {
      printf("Finished at marble %lld\n", marble);
      break;
    }
    else if (marble > last_score) {
      printf("Looks like we overshot!\n");
      break;
    }
  }

  printf("Scores:\n");
  int best = 0;
  for (int i = 0; i < num_players; i++) {
//    printf("- Player %3d: %d\n", i, score[i]);
    if (score[i] > score[best]) {
      best = i;
    }
  }

  printf("Best score is player %d with %d\n", best, score[best]);
  if (has_high_score && score[best] != high_score) {
    printf("Answer is WRONG, expected %d!\n", high_score);
  }
}
