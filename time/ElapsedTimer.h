/**
 * @version 1.0
 * @author  hongxiaodong
 * @date    2012-10-08
 *
 * @brief   计时器
 * Change Log:
 */

#include <stdint.h>

#define INIT_TIMER \
  ElapsedTimer __timer;\
  __timer.Start();

#define PRINT_TIMER \
  printf("%s time: %d us\n", __func__, (int)__timer.ElapsedMicroSeconds());

namespace util {

class ElapsedTimer {
 public:
  ElapsedTimer();

  /// @brief 启动计时器。
  void Start();

  /// @brief 重新启动计时器，并获得计时时间。
  /// @return 计时时间。单位毫秒(整数)。
  int64_t Restart();

  /// @brief  获得计时时间。
  /// @return 计时时间。单位毫秒。(整数表示)
  int64_t Elapsed();
 
  /// @brief  获得计时时间。
  /// @return 计时时间。单位微秒。(小数表示)
  double ElapsedMicroSeconds();
 
 private:
  /// @brief 内部调用gettimeofday 获得系统当前时间。
  /// @param sec[out] 秒
  /// @param frac[out] 与系统平台有关的时间，这里 frac/1000 = 毫秒
  void DoGetTime(int64_t * sec, int64_t * frac);

  /// @brief 内部计算时间差的逻辑。返回的是毫秒。（整数表示）
  int64_t ElapsedAndRestart(int64_t sec, int64_t frac,
                            int64_t * nowsec, int64_t * nowfrac);

  /// @brief 内部计算时间差的逻辑。返回的是微秒。（小数表示）
  double ElapsedAndRestartMicroSeconds(int64_t sec, int64_t frac,
                                       int64_t * nowsec, int64_t * nowfrac);

 private:
  int64_t sec_;
  int64_t frac_;
};

}

/*

Example: 

#include <stdio.h>
#include <unistd.h>
#include "./elapsed_timer.h"

int main() {
  ElapsedTimer * timer = new ElapsedTimer();
  timer->Start();
  for (int i = 0; i < 10000000; i++) {
    // do something.
  }
  int64_t elapsed_time_1 = timer->Restart();
  printf("Elapsed time 1: %ld ms.\n", elapsed_time_1);

  sleep(1);
  int64_t elapsed_time_2 = timer->Elapsed();
  printf("Elapsed time 2: %ld ms.\n", elapsed_time_2);

}

Output:
Elapsed time 1: 31 ms.
Elapsed time 2: 1000 ms.

*/

