#ifndef SCHEDULERCHANNELS_HPP
#define SCHEDULERCHANNELS_HPP

#include <hpx/include/lcos.hpp>
//#include <hpx/include/components.hpp>
//#include <hpx/channel.hpp>
//#include <hpx/include/unordered_map.hpp>
#include <hpx/runtime_distributed/find_localities.hpp>
#include <hpx/iostream.hpp>

namespace Workstealing {
    /**
    //state enum
    enum class ThreadState {
        IDLE,
        WORKING,
        Dead
    };


    inline std::ostream& operator<<(std::ostream& os, const ThreadState& state) {
        switch (state) {
        case ThreadState::IDLE:
            os << "IDLE";
            break;
        case ThreadState::WORKING:
            os << "WORKING";
            break;
        case ThreadState::Dead:
            os << "Dead";
            break;
        default:
            os << "Unknown ThreadState";
            break;
        }
        return os;
    }

    inline std::string toString(const ThreadState& state) {
        switch (state) {
        case ThreadState::IDLE:
            return "IDLE";
        case ThreadState::WORKING:
            return "WORKING";
        case ThreadState::Dead:
            return "Dead";
        default:
            return "Unknown ThreadState";
        }
    }

     //Create a component to hold the channel
    class SchedulerChannelHolder {

    private:
         std::vector<std::unique_ptr<hpx::mutex>> local_mutexs;

        std::vector<ThreadState> threadStateVector;

    public:
        //SchedulerChannelHolder()=default;
        SchedulerChannelHolder();
        void init();
        int getSize();
        ThreadState getState(unsigned id);

        void setState(unsigned id,ThreadState threadState);
    };

    **/
//    extern SchedulerChannelHolder schedulerChannelHolder;

    //channels
    //extern hpx::unordered_map<unsigned, ThreadState> threadStateMap;

    // Create a component to hold the channel
    //class SchedulerChannelHolder : public hpx::components::component_base<SchedulerChannelHolder> {
    //class SchedulerChannelHolder {

    //private:
    //    hpx::mutex local_mutex;
    //    unsigned local_id_num;

    //public:
        //SchedulerChannelHolder()=default;
        //SchedulerChannelHolder();

        //SchedulerChannelHolder(hpx::lcos::channel<int> c);

        //hpx::lcos::channel<int> c_;
        //std::unordered_map<unsigned, ThreadState> threadStateMap;

        //void initChannels();

        //ThreadState getChannelForAction();
        ////HPX_DEFINE_COMPONENT_ACTION(SchedulerChannelHolder, getChannel);
        //HPX_DEFINE_COMPONENT_DIRECT_ACTION(SchedulerChannelHolder, getChannelForAction, getChannelAction);
        //static ThreadState getChannel(hpx::id_type id);

        //void setChannelForAction(int num,ThreadState threadState);
        //HPX_DEFINE_COMPONENT_DIRECT_ACTION(SchedulerChannelHolder, setChannelForAction, setChannelAction);
        //static void setChannel(hpx::id_type id, int num, ThreadState threadState);
    //};

}

//HPX_REGISTER_ACTION_DECLARATION(Workstealing::SchedulerChannelHolder::getChannelAction, schedulerChannelHolder_getChannel_action);
//HPX_REGISTER_ACTION_DECLARATION(Workstealing::SchedulerChannelHolder::setChannelAction, schedulerChannelHolder_setChannel_action);

#endif // SCHEDULARCHANNELS_HPP
