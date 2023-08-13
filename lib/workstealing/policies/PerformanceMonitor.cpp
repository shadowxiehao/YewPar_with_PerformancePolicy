#include "PerformanceMonitor.hpp"
#include <hpx/functional/function.hpp>
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>
#include <hpx/parallel/algorithms/for_loop.hpp>

#include <memory>
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
            locality_count = hpx::find_all_localities().size();
            thread_count = hpx::get_os_thread_count();

            top_add_id_type = local_workpool;
            top_steal_id_type = local_workpool;
            {
                std::string message =
                    hpx::get_locality_name()
                    + "init_local_id_num:" + std::to_string(local_id_num)
                    + "_locality_count:" + std::to_string(locality_count)
                    + "_thread_count:" + std::to_string(thread_count) + "\n";
                hpx::cout << message << std::flush;
            }
            generateNodeInfoVector();
            generateChannels();
            start();
        }

        void PerformanceMonitor::generateNodeInfoVector() {
            //set mutexs
            nodeInfoVectorMutexs.reserve(locality_count);
            for (unsigned i = 0; i < locality_count; ++i) {
                nodeInfoVectorMutexs.push_back(std::make_unique<hpx::mutex>());
            }
            //add nodes
            nodeInfoVector.resize(locality_count);
            //sortedIds.resize(locality_count);
            for (hpx::id_type nodeId : workpools) {
                addNodeInfo(nodeId);
            }

        }

        void PerformanceMonitor::addNodeInfo(const hpx::id_type& nodeId) {
            std::uint32_t nodeId_num = hpx::naming::get_locality_id_from_id(nodeId);
            /*if(nodeId_num==0) {
                top_steal_id_type = nodeId;
            }*/
            std::unique_lock l(*nodeInfoVectorMutexs[nodeId_num]);

            NodeInfo newNode;
            newNode.id = nodeId;
            newNode.workRateAverage = 0.0;
            nodeInfoVector[nodeId_num] = newNode;
            //sortedIds[nodeId_num] = nodeId;
        }

        void PerformanceMonitor::generateChannels() {
            schedulerChannelHolder = std::make_shared<Workstealing::SchedulerChannels>() ;
            schedulerChannelHolder->init();
        }

        void PerformanceMonitor::start() {
            hpx::async(top_priority_executor, [&]() { autoRefreshInfo(); });
            //autoRefreshInfo();
        }
        //====================== get info ======================

        void PerformanceMonitor::refreshSchedularInfo() {
            // get local WorkRateAverage and send it to global
            double local_workRateAverage = schedulerChannelHolder->getWorkRateAverage();
            globalChannelMap->set_value( getWorkRateAverageNameById(local_id_num), local_workRateAverage).wait();

            //get WorkRateAverage for local nodeInfoVector
            for (PerformanceMonitor::NodeInfo& nodeInfo : nodeInfoVector) {
                hpx::id_type nodeID = nodeInfo.id;
                unsigned id_num = hpx::naming::get_locality_id_from_id(nodeID);
                std::unique_lock ln(*nodeInfoVectorMutexs[id_num]);

                if(id_num == local_id_num) {
                    nodeInfo.workRateAverage = local_workRateAverage;
                }else {
                    nodeInfo.workRateAverage = globalChannelMap->get_value(
                        getWorkRateAverageNameById(id_num)
                    ).get();
                }
            }
            
        }

        void PerformanceMonitor::refreshPoolTasksCountInfo() {
            /*hpx::experimental::for_loop(hpx::execution::par_unseq,
                0, locality_count,
                [&](unsigned i) {
                    std::unique_lock ln(*nodeInfoVectorMutexs[i]);
                    nodeInfoVector[i].tasksCount = hpx::async<workstealing::DepthPool::getTasksCount_action>(nodeInfoVector[i].id).get();
                });*/
            for(unsigned i=0;i<locality_count;++i){
                std::unique_lock ln(*nodeInfoVectorMutexs[i]);
                nodeInfoVector[i].tasksCount = hpx::async<workstealing::DepthPool::getTasksCount_action>(nodeInfoVector[i].id).get();
            }
        }

        void PerformanceMonitor::refreshCpuLoad(){
            //std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            //// Retrieve load information from all nodes
            //for (PerformanceMonitor::NodeInfo& nodeInfo : nodeInfoVector) {
            //    //get the idle threads percentages

            //    hpx::id_type locality_id = nodeInfo.id;
            //    std::uint32_t locality_count = hpx::naming::get_locality_id_from_id(locality_id);
            //    //std::string threads_idle_percentages = "/threads{locality#" + std::to_string(locality_count) + "/total}/idle-rate";
            //    std::string threads_idle_percentages = "/threads{locality#" +std::to_string(locality_count) + "/total}/count/cumulative";

            //    hpx::performance_counters::performance_counter counter(threads_idle_percentages);

        }

        bool PerformanceMonitor::refreshInfo() {
            {
                std::unique_lock l(refreshMutex);
                if(refreshRunning==true) {
                    return false;
                }
                refreshRunning = true;
            }

            //infoTaskGroup.run([&]() {refreshCpuLoad(); });
            infoTaskGroup.run([&]() {refreshSchedularInfo(); });
            infoTaskGroup.run([&]() {refreshPoolTasksCountInfo(); });
            //infoTaskGroup.run([&]() {refreshTopWorthStealId(); });
            //infoTaskGroup.run([&]() {refreshTopWorthAddId(); });
            infoTaskGroup.wait();

            //refreshSchedularInfo();
            //refreshPoolTasksCountInfo();
            refreshTopWorthStealId();
            //refreshTopWorthAddId();
            {
                std::unique_lock l(refreshMutex);
                refreshRunning = false;
            }

            return true;
        }
        

        bool PerformanceMonitor::autoRefreshInfo() {
            const int maxRefreshWaitTime = 180;
            const int minRefreshWaitTime = 1;
            int refreshWaitTime = 0;
            //refresh automatically after a fixed interval
            while (Scheduler::running){

                //hpx::async(top_priority_executor,[&](){refreshInfo();});
                refreshInfo();
                /*if (refreshWaitTime > 500) {
                    refreshTopWorthAddId();
                }*/
                bool flag;
                {
                    std::unique_lock lrg(refreshGetTargetMutex);
                    flag = refreshGetTarget;
                }
                if (flag == true) {
                    refreshWaitTime = std::min(refreshWaitTime + 60, maxRefreshWaitTime);
                }
                else {
                    refreshWaitTime = std::max(refreshWaitTime - 180, minRefreshWaitTime);
                }
                hpx::this_thread::sleep_for(std::chrono::milliseconds(refreshWaitTime));

            }

            return true;
        }


        //====================== node func ====================


        //====================== get results ======================

        //get top id_type worth to steal
        void PerformanceMonitor::refreshTopWorthStealId() {

            //hpx::spinlock resultMutex;
            hpx::id_type result_id_type;
            {
                std::unique_lock ld(top_steal_id_type_mutex_);
                result_id_type = top_steal_id_type;
            }
            double best_score = 0;
            double local_workRate = 0;
            unsigned local_tasksCount = 0;

            for(unsigned i=0;i<locality_count;++i) {
                std::unique_lock ln(*nodeInfoVectorMutexs[i]);
                double score = nodeInfoVector[i].workRateAverage * nodeInfoVector[i].tasksCount;
                if (score > best_score) {
                    best_score = score;
                    result_id_type = nodeInfoVector[i].id;
                    local_workRate = nodeInfoVector[i].workRateAverage;
                    local_tasksCount = nodeInfoVector[i].tasksCount;
                }
            }
            if(best_score > 0) {
                {
                    {
                        std::unique_lock ld(top_steal_id_type_mutex_);
                        top_steal_id_type = result_id_type;
                    }

                    {
                        std::unique_lock lrg(refreshGetTargetMutex);
                        refreshGetTarget = true;
                    }

                    auto id_num = hpx::naming::get_locality_id_from_id(result_id_type);
                    std::string message =
                        hpx::get_locality_name()
                        + "stealCacheSet:" + std::to_string(id_num)
                        + "_score:" + std::to_string(best_score)
                        + "_localWorkRate:" + std::to_string(local_workRate)
                        + "_taskCount::" + std::to_string(local_tasksCount) + "\n";
                    hpx::cout << message << std::flush;
                }
            }else {
                {
                    std::unique_lock ld(top_steal_id_type_mutex_);
                    top_steal_id_type = workpools[0];
                }
                {
                    std::unique_lock lrg(refreshGetTargetMutex);
                    refreshGetTarget = false;
                }

                std::string message =
                    hpx::get_locality_name()
                    + "steal_no_target_fount" + "\n";
                hpx::cout << message << std::flush;
            }
        }

        hpx::id_type PerformanceMonitor::getTopWorthStealId() {
            std::unique_lock lt(top_steal_id_type_mutex_);
            return top_steal_id_type;
        }

        void PerformanceMonitor::refreshTopWorthAddId() {
            {
                std::unique_lock lrta(refresh_top_id_state_mutex);
                if(refresh_top_id_state==true) {
                    return;
                }else {
                    refresh_top_id_state = true;
                }
            }
            //hpx::spinlock resultMutex;
            hpx::id_type result_id_type;
            {
                std::unique_lock ld(top_steal_id_type_mutex_);
                result_id_type = top_steal_id_type;
            }
            double best_score = std::numeric_limits<double>::max();
            double local_workRate = std::numeric_limits<double>::max();
            unsigned local_tasksCount = std::numeric_limits<unsigned>::max();

            double here_score = 0;
            unsigned here_tasksCount = 0;

            for (unsigned i = 0; i < locality_count; ++i) {
                std::unique_lock ln(*nodeInfoVectorMutexs[i]);
                double score = nodeInfoVector[i].workRateAverage * nodeInfoVector[i].tasksCount;
                if(i == local_id_num) {
                    here_score = score;
                    here_tasksCount = nodeInfoVector[i].tasksCount;
                }
                if (score < best_score) {
                    best_score = score;
                    result_id_type = nodeInfoVector[i].id;
                    local_workRate = nodeInfoVector[i].workRateAverage;
                    local_tasksCount = nodeInfoVector[i].tasksCount;
                }
            }
            if (here_score > 0 && best_score > 0 && best_score < std::numeric_limits<double>::max() && (best_score*100 < here_score) ) {
                {
                    std::unique_lock ld(top_add_id_type_mutex_);
                    top_add_id_type = result_id_type;
                    top_add_num = static_cast<unsigned>( (here_tasksCount - local_tasksCount) * (best_score / here_score) );

                    auto id_num = hpx::naming::get_locality_id_from_id(result_id_type);
                    std::string message =
                        hpx::get_locality_name()
                        + "addCacheSet:" + std::to_string(id_num)
                        + "_score:" + std::to_string(best_score)
                        + "_taskCount::" + std::to_string(local_tasksCount)
                        + "_WorkRate:" + std::to_string(local_workRate)
                        + "_local_score:" + std::to_string(here_score) + "\n";
                    hpx::cout << message << std::flush;
                }
            }else {
                std::unique_lock ld(top_add_id_type_mutex_);
                top_add_id_type = workpools[0];
                top_add_num = 0;

                std::string message =
                    hpx::get_locality_name()
                    + "no_better_target_add" + "\n";
                hpx::cout << message << std::flush;
            }

            {
                std::unique_lock lrta(refresh_top_id_state_mutex);
                refresh_top_id_state = false;
            }
        }

        hpx::id_type PerformanceMonitor::getTopWorthAddId() {
            std::unique_lock lt(top_add_id_type_mutex_);
            if(top_add_num>0) {
                --top_add_num;
                return top_add_id_type;
            }
            return workpools[0];
        }


        //====================== tool ======================

        //template<typename T>
        //hpx::id_type selectByNum(const std::vector<std::pair<T, hpx::id_type>>& loads, bool select_min = true) {
        //    auto compare_func = select_min ?
        //        [](const std::pair<T, hpx::id_type>& a, const std::pair<T, hpx::id_type>& b) { return a.first < b.first; } :
        //        [](const std::pair<T, hpx::id_type>& a, const std::pair<T, hpx::id_type>& b) { return a.first > b.first; };
        //    auto selected_it = std::minmax_element(
        //        loads.begin(), loads.end(),
        //        compare_func
        //    );
        //    //hpx::cout << "choose:" << selected_it->first << " , " << selected_it->second << std::endl;
        //    return selected_it->second;
        //}


    }
}
