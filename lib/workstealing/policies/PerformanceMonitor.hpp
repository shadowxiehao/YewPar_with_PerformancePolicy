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
                double workRateAverage = WORK_RATE_INIT_VALUE;
                unsigned tasksCount = 0;
                double averageDelayTime = 1;
            };
            hpx::chrono::high_resolution_timer timer;
            
            std::vector<NodeInfo> nodeInfoVector;
            std::vector<std::unique_ptr<hpx::mutex>> nodeInfoVectorMutexs;

            hpx::id_type top_steal_id_type;
            hpx::spinlock top_steal_id_type_mutex_;

            hpx::spinlock top_add_id_type_mutex_;
            hpx::id_type top_add_id_type;
            unsigned top_add_num = 0;

            hpx::mutex refresh_top_id_state_mutex;
            bool refresh_top_id_state = false;

            void addNodeInfo(const hpx::id_type& nodeId);

            //compute
            //void refreshSortedIds();

            //init
		    void generateNodeInfoVector();
            void generateChannels();


            //---get info---
            hpx::execution::experimental::task_group infoTaskGroup;

            //hpx::execution::parallel_executor top_priority_executor = hpx::execution::parallel_executor(hpx::threads::thread_priority::normal);

            void task_group_run_with_executor(hpx::execution::experimental::task_group& tg, hpx::execution::parallel_executor& executor, std::function<void()> func) {
                tg.run([&](){
                    return hpx::async(executor, func).get();
                });
            }

            //refresh data
            mutable hpx::mutex refreshMutex;
            bool refreshRunning = false;
            mutable hpx::mutex refreshGetTargetMutex;
            bool refreshGetTarget = false;
            bool autoRefreshInfo();
            void refreshTopWorthStealId();
            //void sendWorthStealToOther();

            void refreshNetworkTime();
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
            void refreshTopWorthAddId();
            hpx::id_type getTopWorthAddId();
            static int randomIntFrom0ToN(int n1, int n2);
            /*std::vector<hpx::id_type> getTopNIdsWithoutLocal(std::uint32_t n);
            hpx::id_type getTopIdWithoutLocal();
            std::vector<hpx::id_type> getTopNIds(std::uint32_t n);
            hpx::id_type getTopId();*/
        };

	}
}


#endif
