#include <iostream>
#include <string>
#include <bitset>
#include <vector>

using namespace std;

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

int main() {
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

  for (int i = 0; i < 64; i++) {
    assert(W[i] == expected_W[i]);
  }
}