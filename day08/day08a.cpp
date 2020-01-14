#include <cstdio>
#include <vector>

int parse_node(FILE* f)
{
  int num_children, num_metadata;
  fscanf(f, "%d %d ", &num_children, &num_metadata);

  int sum = 0;
  for (int i = 0; i < num_children; i++) {
    sum += parse_node(f);
  }

  for (int i = 0; i < num_metadata; i++) {
    int metadata;
    fscanf(f, "%d ", &metadata);
    sum += metadata;
  }

  return sum;
}


int main(int argc, char** argv)
{
  FILE* f = fopen(argv[1], "r");
  int sum = parse_node(f);
  fclose(f);
  printf("sum of metadata is %d\n", sum);
}
