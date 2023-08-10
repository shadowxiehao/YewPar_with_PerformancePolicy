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
			hpx::id_type local_workpool = {};
			std::vector<hpx::id_type> distributed_workpools = {};

			//node info
            struct NodeInfo {
                hpx::id_type id = {};
                double workRateAverage = {};
            };

            struct CompareNodeInfo {
                bool operator()(const NodeInfo& a,
                                const NodeInfo& b) const {
                // Compare a and b here
                return a.workRateAverage > b.workRateAverage;
                }
            };
            
            std::vector<NodeInfo> nodeInfoVector;
            std::vector<std::unique_ptr<hpx::mutex>> nodeInfoVectorMutexs;

            std::vector<hpx::id_type> sortedIds;  // To store sorted hpx::id_type
            mutable hpx::mutex sortedIdsMutex;   // Mutex for sortedIds


            void addNodeInfo(const hpx::id_type& nodeId);
            std::vector<NodeInfo> getAllNodeInfo() {
                std::unique_lock<hpx::mutex> l(refreshMutex);
                return nodeInfoVector;
            }

            //compute
            void refreshSortedIds();

            //init
		    void generateNodeInfoVector();
            void generateChannels();


            //---get info---
            hpx::execution::experimental::task_group infoTaskGroup;

            hpx::execution::parallel_executor top_priority_executor = hpx::execution::parallel_executor(hpx::threads::thread_priority::bound);

            void task_group_run_with_executor(hpx::execution::experimental::task_group& tg, hpx::execution::parallel_executor& executor, std::function<void()> func) {
                tg.run([&](){
                    return hpx::async(executor, func).get();
                });
            }

            //refresh data
            bool refreshInfo();
            mutable hpx::mutex refreshMutex;
            bool autoRefreshInfo();
            void sendWorthStealToOther();

            void refreshCpuLoad();
            void refreshSchedularInfo();


		public:
            PerformanceMonitor() = default;

            // init
            void start();

            void init(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools);

            //get ids
            hpx::id_type getTopWorthStealId();
            std::vector<hpx::id_type> getTopNIdsWithoutLocal(std::uint32_t n);
            hpx::id_type getTopIdWithoutLocal();
            std::vector<hpx::id_type> getTopNIds(std::uint32_t n);
            hpx::id_type getTopId();
        };

	}
}


#endif
