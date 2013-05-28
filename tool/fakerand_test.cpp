
#include "FakeRand.h"
#include <stdio.h>
#include <map>

int main() {
  util::SeedFakeRand(12);

  for (int i = 0; i < 10; ++i) {
    printf("rand: %d\n", util::FakeRand());
  }

  std::map<int, int> statistic;
  for (int i = 0; i < 1000000; ++i) {
    int rand = util::FakeRand() % 2;
    statistic[rand] = statistic[rand] + 1;
  }
  printf("0: %d\n", statistic[0]);
  printf("1: %d\n", statistic[1]);
}

