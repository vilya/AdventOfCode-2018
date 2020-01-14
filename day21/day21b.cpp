#include <cstdio>
#include <vector>
#include <set>

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


void reveng_step(int& d, int& e)
{
  e = d | 0x10000;
  d = 2176960;
  while (e != 0) {
    d += ((e & 0xff) & 0xffffff);
    d *= 65899;
    d &= 0xffffff;
    e /= 256;
  }
}


int64_t reverse_engineered()
{
  int a = 11474091;
  int d = 0, e = 0;
  while (d != a) {
    reveng_step(d, e);
  }
  return d;
}


void find_cycle()
{
  int slow_d = 0, slow_e = 0;
  int fast_d = 0, fast_e = 0;

  int cycle_start = 0;
  do {
    reveng_step(slow_d, slow_e);

    reveng_step(fast_d, fast_e);
    reveng_step(fast_d, fast_e);

    ++cycle_start;
  } while (slow_d != fast_d);
  printf("found cycle after %d iterations\n", cycle_start);
  fflush(stdout);

  int cycle_len = 0;
  do {
    reveng_step(slow_d, slow_e);
    ++cycle_len;
  } while (slow_d != fast_d);
  printf("cycle repeats after %d iterations\n", cycle_len);
  fflush(stdout);
}


int find_end_of_first_cycle()
{
  std::set<int> seen;
  int d = 0, e = 0;
  int prev_d = 0;

  reveng_step(d, e);
  while (seen.insert(d).second) {
    prev_d = d;
    reveng_step(d, e);
  }
  return prev_d;
}


int main(int argc, char **argv)
{
//  ElfCode elfcode;

//  FILE* f = fopen(argv[1], "r");
//  elfcode.load(f);
//  fclose(f);

//  elfcode.run_to(28, false);
//  elfcode.run(true);

//  printf("ElfCode register 3 contains %lld\n", elfcode.reg[3]);
//  printf("Reverse engineered register 3 contains %lld\n", reverse_engineered());

  find_cycle();

  printf("Answer = %d\n", find_end_of_first_cycle());
}
