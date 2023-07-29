#include "workstealing/policies/SearchManager.hpp"
#include "workstealing/policies/Workpool.hpp"
#include "workstealing/policies/PriorityOrdered.hpp"
//#include "workstealing/policies/DepthPoolPolicy.hpp"
#include "workstealing/policies/PerformancePolicy.hpp"

namespace YewPar {

void registerPerformanceCounters() {
  hpx::register_startup_function(&Workstealing::Policies::SearchManagerPerf::registerPerformanceCounters);
  hpx::register_startup_function(&Workstealing::Policies::WorkpoolPerf::registerPerformanceCounters);
  hpx::register_startup_function(&Workstealing::Policies::PriorityOrderedPerf::registerPerformanceCounters);
  //hpx::register_startup_function(&Workstealing::Policies::DepthPoolPolicyPerf::registerPerformanceCounters);
  hpx::register_startup_function(&Workstealing::Policies::PerformancePolicyPerf::registerPerformanceCounters);
}

}

