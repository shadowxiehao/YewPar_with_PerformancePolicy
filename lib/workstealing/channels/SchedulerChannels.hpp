#ifndef SCHEDULERCHANNELS_HPP
#define SCHEDULERCHANNELS_HPP

#include <hpx/include/lcos.hpp>
//#include <hpx/channel.hpp>
#include <hpx/include/unordered_map.hpp>
#include <hpx/runtime_distributed/find_localities.hpp>
#include <hpx/iostream.hpp>
#include <hpx/chrono.hpp>
#include <hpx/numeric.hpp>

HPX_REGISTER_UNORDERED_MAP_DECLARATION(std::string, double)
namespace Workstealing {

    //extern hpx::unordered_map<unsigned, double> workRatesGlobalMap;
    extern std::string globalChannelMapName;
    extern std::shared_ptr<hpx::unordered_map<std::string, double>> workRatesGlobalMap;
    extern std::string globalWorkRateAverageName;

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
        std::size_t thread_count;

        struct Record {
            hpx::chrono::high_resolution_timer timer;
            ThreadState threadState = ThreadState::Dead;
            std::int64_t idleTime = 1;
            std::int64_t workTime = 1;
        };

        //std::vector<std::unique_ptr<hpx::shared_mutex>> recordMutexs;
        std::vector<std::unique_ptr<hpx::mutex>> recordMutexs;
        std::vector<Record> recordVector;

        std::vector<std::unique_ptr<hpx::mutex>> workRateMutexs;
        std::vector<double> workRateVector;

    public:
        //SchedulerChannelHolder()=default;
        SchedulerChannelHolder()=default;
        void init();
        unsigned getSize() const;
        ThreadState getState(unsigned id) const;
        Record getRecord(unsigned id);
        double getWorkRate(unsigned id) const;
        double getWorkRateSum();
        double getWorkRateAverage();

        void setState(unsigned id,ThreadState threadState);
    };


    
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
