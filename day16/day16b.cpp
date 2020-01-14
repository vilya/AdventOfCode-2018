#include <cstdio>

enum Op {
  ADDR,
  ADDI,
  MULR,
  MULI,
  BANR,
  BANI,
  BORR,
  BORI,
  SETR,
  SETI,
  GTIR,
  GTRI,
  GTRR,
  EQIR,
  EQRI,
  EQRR,
};
const Op kFirstOp = ADDR;
const Op kLastOp = EQRR;


void eval(Op op, const int instr[4], int reg[4])
{
  switch (op) {
  case ADDR: reg[instr[3]] = reg[instr[1]] + reg[instr[2]]; break;
  case ADDI: reg[instr[3]] = reg[instr[1]] + instr[2]; break;
  case MULR: reg[instr[3]] = reg[instr[1]] * reg[instr[2]]; break;
  case MULI: reg[instr[3]] = reg[instr[1]] * instr[2]; break;
  case BANR: reg[instr[3]] = reg[instr[1]] & reg[instr[2]]; break;
  case BANI: reg[instr[3]] = reg[instr[1]] & instr[2]; break;
  case BORR: reg[instr[3]] = reg[instr[1]] | reg[instr[2]]; break;
  case BORI: reg[instr[3]] = reg[instr[1]] | instr[2]; break;
  case SETR: reg[instr[3]] = reg[instr[1]]; break;
  case SETI: reg[instr[3]] = instr[1]; break;
  case GTIR: reg[instr[3]] = int(instr[1] > reg[instr[2]]); break;
  case GTRI: reg[instr[3]] = int(reg[instr[1]] > instr[2]); break;
  case GTRR: reg[instr[3]] = int(reg[instr[1]] > reg[instr[2]]); break;
  case EQIR: reg[instr[3]] = int(instr[1] == reg[instr[2]]); break;
  case EQRI: reg[instr[3]] = int(reg[instr[1]] == instr[2]); break;
  case EQRR: reg[instr[3]] = int(reg[instr[1]] == reg[instr[2]]); break;
  }
}


struct Example {
  int before[4];
  int instr[4];
  int after[4];

  bool read(FILE* f) {
    int n = fscanf(f, "Before: [%d, %d, %d, %d]\n%d %d %d %d\nAfter: [%d, %d, %d, %d]\n\n",
                   &before[0], &before[1], &before[2], &before[3],
                   &instr[0], &instr[1], &instr[2], &instr[3],
                   &after[0], &after[1], &after[2], &after[3]);
    return n == 12;
  }

  bool try_op(Op op) const {
    int reg[4] = { before[0], before[1], before[2], before[3] };
    eval(op, instr, reg);
    return reg[instr[3]] == after[instr[3]];
  }
};


int main(int argc, char **argv)
{
  unsigned int candidates[16];
  for (unsigned int& c : candidates) {
    c = 0xFFFFu;
  }

  Example example;
  FILE* f = fopen(argv[1], "r");
  int num_examples = 0;
  while (example.read(f)) {
    unsigned int example_candidates = 0;
    for (int op = kFirstOp; op <= kLastOp; op++) {
      if (example.try_op(Op(op)))
        example_candidates |= (1u << op);
    }
    candidates[example.instr[0]] &= example_candidates;
    ++num_examples;
  }
  printf("Processed %d examples\n", num_examples);

  Op lookup[16];
  bool found[16] = {};
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      int num_candidates = __builtin_popcount(candidates[j]);
      if (num_candidates != 1)
        continue;
      lookup[j] = Op(__builtin_ctz(candidates[j]));
      found[j] = true;
      unsigned int mask = ~candidates[j];
      for (int k = 0; k < 16; k++) {
        candidates[k] &= mask;
      }
      break;
    }
  }

  bool has_errors = false;
  for (int i = 0; i < 16; i++) {
    if (!found[i]) {
      printf("Don't know which instruction %d is\n", i);
      has_errors = true;
    }
  }
  if (has_errors) {
    return 1;
  }

  fscanf(f, "\n\n");
  int reg[4] = { 0, 0, 0, 0 };
  int instr[4];
  while (fscanf(f, "%d %d %d %d\n", &instr[0], &instr[1], &instr[2], &instr[3]) == 4) {
    Op op = lookup[instr[0]];
    eval(op, instr, reg);
  }
  fclose(f);

  printf("Result = %d\n", reg[0]);
}
