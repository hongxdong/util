
#include <stdint.h>

namespace util {

  static long holdrand = 1L;

  inline void SeedFakeRand (unsigned int seed) {
    holdrand = (long)seed;
  }

  int FakeRand() {
    return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
  }

};

