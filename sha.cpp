#include <iostream>
#include <string>
#include <bitset>

using namespace std;

uint32_t rightrotate32(uint32_t n, int i) {
  i %= 32;
  auto rot = n >> i;
  return rot | (n << (32 - i)); 
}


int main() {
  uint32_t n = 9;
  n = rightrotate32(n, 2);
  bitset<32> s(n);
  cout << s << endl;
}