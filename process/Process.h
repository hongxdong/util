
#ifndef _UTIL_PROCESS_H_
#define _UTIL_PROCESS_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <string>

#define FILENAME_LENGTH 256

namespace util {

class Process {
 public:
  /// @brief 获得进程号pid
  static pid_t GetPid();

  /// @brief 获得进程的可执行文件的绝对路径(包含文件名)
  static std::string GetProcessFilePath();

  /// @brief 获得进程的可执行文件的绝对路径(不包含文件名)
  static std::string GetProcessFileDirectory();

};

}

#endif  /* _UTIL_PROCESS_H_ */

