#include "PerformanceMonitor.hpp"
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>
#include <hpx/parallel/algorithms/for_loop.hpp>
#include <hpx/include/threads.hpp>
#include <hpx/include/util.hpp>

#include "workstealing/DepthPool.hpp"


namespace Workstealing {
        //channel
        std::shared_ptr<SchedulerChannels> schedulerChannelHolder;

    namespace Policies {

        //====================== init ======================

        void PerformanceMonitor::init(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools) {
            
            workpools.reserve(locality_count);
            workpools.push_back(local_workpool);
            for(unsigned i=0;i<distributed_workpools.size();++i) {
                workpools.push_back(distributed_workpools[i]);
            }
            local_id_num = hpx::get_locality_id();
            local_id_type = local_workpool;
            locality_count = hpx::find_all_localities().size();
            thread_count = hpx::get_os_thread_count();

            top_steal_id_num = randomIntFrom0ToN(0, locality_count - 1);
            
            /*{
                std::string message =
                    hpx::get_locality_name()
                    + "init_local_id_num:" + std::to_string(local_id_num)
                    + "_locality_count:" + std::to_string(locality_count)
                    + "_thread_count:" + std::to_string(thread_count) + "\n";
                hpx::cout << message << std::flush;
            }*/
            generateNodeInfoVector();
            generateChannels();
            start();
        }

        void PerformanceMonitor::generateNodeInfoVector() {
            
            //add nodes
            nodeInfoVector.reserve(locality_count);
            for(int i=0;i<locality_count;++i) {
                nodeInfoVector.push_back(std::make_unique<NodeInfo>());
            }
            //sortedIds.resize(locality_count);
            for (hpx::id_type nodeId : workpools) {
                std::uint32_t nodeId_num = hpx::naming::get_locality_id_from_id(nodeId);
                nodeInfoVector[nodeId_num]->id = nodeId;
            }
        }

        void PerformanceMonitor::generateChannels() {
            schedulerChannelHolder = std::make_shared<Workstealing::SchedulerChannels>() ;
            schedulerChannelHolder->init();
        }

        void PerformanceMonitor::start() {
            hpx::async(Scheduler::exe, [&]() { autoRefreshInfo(); });
        }
        //====================== get info ======================

        void PerformanceMonitor::refreshSchedularInfo() {
            // get local WorkRateAverage and send it to global
            setWorkRate_action(local_id_type,schedulerChannelHolder->getWorkRateAverage());
            //get WorkRateAverage for local nodeInfoVector
            for (const std::unique_ptr<NodeInfo>& nodeInfo : nodeInfoVector) {
                hpx::id_type nodeID = nodeInfo->id;
                nodeInfo->workRateAverage = getWorkRate_action(nodeID);
            }
        }

        void PerformanceMonitor::refreshPoolTasksCountInfo() {
            hpx::chrono::high_resolution_timer timer;

            for(unsigned i=0;i<locality_count;++i){
                timer.restart();
                unsigned tasksCount = getTasksCount_action(nodeInfoVector[i]->id);
                double delayTime = timer.elapsed_microseconds();
                nodeInfoVector[i]->tasksCount = tasksCount;
                nodeInfoVector[i]->averageDelayTime = std::log(2.72 + delayTime * thread_count) * 0.65 + nodeInfoVector[i]->averageDelayTime * 0.35;
            }
        }

        bool PerformanceMonitor::refreshInfo() {

            refreshSchedularInfo();
            refreshPoolTasksCountInfo();
            refreshTopWorthStealId();
            //refreshTopWorthAddId();

            return true;
        }

        bool PerformanceMonitor::autoRefreshInfo() {
            const int maxRefreshWaitTime = 240;
            const int minRefreshWaitTime = 5;
            int refreshWaitTime = 1;
            //refresh automatically after a interval
            while (Scheduler::running){

                refreshInfo();
                
                bool flag = refreshGetTarget;

                if (flag == true) {
                    refreshWaitTime = std::min(refreshWaitTime + 60, maxRefreshWaitTime);
                }
                else {
                    refreshWaitTime = std::max(refreshWaitTime - 120, minRefreshWaitTime);
                }
                hpx::this_thread::sleep_for(std::chrono::milliseconds(refreshWaitTime));
            }

            return true;
        }


        //====================== node func ====================

        //get top id_type worth to steal
        void PerformanceMonitor::refreshTopWorthStealId() {

            unsigned result_id_type_num = top_steal_id_num;
            double best_score = std::numeric_limits<double>::min();
            /*double local_workRate = 0.0;
            unsigned local_tasksCount = 0;
            double local_averageParcelArrival = 0.0;*/

            for (unsigned i = 0; i < locality_count; ++i) {
                double score = std::max(static_cast<double>(nodeInfoVector[i]->workRateAverage), 0.0001) * nodeInfoVector[i]->tasksCount - nodeInfoVector[i]->averageDelayTime;
                if (score > best_score) {
                    best_score = score;
                    result_id_type_num = i;
                    /*local_workRate = nodeInfoVector[i]->workRateAverage;
                    local_tasksCount = nodeInfoVector[i]->tasksCount;
                    local_averageParcelArrival = nodeInfoVector[i]->averageDelayTime;*/
                }
            }
            if (best_score > 0) {
                {
                    top_steal_id_num = result_id_type_num;

                    refreshGetTarget = true;

                    /*std::string message =
                        hpx::get_locality_name()
                        + "stealCacheSet:" + std::to_string(top_steal_id_num)
                        + "_score:" + std::to_string(best_score)
                        + "_localWorkRate:" + std::to_string(local_workRate)
                        + "_averageParcelArrival:" + std::to_string(local_averageParcelArrival)
                        + "_taskCount::" + std::to_string(local_tasksCount) + "\n";
                    hpx::cout << message << std::flush;*/
                }
            }
            else {
                top_steal_id_num = randomIntFrom0ToN(0, locality_count - 1);

                refreshGetTarget = false;

                /*std::string message =
                    hpx::get_locality_name()
                    + "steal_no_target_fount" + "\n";
                hpx::cout << message << std::flush;*/
            }
        }

        //====================== get results ======================

        hpx::id_type PerformanceMonitor::getTopWorthStealId() {
            return nodeInfoVector[top_steal_id_num]->id;
        }

        //abort
        //void PerformanceMonitor::refreshTopWorthAddId() {
            //{
            //    std::unique_lock lrta(refresh_top_id_state_mutex);
            //    if(refresh_top_id_state==true) {
            //        return;
            //    }else {
            //        refresh_top_id_state = true;
            //    }
            //}
            ////hpx::spinlock resultMutex;
            //hpx::id_type result_id_type;
            //{
            //    std::unique_lock ld(top_steal_id_type_mutex_);
            //    result_id_type = top_steal_id_type;
            //}
            //double best_score = std::numeric_limits<double>::max();
            //double local_workRate = std::numeric_limits<double>::max();
            //unsigned local_tasksCount = std::numeric_limits<unsigned>::max();
            //double here_score = 0;
            //unsigned here_tasksCount = 0;
            //for (unsigned i = 0; i < locality_count; ++i) {
            //    std::unique_lock ln(*nodeInfoVectorMutexs[i]);
            //    double score = nodeInfoVector[i].workRateAverage * nodeInfoVector[i].tasksCount;
            //    if(i == local_id_num) {
            //        here_score = score;
            //        here_tasksCount = nodeInfoVector[i].tasksCount;
            //    }
            //    if (score < best_score) {
            //        best_score = score;
            //        result_id_type = nodeInfoVector[i].id;
            //        local_workRate = nodeInfoVector[i].workRateAverage;
            //        local_tasksCount = nodeInfoVector[i].tasksCount;
            //    }
            //}
            //if (here_score > 0 && best_score > 0 && best_score < std::numeric_limits<double>::max() && (best_score*100 < here_score) ) {
            //    {
            //        std::unique_lock ld(top_add_id_type_mutex_);
            //        top_add_id_type = result_id_type;
            //        top_add_num = static_cast<unsigned>( (here_tasksCount - local_tasksCount) * (best_score / here_score) );
            //        auto id_num = hpx::naming::get_locality_id_from_id(result_id_type);
            //        std::string message =
            //            hpx::get_locality_name()
            //            + "addCacheSet:" + std::to_string(id_num)
            //            + "_score:" + std::to_string(best_score)
            //            + "_taskCount::" + std::to_string(local_tasksCount)
            //            + "_WorkRate:" + std::to_string(local_workRate)
            //            + "_local_score:" + std::to_string(here_score) + "\n";
            //        hpx::cout << message << std::flush;
            //    }
            //}else {
            //    std::unique_lock ld(top_add_id_type_mutex_);
            //    top_add_id_type = workpools[0];
            //    top_add_num = 0;
            //    std::string message =
            //        hpx::get_locality_name()
            //        + "no_better_target_add" + "\n";
            //    hpx::cout << message << std::flush;
            //}
            //{
            //    std::unique_lock lrta(refresh_top_id_state_mutex);
            //    refresh_top_id_state = false;
            //}
        //}
        //hpx::id_type PerformanceMonitor::getTopWorthAddId() {
            //std::unique_lock lt(top_add_id_type_mutex_);
            //if(top_add_num>0) {
            //    --top_add_num;
            //    return top_add_id_type;
            //}
            //return workpools[0];
        //}

        //====================== tool ======================
        
        int PerformanceMonitor::randomIntFrom0ToN(int n1,int n2) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(n1, n2);

            return dis(gen);
        }

    }
}
