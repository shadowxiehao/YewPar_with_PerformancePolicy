#ifndef YEWPAR_SCHEDULER_HPP
#define YEWPAR_SCHEDULER_HPP

#include <atomic>

#include <hpx/modules/actions_base.hpp>
#include <hpx/synchronization/mutex.hpp>
#include <hpx/synchronization/condition_variable.hpp>
#include <hpx/iostream.hpp>

#include "policies/Policy.hpp"
#include "workstealing/channels/SchedulerChannels.hpp"

namespace Workstealing {
    extern std::shared_ptr<SchedulerChannels> schedulerChannelHolder;

    namespace Scheduler {

// Used to stop all schedulers
extern std::atomic<bool> running;

extern hpx::mutex mtx;
extern hpx::condition_variable exit_cv;
extern unsigned numRunningSchedulers;
//used to lock others when scheduler running main tasks
extern hpx::mutex schedulerTaskRunningMutex;

// Implementation policy
extern std::shared_ptr<Policy> local_policy;

extern hpx::execution::parallel_executor exe;

void stopSchedulers();
HPX_DEFINE_PLAIN_ACTION(stopSchedulers, stopSchedulers_act);

void scheduler(hpx::function<void(), false> initialTask);

// Start "n" uninitialised schedulers
void startSchedulers(unsigned n);
HPX_DEFINE_PLAIN_ACTION(startSchedulers, startSchedulers_act);

}} // Workstealing::Scheduler


#endif //YEWPAR_SCHEDULER_HPP
