#include "PerformancePolicy.hpp"

#include <hpx/functional/function.hpp>
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>

#include <memory>
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
            last_remote = hpx::find_here();

            std::random_device rd;
            randGenerator.seed(rd());
        }


        hpx::function<void(), false> PerformancePolicy::getWork() {

            //std::unique_lock<mutex_t> l(mtx);

            hpx::distributed::function<void(hpx::id_type)> task;
            task = hpx::async<workstealing::DepthPool::getLocal_action>(local_workpool).get();

            if (task) {
                {
                    std::unique_lock<mutex_t> l(mtx);
                    PerformancePolicyPerf::perf_localSteals++;
                }
                return hpx::bind(task, hpx::find_here());
            }
            else {
                {
                    std::unique_lock<mutex_t> l(mtx);
                    PerformancePolicyPerf::perf_failedLocalSteals++;
                }
            }

            if (!distributed_workpools.empty()) {

                // Use a load balancing algorithm to select a node to steal from
                auto victim = performanceMonitor.getTopWorthStealId();

                task = hpx::async<workstealing::DepthPool::steal_action>(victim).get();

                if (task) {
                    {
                        std::unique_lock<mutex_t> l(mtx);
                        PerformancePolicyPerf::perf_distributedSteals++;
                    }
                    return hpx::bind(task, hpx::find_here());
                }
                else {
                    {
                        std::unique_lock<mutex_t> l(mtx);
                        performanceMonitor.refreshInfo();
                        PerformancePolicyPerf::perf_failedDistributedSteals++;
                    }
                }
            }

            return nullptr;
        }

        void PerformancePolicy::addwork(hpx::distributed::function<void(hpx::id_type)> task, unsigned depth) {
            {
                std::unique_lock<mutex_t> l(mtx);
                PerformancePolicyPerf::perf_spawns++;
            }
            hpx::apply<workstealing::DepthPool::addWork_action>(local_workpool, task, depth);
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
