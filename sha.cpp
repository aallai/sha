#include <iostream>
#include <string>
#include <bitset>
#include <vector>

using namespace std;

static const vector<uint32_t> ROUND_CONSTANTS = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

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

uint32_t big_sigma0(uint32_t x) {
  return rightrotate32(x, 2) ^ rightrotate32(x, 13) ^ rightrotate32(x, 22);
}

uint32_t big_sigma1(uint32_t x) {
  return rightrotate32(x, 6) ^ rightrotate32(x, 11) ^ rightrotate32(x, 25);
}

uint32_t choice(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (~x & z);
}

uint32_t majority(uint32_t x, uint32_t y, uint32_t z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

vector<uint32_t> round(vector<uint32_t> const& state, uint32_t round_constant, uint32_t schedule_word) {
  assert(state.size() == 8);

  auto ch = choice(state[4], state[5], state[6]);
  auto temp1 = state[7] + big_sigma1(state[4]) + ch + round_constant + schedule_word;
  auto maj = majority(state[0], state[1], state[2]);
  auto temp2 = big_sigma0(state[0]) + maj;

  return vector<uint32_t>{
    temp1 + temp2,
    state[0],
    state[1],
    state[2],
    state[3] + temp1,
    state[4],
    state[5],
    state[6],
  };
}

vector<uint32_t> message_schedule(vector<uint8_t> const& block) {
  assert(block.size() == 64);

  vector<uint32_t> W(64);

  for (int i = 0; i < block.size(); i += 4) {
    uint32_t word = 0;
    for (int j = 0; j < 4; j++) {
      word |= block[i+j] << (24 - (8 * j));
    }
    W[i / 4] = word;
  }

  for (int i = 16; i < W.size(); i++) {
    W[i] = W[i-16] + little_sigma0(W[i-15]) + W[i-7] + little_sigma1(W[i-2]);
  }

  return W;
}

vector<uint8_t> block_from_string(string const& s) {
  assert(s.size() == 64);

  vector<uint8_t> block(64);
  for (int i = 0; i < s.size(); i++) {
    block[i] = (uint32_t)s[i];
  }

  return block;
}

vector<uint32_t> compress(vector<uint32_t> const& input_state, vector<uint32_t> const& block) {
  assert(input_state.size() == 8);
  assert(block.size() == 64);

  auto W = message_schedule(block);
  auto state = input_state;

  for (int i = 0; i < 64; i++) {
    state = round(state, ROUND_CONSTANTS[i], W[i]);
  }

  return {
    input_state[0] + state[0],
    input_state[1] + state[1],
    input_state[2] + state[2],
    input_state[3] + state[3],
    input_state[4] + state[4],
    input_state[5] + state[5],
    input_state[6] + state[6],
    input_state[7] + state[7],
  }
}

void test_schedule() {
  string s("iguana wombat dog kangaroo llama turkey yak unicorn sheep xenoce");
  s.resize(64);

  auto W = message_schedule(block_from_string(s));

  auto expected_W = vector<uint32_t>{
    1768387937, 1851859063, 1869439585, 1948279919, 1730177889, 1852268914, 1869553772, 1818324321,
    544503154, 1801812256, 2036427552, 1970170211, 1869770272, 1936221541, 1881176165, 1852793701,
    3002878561, 3711121932, 1520676164, 3002441970, 2935068969, 1610329529, 1904580351, 3219988740,
    2337695268, 263015313, 2120931855, 131203777, 3818546915, 19163115, 3479924161, 2154860703,
    1790169326, 516580487, 2414737634, 909025701, 2241053595, 1237268359, 3797503938, 1773623028,
    2840671725, 2299292186, 1933596460, 2279513616, 514132674, 3245155609, 1753922983, 2241450350,
    2449659630, 262239956, 773552098, 3253131632, 3863807927, 879696536, 3143654396, 3973063648,
    509015903, 270850193, 1893431553, 719566283, 2310657204, 365781698, 3761063438, 1007484868
  };

  assert(W == expected_W);
}

void test_round() {
  auto input_state = vector<uint32_t>{2739944672, 3126690193, 4191866847, 1163785745, 3714074692, 1172792371, 283469062, 826169706};
  auto state = round(input_state, 961987163, 3221900128);
  auto expected_state = vector<uint32_t>{1724514418, 2739944672, 3126690193, 4191866847, 1638715774, 3714074692, 1172792371, 283469062};

  assert(state == expected_state);
}

int main() {
  test_schedule();
  test_round();
}