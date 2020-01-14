#include <cstdio>
#include <cstdint>
#include <vector>


constexpr uint32_t kPuzzleInput = 209231;


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


size_t ends_with(const std::vector<uint8_t>& digits, const std::vector<uint8_t>& suffix, size_t num_added)
{
  const size_t dn = digits.size();
  const size_t sn = suffix.size();
  if (dn < sn) {
    return std::numeric_limits<size_t>::max();
  }
  for (size_t i = 0; i < num_added; i++) {
    size_t start = dn - sn - num_added + i;
    if (std::memcmp(digits.data() + start, suffix.data(), sizeof(uint8_t) * sn) == 0) {
      return start;
    }
  }
  return std::numeric_limits<size_t>::max();;
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
  digits.reserve(1000000);
  append(digits, 37);

  std::vector<uint8_t> suffix;
  append(suffix, kPuzzleInput);
//  std::vector<uint8_t> suffix{ 5, 9, 4, 1, 4 };

  size_t elf[2] = { 0, 1 };

//  draw(digits, elf);
  size_t n_new = digits.size();
  size_t n = n_new - 1;
  size_t result = ends_with(digits, suffix, n_new - n);
  while (result == std::numeric_limits<size_t>::max()) {
    step(digits, elf);
//    draw(digits, elf);
    n = n_new;
    n_new = digits.size();
    result = ends_with(digits, suffix, n_new - n);
  }
  printf("%llu\n", uint64_t(result));
}
