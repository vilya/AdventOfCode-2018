#include <cstdio>
#include <vector>

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


static constexpr const char* kOpNames[] = {
  "addr",
  "addi",
  "mulr",
  "muli",
  "banr",
  "bani",
  "borr",
  "bori",
  "setr",
  "seti",
  "gtir",
  "gtri",
  "gtrr",
  "eqir",
  "eqri",
  "eqrr",
};


struct Instruction {
  Op op;
  int a, b, dst;

  bool load(FILE* f) {
    char name[16] = {};
    int n = fscanf(f, "%s %d %d %d\n", name, &a, &b, &dst);
    if (n != 4) {
      return false;
    }
    bool valid = false;
    for (int i = kFirstOp; i <= kLastOp; i++) {
      if (strcmp(name, kOpNames[i]) == 0) {
        op = Op(i);
        valid = true;
        break;
      }
    }
    return valid;
  }
};


struct ElfCode {
  int reg[7] = { 0, 0, 0, 0, 0, 0 };
  int ip = 0;
  int ip_binding = 6;
  std::vector<Instruction> prog;

  void reset() {
    std::memset(reg, 0, sizeof(reg));
    ip = 0;
    ip_binding = 6;
    prog.clear();
  }

  void load(FILE* f) {
    reset();
    fscanf(f, "#ip %d\n", &ip_binding);
    Instruction instr;
    while (instr.load(f)) {
      prog.push_back(instr);
    }
  }

  bool step() {
    if (ip < 0 || ip >= int(prog.size())) {
      return false;
    }
    const Instruction& instr = prog[ip];
    reg[ip_binding] = ip;
    switch (instr.op) {
      case ADDR: reg[instr.dst] = reg[instr.a] + reg[instr.b]; break;
      case ADDI: reg[instr.dst] = reg[instr.a] + instr.b; break;
      case MULR: reg[instr.dst] = reg[instr.a] * reg[instr.b]; break;
      case MULI: reg[instr.dst] = reg[instr.a] * instr.b; break;
      case BANR: reg[instr.dst] = reg[instr.a] & reg[instr.b]; break;
      case BANI: reg[instr.dst] = reg[instr.a] & instr.b; break;
      case BORR: reg[instr.dst] = reg[instr.a] | reg[instr.b]; break;
      case BORI: reg[instr.dst] = reg[instr.a] | instr.b; break;
      case SETR: reg[instr.dst] = reg[instr.a]; break;
      case SETI: reg[instr.dst] = instr.a; break;
      case GTIR: reg[instr.dst] = int(instr.a > reg[instr.b]); break;
      case GTRI: reg[instr.dst] = int(reg[instr.a] > instr.b); break;
      case GTRR: reg[instr.dst] = int(reg[instr.a] > reg[instr.b]); break;
      case EQIR: reg[instr.dst] = int(instr.a == reg[instr.b]); break;
      case EQRI: reg[instr.dst] = int(reg[instr.a] == instr.b); break;
      case EQRR: reg[instr.dst] = int(reg[instr.a] == reg[instr.b]); break;
    }
    ip = reg[ip_binding];
    ++ip;
    return true;
  }

  void run() {
    while (step());
  }
};


int main(int argc, char **argv)
{
  ElfCode elfcode;

  FILE* f = fopen(argv[1], "r");
  elfcode.load(f);
  fclose(f);

  elfcode.run();
  printf("Register 0 contains %d\n", elfcode.reg[0]);
}
