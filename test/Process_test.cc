
#include "../Process.h"
#include <iostream>

int main() {
  std::cout << "ProcessId:\t" << util::Process::GetPid() << "\n";
  std::cout << "ProcessFilePath:\t" << util::Process::GetProcessFilePath() << "\n";
  std::cout << "ProcessFileDirectory:\t" <<util::Process::GetProcessFileDirectory() << "\n";
}

