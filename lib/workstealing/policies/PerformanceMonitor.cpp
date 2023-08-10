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
            this->local_workpool = local_workpool;
            this->distributed_workpools = distributed_workpools;
            local_id_num = hpx::get_locality_id();
            locality_count = hpx::find_all_localities().size();
            thread_count = hpx::get_os_thread_count();

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
            sortedIds.resize(locality_count);
            addNodeInfo(local_workpool);
            for (hpx::id_type nodeId : distributed_workpools) {
                addNodeInfo(nodeId);
            }

        }

        void PerformanceMonitor::addNodeInfo(const hpx::id_type& nodeId) {
            std::uint32_t nodeId_num = hpx::naming::get_locality_id_from_id(nodeId);
            std::unique_lock<hpx::mutex> l(*nodeInfoVectorMutexs[nodeId_num]);

            NodeInfo newNode;
            newNode.id = nodeId;
            newNode.workRateAverage = 0.0;
            nodeInfoVector[nodeId_num] = newNode;
            sortedIds[nodeId_num] = nodeId;
        }

        void PerformanceMonitor::generateChannels() {
            schedulerChannelHolder = std::make_shared<Workstealing::SchedulerChannels>() ;
            schedulerChannelHolder->init();
        }

        void PerformanceMonitor::start() {
            hpx::async(top_priority_executor, [&]() { autoRefreshInfo(); });
            //hpx::async([&]() { autoRefreshInfo(); });
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

            std::unique_lock<hpx::mutex> l(refreshMutex);

            /*task_group_run_with_executor(infoTaskGroup,top_priority_executor,[&](){refreshCpuLoad();});
            task_group_run_with_executor(infoTaskGroup, top_priority_executor, [&]() {refreshSchedularInfo(); });*/

            infoTaskGroup.run([&]() {refreshCpuLoad(); });
            infoTaskGroup.run([&]() {refreshSchedularInfo(); });

            infoTaskGroup.wait();
            refreshSortedIds();
            sendWorthStealToOther();

            return true;
        }
        

        bool PerformanceMonitor::autoRefreshInfo() {

            //refresh automatically after a fixed interval
            while (Scheduler::running){
                //hpx::async(top_priority_executor,[&](){refreshInfo();});
                hpx::async([&]() {refreshInfo(); });

                hpx::this_thread::sleep_for(std::chrono::milliseconds(80));
            }

            return true;
        }


        //====================== node func ====================

        //Active send task
        void PerformanceMonitor::sendWorthStealToOther() {
            hpx::async([&]() {
                hpx::id_type topId;
                hpx::id_type backId;
                {
                    std::unique_lock<hpx::mutex> ls(sortedIdsMutex);
                    topId = sortedIds.front();
                    backId = sortedIds.back();
                }
                hpx::execution::experimental::task_group localTaskGroup;
                for (int i = 0; i < 4; ++i) {
                    localTaskGroup.run([&]() {
                            auto task = hpx::async<workstealing::DepthPool::steal_action>(topId).get();
                            if (task) {
                                hpx::async<workstealing::DepthPool::addWork_action>(backId, task, 0).get();
                            }
                        }
                    );
                }
                localTaskGroup.wait();
                }
            );
        }

        //====================== compute sequence ======================

         // Call this function to update the sorted Ids
        void PerformanceMonitor::refreshSortedIds() {
            std::vector<NodeInfo> tempVector(locality_count);

            // Parallel copying of nodeInfoVector to tempVector
            hpx::experimental::for_loop(hpx::execution::par_unseq,
                0, locality_count,
                [&](unsigned i) {
                    std::unique_lock<hpx::mutex> ln(*nodeInfoVectorMutexs[i]);
                    tempVector[i] = nodeInfoVector[i];
                });
            /*for(unsigned i=0;i< locality_count;++i) {
                std::unique_lock<hpx::mutex> ln(*nodeInfoVectorMutexs[i]);
                tempVector[i] = nodeInfoVector[i];
            }*/

            // Sorting tempVector
            std::sort(tempVector.begin(), tempVector.end(), CompareNodeInfo());

            // Updating sortedIds
            {
                std::unique_lock<hpx::mutex> ls(sortedIdsMutex);
                for (std::uint32_t i = 0; i < tempVector.size(); i++) {
                    sortedIds[i] = tempVector[i].id;
                }
            }
        }


        //====================== get results ======================

        //get top id_type worth to steal
        hpx::id_type PerformanceMonitor::getTopWorthStealId() {
            return getTopIdWithoutLocal();
        }

        // Retrieve top n ids without local id
        std::vector<hpx::id_type> PerformanceMonitor::getTopNIdsWithoutLocal(std::uint32_t n) {
            std::unique_lock<hpx::mutex> lock(sortedIdsMutex);
            if (n > sortedIds.size() - 1) {
                n = sortedIds.size() - 1;
            }
            std::vector<hpx::id_type> result;

            for (const auto& id : sortedIds) {
                if (hpx::naming::get_locality_id_from_id(id) != local_id_num) {
                    result.push_back(id);
                    if (result.size() == n) {
                        break;
                    }
                }
            }

            return result;
        }

        // Retrieve the top id without local id
        hpx::id_type PerformanceMonitor::getTopIdWithoutLocal() {
            std::unique_lock<hpx::mutex> lock(sortedIdsMutex);

            for (const auto& id : sortedIds) {
                if (hpx::naming::get_locality_id_from_id(id) != local_id_num) {
                    std::string message = hpx::get_locality_name() + "return:" + std::to_string(hpx::naming::get_locality_id_from_id(id)) + "\n";
                    hpx::cout << message <<std::flush;
                    return id;
                }
            }

            return {};  // Return an invalid id if not found
        }

        // Retrieve top n ids
        std::vector<hpx::id_type> PerformanceMonitor::getTopNIds(std::uint32_t n) {
            std::unique_lock<hpx::mutex> lock(sortedIdsMutex);
            if (n > sortedIds.size()) {
                n = sortedIds.size();
            }
            return std::vector<hpx::id_type>(sortedIds.begin(), sortedIds.begin() + n);
        }

        // Retrieve the top id
        hpx::id_type  PerformanceMonitor::getTopId() {
            std::unique_lock<hpx::mutex> lock(sortedIdsMutex);
            if (!sortedIds.empty()) {
                return sortedIds[0];
            }
            return hpx::id_type();  // Return an invalid id if empty
        }

        //====================== tool ======================

        template<typename T>
        hpx::id_type selectByNum(const std::vector<std::pair<T, hpx::id_type>>& loads, bool select_min = true) {
            auto compare_func = select_min ?
                [](const std::pair<T, hpx::id_type>& a, const std::pair<T, hpx::id_type>& b) { return a.first < b.first; } :
                [](const std::pair<T, hpx::id_type>& a, const std::pair<T, hpx::id_type>& b) { return a.first > b.first; };

            auto selected_it = std::minmax_element(
                loads.begin(), loads.end(),
                compare_func
            );

            //hpx::cout << "choose:" << selected_it->first << " , " << selected_it->second << std::endl;

            return selected_it->second;
        }


    }
}
