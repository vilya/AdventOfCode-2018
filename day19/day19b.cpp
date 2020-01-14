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
  int64_t a, b, dst;

  bool load(FILE* f) {
    char name[16] = {};
//    int n = fscanf(f, "%s %d %d %d\n", name, &a, &b, &dst);
    int n = fscanf(f, "%s %lld %lld %lld\n", name, &a, &b, &dst);
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
  int64_t reg[7] = { 0, 0, 0, 0, 0, 0 };
  int64_t ip = 0;
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

  bool step(bool trace) {
    if (ip < 0 || ip >= int(prog.size())) {
      return false;
    }
    const Instruction& instr = prog[ip];
    reg[ip_binding] = ip;

    if (trace) {
//      printf("ip = %3d, ", ip);
//      printf("[%d, %d, %d, %d, %d, %d]", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5]);
//      printf("   %s %d %d %d   ", kOpNames[instr.op], instr.a, instr.b, instr.dst);
      printf("ip = %3lld, ", ip);
      printf("[%lld, %lld, %lld, %lld, %lld, %lld]", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5]);
      printf("   %s %lld %lld %lld   ", kOpNames[instr.op], instr.a, instr.b, instr.dst);
    }

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

    if (trace) {
//      printf("[%d, %d, %d, %d, %d, %d]", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5]);
      printf("[%lld, %lld, %lld, %lld, %lld, %lld]", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5]);
      printf("\n");
      fflush(stdout);
    }

    ip = reg[ip_binding];
    ++ip;
    return true;
  }


  void run(bool trace) {
    while (true) {
      if (!step(trace)) {
        break;
      }
    }
  }


  void run_to(int64_t ip_to_halt_at, bool trace) {
    while (ip != ip_to_halt_at && step(trace));
  }
};


int64_t reverse_engineered(bool part_b)
{
  const int64_t num = part_b ? 10551309 : 909;
  int64_t result = 0;

  for (int64_t i = 1; i <= num; i++) {
    if (num % i == 0) {
      result += i;
    }
  }

  return result;
}


int main(int argc, char **argv)
{
//  ElfCode elfcode;

//  FILE* f = fopen(argv[1], "r");
//  elfcode.load(f);
//  fclose(f);

//  elfcode.reg[0] = 1;
//  elfcode.run_to(3, true);
//  elfcode.run(true);

//  printf("Register 0 contains %d\n", elfcode.reg[0]);
//  printf("Register 0 contains %lld\n", elfcode.reg[0]);
  printf("Register 0 contains %lld\n", reverse_engineered(true));
}
