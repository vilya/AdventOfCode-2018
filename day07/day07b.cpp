#include <algorithm>
#include <cstdio>
#include <vector>

constexpr int kNumWorkers = 5;
constexpr int kTaskSecs = 61;

enum State {
  eInvalid,
  eWaiting,
  eReady,
  eRunning,
  eDone,
};


int update_readiness(State states[26], const bool requires[26][26])
{
  int num_ready = 0;
  for (int i = 0; i < 26; i++) {
    if (states[i] == eReady)
      ++num_ready;
    if (states[i] != eWaiting)
      continue;

    bool ready = true;
    for (int j = 0; j < 26; j++) {
      if (requires[i][j] && states[j] != eDone) {
        ready = false;
        break;
      }
    }
    if (ready) {
      states[i] = eReady;
      ++num_ready;
    }
  }
  return num_ready;
}


int main(int argc, char** argv)
{
  State states[26];
  for (int i = 0; i < 26; i++) {
    states[i] = eInvalid;
  }
  bool requires[26][26] = {};

  FILE* f = fopen(argv[1], "r");
  char src, dst;
  while (fscanf(f, "Step %c must be finished before step %c can begin.\n", &src, &dst) == 2) {
    requires[dst - 'A'][src - 'A'] = true;
    states[src - 'A'] = eWaiting;
    states[dst - 'A'] = eWaiting;
  }
  fclose(f);

  int num_tasks = 0, num_finished = 0;
  for (int i = 0; i < 26; i++) {
    if (states[i] == eWaiting)
      ++num_tasks;
  }

  char finished[27] = {};
  int busy_until[kNumWorkers] = { 0, 0, 0, 0, 0 };
  int working_on[kNumWorkers] = { -1, -1, -1, -1, -1 };
  int t = 0;

  while (num_finished < num_tasks) {
//    printf("t = %5d, %2d/%2d tasks finished: %s\n", t, num_finished, num_tasks, finished);

    // Have any available workers take on new tasks, if any are available.
    int num_ready = update_readiness(states, requires);
    int prev_j = -1;
    for (int i = 0; i < kNumWorkers && num_ready > 0; i++) {
      if (working_on[i] != -1) {
        continue;
      }
      // Worker is available, let's find a task for it to do...
      for (int j = prev_j + 1; j < 26; j++) {
        if (states[j] == eReady) {
          working_on[i] = j;
          busy_until[i] = t + kTaskSecs + j;
          states[j] = eRunning;
          --num_ready;
          prev_j = j;
          break;
        }
      }
    }

    // Wait until the next task completes.
    int min_t = std::numeric_limits<int>::max();
    for (int i = 0; i < kNumWorkers; i++) {
      if (busy_until[i] > t && busy_until[i] < min_t) {
        min_t = busy_until[i];
      }
    }
    t = min_t;

    // Add tasks to the finished list for any workers that have completed since
    // the previous iteration and mark those workers as available.
    for (int i = 0; i < kNumWorkers; i++) {
      if (working_on[i] != -1 && busy_until[i] == t) {
        finished[num_finished++] = static_cast<char>('A' + working_on[i]);
        states[working_on[i]] = eDone;
        working_on[i] = -1;
      }
    }
  }

  printf("%s in %d secs\n", finished, t);
}
