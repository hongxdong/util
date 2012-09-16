
#include "Process.h"
namespace util {

pid_t Process::GetPid() {
  return getpid();
}

std::string Process::GetProcessFilePath() {
  char file_path[FILENAME_LENGTH];
  sprintf(file_path, "/proc/%d/exe", getpid());
  file_path[readlink(file_path, file_path, FILENAME_LENGTH - 1)] = '\0';
  return file_path;
}

std::string Process::GetProcessFileDirectory() {
  char file_path[FILENAME_LENGTH];
  sprintf(file_path, "/proc/%d/exe", getpid());
  file_path[readlink(file_path, file_path, FILENAME_LENGTH - 1)] = '\0';
  strrchr(file_path, '/')[1] = '\0';
  return file_path;
}

}
