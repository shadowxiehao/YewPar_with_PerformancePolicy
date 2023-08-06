#ifndef YEWPAR_POLICY_HPP
#define YEWPAR_POLICY_HPP

#include <hpx/functional/function.hpp>

class Policy {
 public:
  // Scheduler hook point
  virtual hpx::function<void(), false> getWork() = 0;

  virtual void initPerformanceMonitor() {}
  virtual void startPerformanceMonitor() {}
  virtual void stopPerformanceMonitor() {}

};

#endif
