#ifndef Performance_Moitor_HPP
#define Performance_Moitor_HPP

#include "workstealing/channels/SchedulerChannels.hpp"
#include "workstealing/Scheduler.hpp"

#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/include/components.hpp>
#include <hpx/parallel/task_group.hpp>
#include <hpx/execution.hpp>
#include <hpx/iostream.hpp>
#include <atomic>
#include <hpx/include/unordered_map.hpp>

namespace Workstealing
{

	namespace Policies
	{

		class PerformanceMonitor{

		private:

			//node id
			/*hpx::id_type local_workpool = {};
			std::vector<hpx::id_type> distributed_workpools = {};*/
            std::vector<hpx::id_type> workpools;

			//node info
            struct NodeInfo {
                hpx::id_type id = {};
                double workRateAverage = {};
                unsigned tasksCount = 0;
            };

            struct CompareNodeInfo {
                bool operator()(const NodeInfo& a,
                                const NodeInfo& b) const {
                // Compare a and b here
                    double aResult = a.workRateAverage * a.tasksCount;
                    double bResult = b.workRateAverage * b.tasksCount;

                    return aResult > bResult;
                }
            };
            
            std::vector<NodeInfo> nodeInfoVector;
            std::vector<std::unique_ptr<hpx::mutex>> nodeInfoVectorMutexs;

            hpx::id_type top_id_type;
            hpx::mutex top_id_type_mutex_;

            void addNodeInfo(const hpx::id_type& nodeId);

            //compute
            //void refreshSortedIds();

            //init
		    void generateNodeInfoVector();
            void generateChannels();


            //---get info---
            hpx::execution::experimental::task_group infoTaskGroup;

            hpx::execution::parallel_executor top_priority_executor = hpx::execution::parallel_executor(hpx::threads::thread_priority::bound);
            hpx::execution::parallel_executor normal_priority_executor = hpx::execution::parallel_executor(hpx::threads::thread_priority::normal);

            void task_group_run_with_executor(hpx::execution::experimental::task_group& tg, hpx::execution::parallel_executor& executor, std::function<void()> func) {
                tg.run([&](){
                    return hpx::async(executor, func).get();
                });
            }

            //refresh data
            mutable hpx::mutex refreshMutex;
            bool refreshRunning = false;
            bool autoRefreshInfo();
            void refreshTopWorthStealId();
            //void sendWorthStealToOther();

            void refreshCpuLoad();
            void refreshSchedularInfo();
            void refreshPoolTasksCountInfo();

        public:
            PerformanceMonitor() = default;

            // init
            void start();

            void init(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools);

            //get ids
            bool refreshInfo();
            hpx::id_type getTopWorthStealId();
            /*std::vector<hpx::id_type> getTopNIdsWithoutLocal(std::uint32_t n);
            hpx::id_type getTopIdWithoutLocal();
            std::vector<hpx::id_type> getTopNIds(std::uint32_t n);
            hpx::id_type getTopId();*/
        };

	}
}


#endif
