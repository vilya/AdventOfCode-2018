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
    return reg[instr[3]] == after[instr[3]];
  }
};


int main(int argc, char **argv)
{
  int count = 0;

  Example example;
  FILE* f = fopen(argv[1], "r");
  while (example.read(f)) {
    int candidates = 0;
    for (int op = kFirstOp; op <= kLastOp && candidates < 3; op++) {
      if (example.try_op(Op(op)))
        ++candidates;
    }
    if (candidates >= 3) {
      ++count;
    }
  }

  printf("Result = %d\n", count);
}
