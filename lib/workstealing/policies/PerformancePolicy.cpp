#include "PerformancePolicy.hpp"

#include <hpx/functional/function.hpp>
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>

#include <hpx/iostream.hpp>

#include "util/util.hpp"

namespace Workstealing {
    namespace Policies {

        namespace PerformancePolicyPerf {

            std::atomic<std::uint64_t> perf_spawns(0);
            std::atomic<std::uint64_t> perf_localSteals(0);
            std::atomic<std::uint64_t> perf_distributedSteals(0);
            std::atomic<std::uint64_t> perf_failedLocalSteals(0);
            std::atomic<std::uint64_t> perf_failedDistributedSteals(0);

            std::uint64_t get_and_reset(std::atomic<std::uint64_t>& cntr, bool reset) {
                auto res = cntr.load();
                if (reset) { cntr = 0; }
                return res;
            }

            std::uint64_t getSpawns(bool reset) { return get_and_reset(perf_spawns, reset); }
            std::uint64_t getLocalSteals(bool reset) { return get_and_reset(perf_localSteals, reset); }
            std::uint64_t getDistributedSteals(bool reset) { return get_and_reset(perf_distributedSteals, reset); }
            std::uint64_t getFailedLocalSteals(bool reset) { return get_and_reset(perf_failedLocalSteals, reset); }
            std::uint64_t getFailedDistributedSteals(bool reset) { return get_and_reset(perf_failedDistributedSteals, reset); }

            void registerPerformanceCounters() {
                hpx::performance_counters::install_counter_type(
                    "/workstealing/depthpool/spawns",
                    &getSpawns,
                    "Returns the number of tasks spawned on this locality"
                );

                hpx::performance_counters::install_counter_type(
                    "/workstealing/depthpool/localSteals",
                    &getLocalSteals,
                    "Returns the number of tasks stolen from another thread on the same locality"
                );

                hpx::performance_counters::install_counter_type(
                    "/workstealing/depthpool/distributedSteals",
                    &getDistributedSteals,
                    "Returns the number of tasks stolen from another thread on another locality"
                );

                hpx::performance_counters::install_counter_type(
                    "/workstealing/depthpool/localFailedSteals",
                    &getFailedLocalSteals,
                    "Returns the number of failed steals from this locality "
                );

                hpx::performance_counters::install_counter_type(
                    "/workstealing/depthpool/distributedFailedSteals",
                    &getFailedDistributedSteals,
                    "Returns the number of failed steals from another locality "
                );
            }

        }

        PerformancePolicy::PerformancePolicy(hpx::id_type workpool) {
            local_workpool = workpool;
        }


        hpx::function<void(), false> PerformancePolicy::getWork() {

            hpx::distributed::function<void(hpx::id_type)> task;
            task = getLocal_action(local_workpool);

            if (task) {
                PerformancePolicyPerf::perf_localSteals++;
                return hpx::bind(task, hpx::find_here());
            }
            else {
                PerformancePolicyPerf::perf_failedLocalSteals++;
            }

            if (!distributed_workpools.empty()) {
                hpx::id_type victim;
                    victim = performanceMonitor.getTopWorthStealId();

                task = steal_action(victim);

                if (task) {
                    PerformancePolicyPerf::perf_distributedSteals++;
                    return hpx::bind(task, hpx::find_here());
                }
                else {
                    performanceMonitor.refreshInfo();
                    auto victim = performanceMonitor.getTopWorthStealId();
                    task = steal_action(victim);
                    if (task) {
                        PerformancePolicyPerf::perf_distributedSteals++;
                        return hpx::bind(task, hpx::find_here());
                    }else {
                        PerformancePolicyPerf::perf_failedDistributedSteals++;
                    }
                }
            }
            return nullptr;
        }

        void PerformancePolicy::addwork(hpx::distributed::function<void(hpx::id_type)> task, unsigned depth) {
            PerformancePolicyPerf::perf_spawns++;
            //hpx::id_type target = performanceMonitor.getTopWorthAddId();
            /*if(target!=local_workpool) {
                performanceMonitor.refreshTopWorthAddId();
            }*/
            //hpx::apply<workstealing::DepthPool::addWork_action>(local_workpool, task, depth);
            addWork_action(local_workpool, task, depth);
        }

        void PerformancePolicy::registerDistributedDepthPools(std::vector<hpx::id_type> workpools) {
            std::unique_lock<mutex_t> l(mtx);
            distributed_workpools = workpools;
            distributed_workpools.erase(
                std::remove_if(distributed_workpools.begin(), distributed_workpools.end(), YewPar::util::isColocated),
                distributed_workpools.end());
        }

    }
}
