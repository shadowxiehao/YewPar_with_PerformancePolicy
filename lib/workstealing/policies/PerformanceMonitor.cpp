#include "PerformanceMonitor.hpp"
#include <hpx/functional/function.hpp>
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>

#include <memory>
#include <hpx/include/threads.hpp>
#include <hpx/include/util.hpp>


namespace Workstealing {
        //channel
        std::shared_ptr<SchedulerChannelHolder> schedulerChannelHolder;

    namespace Policies {

        //====================== init ======================

        /*PerformanceMonitor::PerformanceMonitor() {
        }*/
        void PerformanceMonitor::init(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools) {
            this->local_workpool = local_workpool;
            this->distributed_workpools = distributed_workpools;

            generateNodeInfoVector();
            generateChannels();
            start();
        }

        void PerformanceMonitor::generateNodeInfoVector() {
            
            for (hpx::id_type nodeId : distributed_workpools) {
                if (!nodeExists(nodeId)) {
                    addNodeInfo(nodeId);
                }
            }
        }

        void PerformanceMonitor::generateChannels() {
            //generate SchedulerChannel
            //hpx::cout << "generateChannels_start"  << std::endl;
            schedulerChannelHolder = std::make_shared<Workstealing::SchedulerChannelHolder>() ;
            schedulerChannelHolder->init();
        }

        //====================== node func ====================
        void PerformanceMonitor::addNodeInfo(const hpx::id_type& nodeId) {
            std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            NodeInfo newNode;
            newNode.id = nodeId;
            newNode.cpuLoad = 0.0;
            nodeInfoVector.push_back(newNode);
        }

        void PerformanceMonitor::updateNodeCpuLoad(const hpx::id_type& nodeId, double load) {
            std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            auto it = std::find_if(nodeInfoVector.begin(), nodeInfoVector.end(),
                [nodeId](const NodeInfo& s) {
                    return s.id == nodeId;
                });

            if (it != nodeInfoVector.end()) {
                it->cpuLoad = load;
            }
        }

        bool PerformanceMonitor::nodeExists(const hpx::id_type& nodeId) {
            std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            auto it = std::find_if(nodeInfoVector.begin(), nodeInfoVector.end(),
                [nodeId](const NodeInfo& s) {
                    return s.id == nodeId;
                });

            return it != nodeInfoVector.end();
        }

        hpx::id_type PerformanceMonitor::getTopId() {
            std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            return nodeInfoVector.front().id;
        }

        //====================== get info ======================

        void PerformanceMonitor::refreshSchedularInfo() {
            // Use the channel
            workRatesGlobalMap->set_value(globalWorkRateAverageName + std::to_string(hpx::get_locality_id()), schedulerChannelHolder->getWorkRateAverage()).get();
            double value = workRatesGlobalMap->get_value(globalWorkRateAverageName+ std::to_string(hpx::get_locality_id())).get();

            std::string message = hpx::get_locality_name() + "received: " + std::to_string(value) + "\n";
            hpx::cout << message <<std::flush;
        }

        void PerformanceMonitor::refreshCpuLoad(){
            std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            // Retrieve load information from all nodes
            for (PerformanceMonitor::NodeInfo& nodeInfo : nodeInfoVector) {
                //get the idle threads percentages

                hpx::id_type locality_id = nodeInfo.id;
                std::uint32_t locality_num = hpx::naming::get_locality_id_from_id(locality_id);
                //std::string threads_idle_percentages = "/threads{locality#" + std::to_string(locality_num) + "/total}/idle-rate";
                std::string threads_idle_percentages = "/threads{locality#" +std::to_string(locality_num) + "/total}/count/cumulative";

                hpx::performance_counters::performance_counter counter(threads_idle_percentages);
                
                //hpx::cout << "pos1: " << threads_idle_percentages
                //          << hpx::get_locality_id()
                //          << std::endl;  // TEST

                double load = counter.get_value<int>().get();
                //hpx::cout << "pos2" << " " << load << std::endl;//TEST
                nodeInfo.cpuLoad = load;
            }
        }

        bool PerformanceMonitor::refreshInfo() {

            std::unique_lock<hpx::mutex> l(refreshMutex);

            task_group_run_with_executor(infoTasks,top_priority_executor,[&](){refreshCpuLoad();});
            task_group_run_with_executor(infoTasks, top_priority_executor, [&]() {refreshSchedularInfo(); });

            infoTasks.wait();
            compareNode();

            return true;
        }
        

        bool PerformanceMonitor::autoRefreshInfo() {

            while (Scheduler::running){
                //hpx::cout<<"autoRefreshInfo" << hpx::get_locality_id() <<std::endl;
                hpx::async(top_priority_executor,[&](){refreshInfo();});

                hpx::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            return true;
        }

        hpx::id_type PerformanceMonitor::getTopWorthStealId() {
            hpx::id_type resultId = getTopId();
            //hpx::async([&]() {refreshInfo(); });
            //hpx::async(top_priority_executor, [&]() { refreshInfo(); });
           
            //hpx::id_type resultId = distributed_workpools.front();
            /*bool stat = nodeExists(resultId);
            hpx::cout << hpx::get_locality_name() + std::to_string(stat) << std::endl;*/

            return resultId;
        }

        //====================== compute sequence ======================
        void PerformanceMonitor::compareNode() {
            std::unique_lock<hpx::mutex> l(nodeInfoVectorMutex);

            hpx::sort(nodeInfoVector.begin(),
                nodeInfoVector.end(),
                CompareNodeInfo());
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
