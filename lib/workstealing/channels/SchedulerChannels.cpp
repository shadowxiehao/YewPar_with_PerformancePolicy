#include "SchedulerChannels.hpp"


HPX_REGISTER_UNORDERED_MAP(std::string, double);

namespace Workstealing {
    
    //hpx::unordered_map<unsigned, double> workRatesGlobalMap;
    std::string globalChannelMapName = "GlobalChannelMap";
    std::shared_ptr<hpx::unordered_map<std::string, double>> workRatesGlobalMap;
    std::string globalWorkRateAverageName = "workRateAverage/";

    void SchedulerChannelHolder::init() {

        thread_count = hpx::get_os_thread_count();
        //recordMutexs.resize(thread_count);
        recordMutexs.reserve(thread_count);
        workRateMutexs.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i) {
            recordMutexs.push_back(std::make_unique<hpx::mutex>());
            workRateMutexs.push_back(std::make_unique<hpx::mutex>());
        }
        
        recordVector.assign(thread_count, Record());
        workRateVector.assign(thread_count, 0.1);

        //global
        hpx::distributed::barrier b("global_barrier", hpx::find_all_localities().size());

        if (0 == hpx::get_locality_id()) {
            std::size_t num_segments = 8;
            std::vector<hpx::id_type> locs = hpx::find_all_localities();
            auto layout = hpx::container_layout(num_segments, locs);
            workRatesGlobalMap = std::make_shared<hpx::unordered_map<std::string, double>>(layout);

            const std::string localChannelMapNme = globalChannelMapName;

            workRatesGlobalMap->register_as(localChannelMapNme).get();
            b.wait();
        }else {
            b.wait();
            hpx::cout << hpx::get_locality_name() + "TRYconnect" << std::endl;
            workRatesGlobalMap = std::make_shared<hpx::unordered_map<std::string, double>>();
            workRatesGlobalMap->connect_to(globalChannelMapName).get();
            hpx::cout << hpx::get_locality_name() + "connected" << std::endl;
        }

    }

    void SchedulerChannelHolder::setState(unsigned id, ThreadState threadState) {
        std::unique_lock lr(*recordMutexs[id]);
        
        if(recordVector[id].threadState == ThreadState::IDLE && threadState == ThreadState::WORKING) {
            //if idle->work 1.record idle time 2.restart timer 3.set ThreadState->work
            recordVector[id].idleTime = recordVector[id].timer.elapsed_nanoseconds();
            recordVector[id].timer.restart();
            recordVector[id].threadState = threadState;
            
        }else if(recordVector[id].threadState == ThreadState::WORKING && threadState == ThreadState::IDLE){
            //if work->idle 1.record work time 2.calculate the working rate 3.restart timer 4. set ThreadState->idle
            recordVector[id].workTime = recordVector[id].timer.elapsed_nanoseconds();
            //use Exponential Weighted Moving Average, EWMA; but int64->double may lose some accuracy, just for speed.
            std::unique_lock lw(*workRateMutexs[id]);
            workRateVector[id] = (static_cast<double>(recordVector[id].workTime) / static_cast<double>(recordVector[id].idleTime)) * 0.6 + workRateVector[id] * 0.4;
            recordVector[id].timer.restart();
            recordVector[id].threadState = threadState;
        }else {
            //set ThreadState
            recordVector[id].threadState = threadState;
        }

    }

    unsigned SchedulerChannelHolder::getSize() const {
            return thread_count;
    }

    ThreadState SchedulerChannelHolder::getState(unsigned id) const {
        std::unique_lock lr(*recordMutexs[id]);
        return recordVector[id].threadState;
    }

    SchedulerChannelHolder::Record SchedulerChannelHolder::getRecord(unsigned id) {
        std::unique_lock lr(*recordMutexs[id]);
        return recordVector[id];
    }

    double SchedulerChannelHolder::getWorkRate(unsigned id) const {
        std::unique_lock lw(*workRateMutexs[id]);
        return workRateVector[id];
    }

    double SchedulerChannelHolder::getWorkRateSum() {
        
        double totalWorkRate = 0.0;

        for (unsigned i = 0; i < recordVector.size(); ++i) {
            totalWorkRate += getWorkRate(i);
        }

        return totalWorkRate;
    }

    double SchedulerChannelHolder::getWorkRateAverage() {
        return getWorkRateSum() / static_cast<double>(thread_count);
    }
 
    //SchedulerChannelHolder::SchedulerChannelHolder(){

    //    local_id_num = hpx::get_locality_id();
    //    
    //}

    //void SchedulerChannelHolder::initChannels() {
    //    hpx::unordered_map<int, ThreadState> my_map = hpx::new_<hpx::unordered_map<int, ThreadState>>(hpx::find_here());
    //    std::string global_name = "threadStateMap_" + std::to_string(local_id_num);
    //    hpx::agas::register_name(global_name, my_map.get());
    //}


    //ThreadState SchedulerChannelHolder::getChannelForAction() {
    //    return threadStateMap.find(1)->second;
    //}

    //ThreadState SchedulerChannelHolder::getChannel(hpx::id_type id) {
    //    return hpx::async<Workstealing::SchedulerChannelHolder::getChannelAction>(id).get();
    //}

    //void SchedulerChannelHolder::setChannelForAction(int num,ThreadState threadState) {
    //    {
    //        hpx::cout << "setChannelForAction1:" << hpx::get_locality_name() << " :" << num << threadState << std::endl;
    //        std::unique_lock<hpx::mutex> l(local_mutex);
    //        threadStateMap.insert_or_assign(num, threadState);
    //        hpx::cout << "setChannelForAction2:" << hpx::get_locality_name() << std::endl;
    //    }
    //}

    //void SchedulerChannelHolder::setChannel(hpx::id_type id, int num, ThreadState threadState) {
    //    hpx::async<Workstealing::SchedulerChannelHolder::setChannelAction>(id,num, threadState).wait();
    //}
}


//HPX_REGISTER_CHANNEL(int);

//HPX_REGISTER_COMPONENT(hpx::components::component<Workstealing::SchedulerChannelHolder>, SchedulerChannelHolder);
//HPX_REGISTER_ACTION(Workstealing::SchedulerChannelHolder::getChannelAction, SchedulerChannelHolder_getChannel_action);
//HPX_REGISTER_ACTION(Workstealing::SchedulerChannelHolder::setChannelAction, SchedulerChannelHolder_setChannel_action);