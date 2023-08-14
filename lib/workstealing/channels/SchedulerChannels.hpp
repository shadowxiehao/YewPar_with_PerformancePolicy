#ifndef SCHEDULERCHANNELS_HPP
#define SCHEDULERCHANNELS_HPP

#include <hpx/include/lcos.hpp>
//#include <hpx/channel.hpp>
#include <hpx/include/unordered_map.hpp>
#include <hpx/runtime_distributed/find_localities.hpp>
#include <hpx/iostream.hpp>
#include <hpx/chrono.hpp>
#include <hpx/numeric.hpp>
#include <atomic>

HPX_REGISTER_UNORDERED_MAP_DECLARATION(std::string, double)
namespace Workstealing {

    extern std::size_t thread_count; //hpx::get_os_thread_count
    extern std::uint32_t local_id_num; //record hpx::get_locality_id
    extern unsigned locality_count;
    inline hpx::id_type local_id_type;
    //global
    /*extern std::shared_ptr<hpx::unordered_map<std::string, double>> globalChannelMap;
    extern std::string globalChannelMapName;
    extern std::string globalWorkRateAverageName;
    extern std::string getWorkRateAverageNameById(std::uint32_t id);*/

    constexpr double WORK_RATE_INIT_VALUE = 1.0;

    inline double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }
    
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
    class SchedulerChannels {

    private:

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
        //SchedulerChannels()=default;
        SchedulerChannels()=default;
        void init();
        unsigned getSize() const;
        ThreadState getState(unsigned id) const;
        Record getRecord(unsigned id);
        double getWorkRate(unsigned id) const;
        double getWorkRateSum();
        double getWorkRateAverage();

        void setState(unsigned id,ThreadState threadState);
        
    };


    
//    extern SchedulerChannels schedulerChannelHolder;

    //channels
    //extern hpx::unordered_map<unsigned, ThreadState> threadStateMap;

    // Create a component to hold the channel
    //class SchedulerChannels : public hpx::components::component_base<SchedulerChannels> {
    //class SchedulerChannels {

    //private:
    //    hpx::mutex local_mutex;
    //    unsigned local_id_num;

    //public:
        //SchedulerChannels()=default;
        //SchedulerChannels();

        //SchedulerChannels(hpx::lcos::channel<int> c);

        //hpx::lcos::channel<int> c_;
        //std::unordered_map<unsigned, ThreadState> threadStateMap;

        //void initChannels();

        //ThreadState getChannelForAction();
        ////HPX_DEFINE_COMPONENT_ACTION(SchedulerChannels, getChannel);
        //HPX_DEFINE_COMPONENT_DIRECT_ACTION(SchedulerChannels, getChannelForAction, getChannelAction);
        //static ThreadState getChannel(hpx::id_type id);

        //void setChannelForAction(int num,ThreadState threadState);
        //HPX_DEFINE_COMPONENT_DIRECT_ACTION(SchedulerChannels, setChannelForAction, setChannelAction);
        //static void setChannel(hpx::id_type id, int num, ThreadState threadState);
    //};

}

//HPX_REGISTER_ACTION_DECLARATION(Workstealing::SchedulerChannels::getChannelAction, schedulerChannelHolder_getChannel_action);
//HPX_REGISTER_ACTION_DECLARATION(Workstealing::SchedulerChannels::setChannelAction, schedulerChannelHolder_setChannel_action);

#endif // SCHEDULARCHANNELS_HPP
