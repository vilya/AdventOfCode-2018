#include <cstdio>
#include <vector>

int parse_node(FILE* f)
{
  int num_children, num_metadata;
  fscanf(f, "%d %d ", &num_children, &num_metadata);

  int sum = 0, metadata;
  if (num_children > 0) {
    std::vector<int> child_sums(size_t(num_children + 1), 0);
    for (int i = 1; i <= num_children; i++) {
      child_sums[i] = parse_node(f);
    }
    for (int i = 0; i < num_metadata; i++) {
      fscanf(f, "%d ", &metadata);
      if (metadata > 0 && metadata <= num_children)
        sum += child_sums[metadata];
    }
  }
  else {
    for (int i = 0; i < num_metadata; i++) {
      int metadata;
      fscanf(f, "%d ", &metadata);
      sum += metadata;
    }
  }
  return sum;
}


int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  int val = parse_node(f);
  fclose(f);
  printf("value of root node is %d\n", val);
}
