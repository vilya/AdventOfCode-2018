#include <cstdio>
#include <cstdint>
#include <vector>


constexpr uint32_t kPuzzleInput = 209231;
//constexpr uint32_t kPuzzleInput = 2018;


void append(std::vector<uint8_t>& digits, uint32_t val)
{
  uint8_t val_digits[32];
  int num = 0;
  if (val <= 0) {
    val_digits[num++] = 0;
  }
  else {
    while (val > 0) {
      val_digits[num++] = uint8_t(val % 10);
      val /= 10;
    }
  }
  while (num > 0) {
    digits.push_back(val_digits[--num]);
  }
}


void step(std::vector<uint8_t>& digits, size_t elf[2])
{
  uint32_t new_recipe = digits[elf[0]] + digits[elf[1]];
  append(digits, new_recipe);

  elf[0] = (elf[0] + 1 + digits[elf[0]]) % digits.size();
  elf[1] = (elf[1] + 1 + digits[elf[1]]) % digits.size();
}


void draw(std::vector<uint8_t>& digits, size_t elf[2])
{
  for (size_t i = 0; i < digits.size(); i++) {
    if (i == elf[0] && i == elf[1]) {
      printf("{%hhu}", digits[i]);
    }
    else if (i == elf[0]) {
      printf("(%hhu)", digits[i]);
    }
    else if (i == elf[1]) {
      printf("[%hhu]", digits[i]);
    }
    else {
      printf(" %hhu ", digits[i]);
    }
  }
  printf("\n");
}


int main()
{
  std::vector<uint8_t> digits;
  digits.reserve(kPuzzleInput + 12);
  append(digits, 37);

  size_t elf[2] = { 0, 1 };

//  draw(digits, elf);
  while (digits.size() < kPuzzleInput + 10) {
    step(digits, elf);
//    draw(digits, elf);
  }

  for (size_t i = kPuzzleInput; i < kPuzzleInput + 10; i++) {
    fputc(digits[i] + '0', stdout);
  }
  fputc('\n', stdout);
}
