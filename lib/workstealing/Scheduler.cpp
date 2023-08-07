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

        // Implementation policy
        std::shared_ptr<Policy> local_policy;

void scheduler(hpx::function<void(), false> initialTask) {
  workstealing::ExponentialBackoff backoff;

  if (!local_policy) {
    std::cerr << "No local policy set when calling scheduler. Returning\n";
    return;
  }

  //unsigned local_id_num;

  {
    std::unique_lock<hpx::mutex> l(mtx);
    //local_id_num = numRunningSchedulers;

    ++numRunningSchedulers;
  }

  // If we are pre-initialised then run that task first then enter the scheduler in this thread
  if (initialTask) {
    initialTask();
  }



  for (;;) {
    if (!running) {
      break;
    }
      //note State
    //schedulerChannelHolder->setState(local_id_num, ThreadState::WORKING);
    //std::string message = "setChannel_" + hpx::get_locality_name() + "-" + std::to_string(local_id_num)  + ":" + toString(schedulerChannelHolder->getState(local_id_num));
    //hpx::cout << message + "\n";

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

    //hpx::cout << "stopSchedulers:" << hpx::get_locality_name() << std::endl;

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

    //numRunningSchedulers = 0;

    //// Find the channel using the symbolic name
    //hpx::id_type id = hpx::find_from_basename("SchedulerChannel", 0).get();

    //hpx::cout << "startSchedulers:" << hpx::get_locality_name() << std::endl;

  hpx::execution::parallel_executor exe(hpx::threads::thread_priority::critical,
                                        hpx::threads::thread_stacksize::huge);

  for (auto i = 0; i < n; ++i) {
    //exe.add(hpx::bind(&scheduler, nullptr));
    hpx::async(exe, &scheduler, nullptr);
  }
}

}}
