/**
 * @version 1.0
 * @author  hongxiaodong
 * @date    2012-10-08
 *
 * @brief   计时器
 * Change Log:
 */

#include "./elapsed_timer.h"
#include <time.h>
#include <sys/time.h>

namespace util {

ElapsedTimer::ElapsedTimer()
  :sec_(0)
  ,frac_(0)
{ }

void ElapsedTimer::Start() {
  this->DoGetTime(&sec_, &frac_);
}

int64_t ElapsedTimer::Restart() {
  return this->ElapsedAndRestart(sec_, frac_, &sec_, &frac_);
}

int64_t ElapsedTimer::Elapsed() {
  int64_t sec_tmp, frac_tmp;
  return this->ElapsedAndRestart(sec_, frac_, &sec_tmp, &frac_tmp);
}

double ElapsedTimer::ElapsedMicroSeconds() {
  int64_t sec_tmp, frac_tmp;
  return this->ElapsedAndRestartMicroSeconds(sec_, frac_, &sec_tmp, &frac_tmp);
}

inline void ElapsedTimer::DoGetTime(int64_t * sec, int64_t * frac) {
  timeval tv;
  ::gettimeofday(&tv, 0);
  *sec  = tv.tv_sec;
  *frac = tv.tv_usec;
}

int64_t ElapsedTimer::ElapsedAndRestart(int64_t sec, int64_t frac,
                                        int64_t * nowsec, int64_t * nowfrac) {
  this->DoGetTime(nowsec, nowfrac);
  sec  = *nowsec  - sec;
  frac = *nowfrac - frac;
  return sec * 1000 + frac / 1000;
}

double ElapsedTimer::ElapsedAndRestartMicroSeconds(int64_t sec, int64_t frac,
                                        int64_t * nowsec, int64_t * nowfrac) {
  this->DoGetTime(nowsec, nowfrac);
  sec  = *nowsec  - sec;
  frac = *nowfrac - frac;
  return ((double)sec) * 1000000.0 + ((double)frac);
}

}

