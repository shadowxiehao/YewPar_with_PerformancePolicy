#ifndef Performance_Moitor_HPP
#define Performance_Moitor_HPP

//#include "workstealing/channels/SchedulerChannels.hpp"
#include "workstealing/Scheduler.hpp"

#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/include/components.hpp>
#include <hpx/parallel/task_group.hpp>
#include <hpx/execution.hpp>
#include <hpx/iostream.hpp>
#include <atomic>

namespace Workstealing
{
    namespace Scheduler {
        //extern std::shared_ptr<SchedulerChannelHolder> schedulerChannelHolder;
    }
	namespace Policies
	{
		//namespace PerformancePolicyPerf
		//{
		//	void registerPerformanceCounters();
		//}


		class PerformanceMonitor{

		private:
            //channels
            //hpx::components::client<SchedulerChannelHolder> schedulerChannelHolder;

			//node id
			hpx::id_type local_workpool;
			std::vector<hpx::id_type> distributed_workpools;

			//node info
            struct NodeInfo {
                hpx::id_type id;
                double cpuLoad;
                
                // Add other information here
            };

            hpx::mutex nodeInfoVectorMutex;
            hpx::mutex refreshMutex;

            struct CompareNodeInfo {
                bool operator()(const NodeInfo& a,
                                const NodeInfo& b) const {
                // Compare a and b here
                // For example, sort by CPU load
                return a.cpuLoad < b.cpuLoad;
                }
            };
            
            std::vector<NodeInfo> nodeInfoVector;

            void addNodeInfo(const hpx::id_type& nodeId);
            std::vector<NodeInfo> getAllNodeInfo() {
                std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);
                return nodeInfoVector;
            }
            void updateNodeCpuLoad(const hpx::id_type& nodeId, double load);
            bool nodeExists(const hpx::id_type& nodeId);
            hpx::id_type getTopId();

            //compute
            void compareNode();

            //init
		    void generateNodeInfoVector();
            void generateChannels();


            //---get info---
            hpx::execution::experimental::task_group infoTasks;

            hpx::execution::parallel_executor top_priority_executor = hpx::execution::parallel_executor(hpx::threads::thread_priority::bound);

            void task_group_run_with_executor(hpx::execution::experimental::task_group& tg, hpx::execution::parallel_executor& executor, std::function<void()> func) {
                tg.run([&](){
                    return hpx::async(executor, func).get();
                });
            }

            //refresh data
            bool refreshInfo();
            bool autoRefreshInfo();

            void refreshCpuLoad();
            void refreshSchedularInfo();


		public:
            PerformanceMonitor() = default;
			//PerformanceMonitor(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools);

            //std::atomic<bool> performanceMonitorRunning;

            // init

            void start() {
                hpx::async(top_priority_executor, [&]() { autoRefreshInfo(); });
            }

            void init(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools) {
                this->local_workpool = local_workpool;
                this->distributed_workpools = distributed_workpools;
                generateChannels();
                generateNodeInfoVector();
                start();
            }

            //void stop() {
            //    performanceMonitorRunning.store(false);
            //    //hpx::cout << "stopSchedulers2:" << hpx::get_locality_name()<< performanceMonitorRunning << std::endl;
            //}

            //get ids
            hpx::id_type getTopWorthStealId();
            
            //channel
        };

	}
}


#endif
