#include <iostream>
#include <string>
#include <bitset>
#include <vector>

using namespace std;

#define BLOCK_SIZE 64

uint32_t rightrotate32(uint32_t x, int i) {
  i %= 32;
  auto rot = x >> i;
  return rot | (x << (32 - i)); 
}

uint32_t little_sigma0(uint32_t x) {
  return rightrotate32(x, 7) ^ rightrotate32(x, 18) ^ (x >> 3);
}

uint32_t little_sigma1(uint32_t x) {
  return rightrotate32(x, 17) ^ rightrotate32(x, 19) ^ (x >> 10);
}

vector<uint32_t> message_schedule(vector<uint8_t> const& block) {
  assert(block.size() == BLOCK_SIZE);

  vector<uint32_t> schedule(BLOCK_SIZE);

  for (int i = 0; i < block.size(); i += 4) {
    uint32_t word = 0;
    for (int j = 0; j < 4; j++) {
      word |= block[i+j] << (24 - (8 * j));
    }
    schedule[i / 4] = word;
  }

  return schedule;
}

vector<uint8_t> block_from_string(string const& s) {
  assert(s.size() == BLOCK_SIZE);

  vector<uint8_t> block(BLOCK_SIZE);
  for (int i = 0; i < s.size(); i++) {
    block[i] = (uint32_t)s[i];
  }

  return block;
}

int main() {
  string s("iguana wombat dog kangaroo llama turkey yak unicorn sheep xenoce");
  s.resize(BLOCK_SIZE);

  auto schedule = message_schedule(block_from_string(s));
  for (int i = 0; i < 16; i++) {
    cout << schedule[i] << " ";
  }
}