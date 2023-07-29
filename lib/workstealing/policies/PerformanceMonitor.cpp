#include "PerformanceMonitor.hpp"
#include <hpx/functional/function.hpp>
#include <hpx/modules/runtime_distributed.hpp>
#include <hpx/performance_counters/manage_counter_type.hpp>

#include <memory>
#include <hpx/include/threads.hpp>
#include <hpx/include/util.hpp>

namespace Workstealing {
    namespace Policies {

        PerformanceMonitor::PerformanceMonitor(hpx::id_type& local_workpool, std::vector<hpx::id_type>& distributed_workpools)
            : local_workpool(local_workpool),
            distributed_workpools(distributed_workpools),
            performanceMonitorRunning(true) {

        }

        void PerformanceMonitor::generateNodeInfoVector() {
            hpx::cout << "generateNodeInfoVector:"
                    << distributed_workpools.size()
                    << hpx::get_locality_id()
                    << std::endl;

            while (nodeInfoVector.size() != distributed_workpools.size()) {
                std::vector<NodeInfo> newNodeInfoVector;
                for (hpx::id_type nodeId : distributed_workpools) {

                    auto it = std::find_if(nodeInfoVector.begin(),
                                           nodeInfoVector.end(),
                      [nodeId](const NodeInfo& s) {
                          return s.id == nodeId;
                                                   });

                    if (it != nodeInfoVector.end()) {
                      NodeInfo newNode = *it;
                      newNodeInfoVector.push_back(newNode);
                    }else {
                      NodeInfo newNode;
                      newNode.id = nodeId;
                      newNodeInfoVector.push_back(newNode);
                    }
                }

                nodeInfoVector = newNodeInfoVector;
            }

            hpx::cout << "generate complete"  << hpx::naming::get_locality_id_from_id(nodeInfoVector.at(0).id)
                <<" "
                << hpx::naming::get_locality_id_from_id(nodeInfoVector.at(1).id)
                << " "
                << nodeInfoVector.size()
                << " "
                << hpx::get_locality_id
                << std::endl;
        }

        void PerformanceMonitor::refreshCpuLoad(){
            // Retrieve load information from all nodes
            for (PerformanceMonitor::NodeInfo& nodeInfo : nodeInfoVector) {
                //get the idle threads percentages

                hpx::id_type locality_id = nodeInfo.id;
                std::uint32_t locality_num = hpx::naming::get_locality_id_from_id(locality_id);
                //std::string threads_idle_percentages = "/threads{locality#" + std::to_string(locality_num) + "/total}/idle-rate";
                std::string threads_idle_percentages = "/threads{locality#" +std::to_string(locality_num) + "/total}/count/cumulative";

                hpx::performance_counters::performance_counter counter(threads_idle_percentages);
                
                hpx::cout << "pos1: " << threads_idle_percentages
                          << hpx::get_locality_id()
                          << std::endl;  // TEST

                double load = counter.get_value<int>().get();
                hpx::cout << "pos2" << " " << load << std::endl;//TEST
                nodeInfo.cpuLoad = load;
            }
        }

        bool PerformanceMonitor::refreshInfo() {
            
            task_group_run_with_executor(infoTasks,top_priority_executor,[&](){refreshCpuLoad();});

            infoTasks.wait();
            compareNode();

            return true;
        }
        

        bool PerformanceMonitor::autoRefreshInfo() {

            while (performanceMonitorRunning){
                hpx::cout<<"autoRefreshInfo" << hpx::get_locality_id() <<std::endl;
                hpx::async(top_priority_executor,[&](){PerformanceMonitor::refreshInfo();});

                hpx::this_thread::sleep_for(std::chrono::seconds(1));
            }

            return true;
        }

        hpx::id_type PerformanceMonitor::getTopWorthStealId() {
            hpx::id_type id = nodeInfoVector.at(0).id;
            hpx::async(top_priority_executor, [&]() { refreshInfo(); });
            hpx::cout << "getTopWorthStealId:" << id << std::endl;
            return id;
        }

        //tool

        template<typename T>
        hpx::id_type selectByNum(const std::vector<std::pair<T, hpx::id_type>>& loads, bool select_min = true) {
            auto compare_func = select_min ?
                [](const std::pair<T, hpx::id_type>& a, const std::pair<T, hpx::id_type>& b) { return a.first < b.first; } :
                [](const std::pair<T, hpx::id_type>& a, const std::pair<T, hpx::id_type>& b) { return a.first > b.first; };

            auto selected_it = std::minmax_element(
                loads.begin(), loads.end(),
                compare_func
            );

            hpx::cout << "choose:" << selected_it->first << " , " << selected_it->second << std::endl;

            return selected_it->second;
        }


    }
}
