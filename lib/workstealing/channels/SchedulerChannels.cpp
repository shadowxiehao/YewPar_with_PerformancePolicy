#include "SchedulerChannels.hpp"
#include <hpx/parallel/algorithms/for_loop.hpp>

#include "workstealing/DepthPool.hpp"

HPX_REGISTER_UNORDERED_MAP(std::string, double);

namespace Workstealing {
    //std::shared_ptr<hpx::unordered_map<std::string, double>> globalChannelMap;

    std::size_t thread_count; //hpx::get_os_thread_count
    std::uint32_t local_id_num; //record hpx::get_locality_id
    unsigned locality_count;

    //std::string globalChannelMapName = "GlobalChannelMap";
    //std::string globalWorkRateAverageName = "workRateAverage/";

    /*std::string getWorkRateAverageNameById(std::uint32_t id) {
        return globalWorkRateAverageName + std::to_string(id);
    }*/

    void SchedulerChannels::init() {
        //recordMutexs.resize(thread_count);
        recordMutexs.reserve(thread_count);
        workRateMutexs.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i) {
            recordMutexs.push_back(std::make_unique<hpx::mutex>());
            workRateMutexs.push_back(std::make_unique<hpx::mutex>());
        }
        recordVector.assign(thread_count, Record());
        workRateVector.assign(thread_count, 0.0);

        //==global==
        //global locker
        //hpx::distributed::barrier b("global_barrier", hpx::find_all_localities().size());

        //set global map for data transferring
        //if (0 == local_id_num) { //only one init, others connect
        //    std::vector<hpx::id_type> locs = hpx::find_all_localities();
        //    std::size_t num_segments = locs.size();
        //    auto layout = hpx::container_layout(num_segments, locs);
        //    globalChannelMap = std::make_shared<hpx::unordered_map<std::string, double>>(num_segments,layout);
        //    const std::string localChannelMapNme = globalChannelMapName;
        //    //regist local global container
        //    globalChannelMap->register_as(localChannelMapNme).wait();
        //    //set initial data
        //    globalChannelMap->set_value(getWorkRateAverageNameById(local_id_num), WORK_RATE_INIT_VALUE).wait();
        //    b.wait();
        //}else {
        //    b.wait();
        //    globalChannelMap = std::make_shared<hpx::unordered_map<std::string, double>>();
        //    //connect the global container
        //    globalChannelMap->connect_to(globalChannelMapName).wait();
        //    //set local initial data
        //    globalChannelMap->set_value(getWorkRateAverageNameById(local_id_num), WORK_RATE_INIT_VALUE).wait();
        //}
    }

    void SchedulerChannels::setState(unsigned id, ThreadState threadState) {
        std::unique_lock lr(*recordMutexs[id]);
        
        if(recordVector[id].threadState == ThreadState::IDLE && threadState == ThreadState::WORKING) {
            //if idle->work 1.record idle time 2.restart timer 3.set ThreadState->work
            recordVector[id].idleTime = recordVector[id].timer.elapsed_microseconds();
            recordVector[id].timer.restart();
            recordVector[id].threadState = threadState;
        }else if(recordVector[id].threadState == ThreadState::WORKING && threadState == ThreadState::IDLE){
            //if work->idle 1.record work time 2.calculate the working rate 3.restart timer 4. set ThreadState->idle
            recordVector[id].workTime = recordVector[id].timer.elapsed_microseconds();
            //use Exponential Weighted Moving Average, EWMA; but int64->double may lose some accuracy, just for speed.
            std::unique_lock lw(*workRateMutexs[id]);
            double workTime = static_cast<double>(recordVector[id].workTime);
            double idleTime = static_cast<double>(recordVector[id].idleTime);

            /*workRateVector[id] = sigmoid( 
                (sigmoid((workTime / idleTime))*0.5 + sigmoid(workTime+idleTime)*0.5)* 0.65
                + workRateVector[id] * 0.35 
            );*/
            
            /*workRateVector[id] = 
                (workTime / idleTime) * (workTime + idleTime) /100000 * 0.65
                + workRateVector[id] * 0.35;*/
            //double workPercent = workTime / (workTime + idleTime);
            //workRateVector[id] =
            //    //std::pow( std::max(workPercent-0.13,0.0) *3 , 3) + std::max( std::log(workTime/100),1.0) * 0.65
            //    std::pow(std::max(workPercent - 0.02, 0.0001) * 2, 3)* std::max(std::log(workTime / 100), 1.0) * 0.65
            //    + workRateVector[id] * 0.35;
            workRateVector[id] =
                (workTime / (workTime + idleTime))* 10 * std::log(2.72 + (workTime + idleTime) * thread_count) * 0.65
                + workRateVector[id] * 0.35;
            recordVector[id].timer.restart();
            recordVector[id].threadState = threadState;
            //hpx::sync<workstealing::DepthPool::setWorkRate_action>(local_id_type, workRateVector[id]);
        }else {
            //set ThreadState
            recordVector[id].threadState = threadState;
        }

    }

    unsigned SchedulerChannels::getSize() const {
            return thread_count;
    }

    ThreadState SchedulerChannels::getState(unsigned id) const {
        std::unique_lock lr(*recordMutexs[id]);
        return recordVector[id].threadState;
    }

    SchedulerChannels::Record SchedulerChannels::getRecord(unsigned id) {
        std::unique_lock lr(*recordMutexs[id]);
        return recordVector[id];
    }

    double SchedulerChannels::getWorkRate(unsigned id) const {
        std::unique_lock lw(*workRateMutexs[id]);
        return workRateVector[id];
    }

    double SchedulerChannels::getWorkRateSum() {
        
        double totalWorkRate = 0.0;
        //hpx::spinlock dm;
        /*hpx::experimental::for_loop(hpx::execution::par_unseq,
                 0, recordVector.size(),
                [&](unsigned i) {
                    std::unique_lock ld(dm);
                    totalWorkRate += getWorkRate(i);
                });*/
        for (unsigned i = 0; i < recordVector.size(); ++i) {
            totalWorkRate += getWorkRate(i);
        }

        return totalWorkRate;
    }

    double SchedulerChannels::getWorkRateAverage() {
        return getWorkRateSum() / static_cast<double>(thread_count);
    }
 
    //SchedulerChannels::SchedulerChannels(){

    //    local_id_num = hpx::get_locality_id();
    //    
    //}

    //void SchedulerChannels::initChannels() {
    //    hpx::unordered_map<int, ThreadState> my_map = hpx::new_<hpx::unordered_map<int, ThreadState>>(hpx::find_here());
    //    std::string global_name = "threadStateMap_" + std::to_string(local_id_num);
    //    hpx::agas::register_name(global_name, my_map.get());
    //}


    //ThreadState SchedulerChannels::getChannelForAction() {
    //    return threadStateMap.find(1)->second;
    //}

    //ThreadState SchedulerChannels::getChannel(hpx::id_type id) {
    //    return hpx::async<Workstealing::SchedulerChannels::getChannelAction>(id).get();
    //}

    //void SchedulerChannels::setChannelForAction(int num,ThreadState threadState) {
    //    {
    //        hpx::cout << "setChannelForAction1:" << hpx::get_locality_name() << " :" << num << threadState << std::endl;
    //        std::unique_lock<hpx::mutex> l(local_mutex);
    //        threadStateMap.insert_or_assign(num, threadState);
    //        hpx::cout << "setChannelForAction2:" << hpx::get_locality_name() << std::endl;
    //    }
    //}

    //void SchedulerChannels::setChannel(hpx::id_type id, int num, ThreadState threadState) {
    //    hpx::async<Workstealing::SchedulerChannels::setChannelAction>(id,num, threadState).wait();
    //}
}


//HPX_REGISTER_CHANNEL(int);

//HPX_REGISTER_COMPONENT(hpx::components::component<Workstealing::SchedulerChannels>, SchedulerChannels);
//HPX_REGISTER_ACTION(Workstealing::SchedulerChannels::getChannelAction, SchedulerChannelHolder_getChannel_action);
//HPX_REGISTER_ACTION(Workstealing::SchedulerChannels::setChannelAction, SchedulerChannelHolder_setChannel_action);