#ifndef YEWPAR_POLICY_DEPTHPOOL_HPP
#define YEWPAR_POLICY_DEPTHPOOL_HPP

#include "Policy.hpp"

#include "PerformanceMonitor.hpp"

#include <hpx/include/components.hpp>
#include <hpx/modules/collectives.hpp>
#include <hpx/naming_base/id_type.hpp>
#include <hpx/runtime_distributed/find_all_localities.hpp>

#include "../DepthPool.hpp"

#include <random>
#include <vector>
#include <hpx/iostream.hpp>

namespace Workstealing { namespace Scheduler { extern std::shared_ptr<Policy> local_policy; } }

namespace Workstealing {
    namespace Policies {

        namespace PerformancePolicyPerf {
            void registerPerformanceCounters();
        }


        // TODO: Performance counters
        class PerformancePolicy : public Policy {

        private:
            hpx::id_type local_workpool;
            hpx::id_type last_remote;
            std::vector<hpx::id_type> distributed_workpools;

            // random number generator
            std::mt19937 randGenerator;

            using mutex_t = hpx::mutex;
            mutex_t mtx;



        public:
            PerformancePolicy(hpx::id_type workpool);
            ~PerformancePolicy() = default;

            hpx::id_type selectVictimBasedOnLoad(const std::vector<std::pair<double, hpx::id_type>>& loads);

            hpx::function<void(), false> getWork() override;

            void addwork(hpx::distributed::function<void(hpx::id_type)> task, unsigned depth);

            void registerDistributedDepthPools(std::vector<hpx::id_type> workpools);

            static void setDepthPool(hpx::id_type localworkpool) {
              hpx::cout << "setDepthPool:"
                        << hpx::naming::get_locality_id_from_id(localworkpool)
                        << std::endl;
                Workstealing::Scheduler::local_policy = std::make_shared<PerformancePolicy>(localworkpool);
            }
            struct setDepthPool_act : hpx::actions::make_action<
                decltype(&PerformancePolicy::setDepthPool),
                &PerformancePolicy::setDepthPool,
                setDepthPool_act>::type {};

            static void setDistributedDepthPools(std::vector<hpx::id_type> workpools) {
                std::static_pointer_cast<Workstealing::Policies::PerformancePolicy>(Workstealing::Scheduler::local_policy)->registerDistributedDepthPools(workpools);
            }
            struct setDistributedDepthPools_act : hpx::actions::make_action<
                decltype(&PerformancePolicy::setDistributedDepthPools),
                &PerformancePolicy::setDistributedDepthPools,
                setDistributedDepthPools_act>::type {};

            PerformanceMonitor performanceMonitor = PerformanceMonitor(local_workpool, distributed_workpools);

            void initPerformanceMonitor() override{
                performanceMonitor.init();
            }
            void startPerformanceMonitor() override {
                performanceMonitor.start();
            }
            void stopPerformanceMonitor() override {
                performanceMonitor.stop();
            }

            static void setPerformanceMonitor() {
                auto performancePolicy = std::static_pointer_cast<
                    Workstealing::Policies::PerformancePolicy>(
                    Workstealing::Scheduler::local_policy);

                performancePolicy->initPerformanceMonitor();
            }

            struct setPerformanceMoitor_act
                : hpx::actions::make_action<
                      decltype(&PerformancePolicy::setPerformanceMonitor),
                      &PerformancePolicy::setPerformanceMonitor,
                      setPerformanceMoitor_act>::type {};

            static void initAllPerformanceMonitor() {
                hpx::wait_all(
                    hpx::lcos::broadcast<setPerformanceMoitor_act>(
                        hpx::find_all_localities()));
            }

            static void initPolicy() {
                std::vector<hpx::future<void> > futs;
                std::vector<hpx::id_type> pools;
                for (auto const& loc : hpx::find_all_localities()) {
                    hpx::cout << "initPolicy:"<< hpx::naming::get_locality_id_from_id(loc)<< std::endl;

                    auto depthpool = hpx::new_<workstealing::DepthPool>(loc).get();
                    futs.push_back(hpx::async<setDepthPool_act>(loc, depthpool));
                    pools.push_back(depthpool);
                }
                hpx::wait_all(futs);

                hpx::wait_all(hpx::lcos::broadcast<setDistributedDepthPools_act>(hpx::find_all_localities(), pools));

                initAllPerformanceMonitor();
            }
        };

    }
}

#endif
