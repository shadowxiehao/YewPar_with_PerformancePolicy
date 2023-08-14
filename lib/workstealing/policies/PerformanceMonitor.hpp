#ifndef Performance_Moitor_HPP
#define Performance_Moitor_HPP

#include "workstealing/channels/SchedulerChannels.hpp"
#include "workstealing/Scheduler.hpp"

#include <hpx/execution.hpp>
#include <hpx/iostream.hpp>
#include <atomic>

#include "workstealing/DepthPool.hpp"

namespace Workstealing
{
	namespace Policies
	{
		class PerformanceMonitor{

		private:
            std::vector<hpx::id_type> workpools;

			//node info
            struct NodeInfo {
                hpx::id_type id = {};
                std::atomic<double> workRateAverage = WORK_RATE_INIT_VALUE;
                std::atomic<unsigned> tasksCount = 0;
                std::atomic<double> averageDelayTime = 0;
            };
            
            std::vector<std::unique_ptr<NodeInfo>> nodeInfoVector;

            //hpx::mutex refresh_top_id_state_mutex;
            std::atomic<bool> refresh_top_id_state = false;

            workstealing::DepthPool::getWorkRate_action getWorkRate_action;
            workstealing::DepthPool::getTasksCount_action getTasksCount_action;
            workstealing::DepthPool::setWorkRate_action setWorkRate_action;

            //compute
            void refreshTopWorthStealId();
            std::atomic<unsigned> top_steal_id_num;
            //void refreshTopWorthAddId();
            //hpx::id_type getTopWorthAddId();

            //init
		    void generateNodeInfoVector();
            void generateChannels();


            //---get info---

            //refresh data
            std::atomic<bool> refreshRunning = false;
            std::atomic<bool> refreshGetTarget = false;

            bool autoRefreshInfo();

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

            //tool
            static int randomIntFrom0ToN(int n1, int n2);
        };
	}
}

#endif
