#include "SchedulerChannels.hpp"


//HPX_REGISTER_COMPONENT_MODULE();  // Register the component module

//HPX_REGISTER_UNORDERED_MAP(unsigned, int);

namespace Workstealing {
    /**
    SchedulerChannelHolder::SchedulerChannelHolder() {
    }

    void SchedulerChannelHolder::init() {

        const std::size_t thread_count = hpx::get_os_thread_count();
        //local_mutexs.resize(thread_count);
        local_mutexs.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i) {
            local_mutexs.push_back(std::make_unique<hpx::mutex>());
        }
        threadStateVector.assign(thread_count, ThreadState::Dead);

        //hpx::cout << "SchedulerChannelHolder_INIT_end" << std::endl;

    }


    ThreadState SchedulerChannelHolder::getState(unsigned id) {
        ThreadState value;
        {
            std::unique_lock<hpx::mutex> l(*local_mutexs[id]);
            value = threadStateVector[id];
        }
        return value;
    }

    void SchedulerChannelHolder::setState(unsigned id, ThreadState threadState) {
        {
            std::unique_lock<hpx::mutex> l(*local_mutexs[id]);
            threadStateVector[id] = threadState;
        }
    }

    int SchedulerChannelHolder::getSize() {
            return threadStateVector.size();
    }

    **/
 
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