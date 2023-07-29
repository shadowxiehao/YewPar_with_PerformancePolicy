#include "Scheduler.hpp"
#include "ExponentialBackoff.hpp"

#include <hpx/execution.hpp>


namespace Workstealing { namespace Scheduler {

void scheduler(hpx::function<void(), false> initialTask) {
  workstealing::ExponentialBackoff backoff;

  if (!local_policy) {
    std::cerr << "No local policy set when calling scheduler. Returning\n";
    return;
  }

  {
    std::unique_lock<hpx::mutex> l(mtx);
    numRunningSchedulers++;
  }

  // If we are pre-initialised then run that task first then enter the scheduler in this thread
  if (initialTask) {
    initialTask();
  }

  for (;;) {
    if (!running) {
      break;
    }

    auto task = local_policy->getWork();

    if (task) {
      backoff.reset();
      task();
    } else {
      backoff.failed();
      hpx::this_thread::suspend(backoff.getSleepTime());
    }
  }

  {
    // Signal exit
    std::unique_lock<hpx::mutex> l(mtx);
    numRunningSchedulers--;
    exit_cv.notify_all();
  }
}

void stopSchedulers() {

    local_policy->stopPerformanceMonitor();
    hpx::cout << "stopSchedulers:" << hpx::get_locality_name() << std::endl;

  running.store(false);
  {
    // Block until all schedulers have finished
    std::unique_lock<hpx::mutex> l(mtx);
    while (numRunningSchedulers > 0) {
      exit_cv.wait(l);
    }
  }
}

void startSchedulers(unsigned n) {

    local_policy->startPerformanceMonitor();
    hpx::cout << "startSchedulers:" << hpx::get_locality_name() << std::endl;

  hpx::execution::parallel_executor exe(hpx::threads::thread_priority::critical,
                                        hpx::threads::thread_stacksize::huge);

  for (auto i = 0; i < n; ++i) {
    //exe.add(hpx::bind(&scheduler, nullptr));
    hpx::async(exe, &scheduler, nullptr);
  }
}

}}
