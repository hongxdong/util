
#include "FakeRand.h"
#include <stdio.h>

int main() {
  util::SeedFakeRand(12);

  for (int i = 0; i < 10; ++i) {
    printf("rand: %d\n", util::FakeRand());
  }
}

