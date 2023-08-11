#include "Scheduler.hpp"
#include "ExponentialBackoff.hpp"

#include <hpx/execution.hpp>


namespace Workstealing {

    namespace Scheduler {


        // Used to stop all schedulers
        std::atomic<bool> running(true);

        hpx::mutex mtx;
        hpx::condition_variable exit_cv;
        unsigned numRunningSchedulers;

        hpx::mutex schedulerTaskRunningMutex;

        // Implementation policy
        std::shared_ptr<Policy> local_policy;

void scheduler(hpx::function<void(), false> initialTask) {
  workstealing::ExponentialBackoff backoff;

  if (!local_policy) {
    std::cerr << "No local policy set when calling scheduler. Returning\n";
    return;
  }

  unsigned local_id_num;

  {
    std::unique_lock l(mtx);
    local_id_num = numRunningSchedulers;

    ++numRunningSchedulers;
  }

  // If we are pre-initialised then run that task first then enter the scheduler in this thread
  if (initialTask) {
    initialTask();
  }

  schedulerChannelHolder->setState(local_id_num, ThreadState::IDLE);

  for (;;) {
    if (!running) {
      break;
    }

    auto task = local_policy->getWork();

    if (task) {
      backoff.reset();

      schedulerChannelHolder->setState(local_id_num, ThreadState::WORKING);
      task();
      schedulerChannelHolder->setState(local_id_num, ThreadState::IDLE);

      /*std::string msg = hpx::get_locality_name() + "_" + "getRate_" + std::to_string(schedulerChannelHolder->getWorkRate(local_id_num))+"\n";
      hpx::cout << msg << std::flush;*/

    } else {
      backoff.failed();
      hpx::this_thread::suspend(backoff.getSleepTime());
    }
  }
  schedulerChannelHolder->setState(local_id_num, ThreadState::Dead);
  {
    // Signal exit
    std::unique_lock l(mtx);
    numRunningSchedulers--;
    exit_cv.notify_all();
  }
}

void stopSchedulers() {

  running.store(false);
  {
    // Block until all schedulers have finished
    std::unique_lock l(mtx);
    while (numRunningSchedulers > 0) {
      exit_cv.wait(l);
    }
  }
}

void startSchedulers(unsigned n) {

    //numRunningSchedulers = 0;

  //hpx::execution::parallel_executor exe(hpx::threads::thread_priority::critical,
  //                                      hpx::threads::thread_stacksize::huge);

  hpx::execution::parallel_executor exe(hpx::threads::thread_priority::high_recursive,
      hpx::threads::thread_stacksize::huge);

  for (auto i = 0; i < n; ++i) {
    //exe.add(hpx::bind(&scheduler, nullptr));
    hpx::async(exe, &scheduler, nullptr);
  }
}

}}
