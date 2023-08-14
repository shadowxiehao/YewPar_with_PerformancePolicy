#ifndef SCHEDULERCHANNELS_HPP
#define SCHEDULERCHANNELS_HPP

#include <hpx/include/lcos.hpp>
#include <hpx/runtime_distributed/find_localities.hpp>
#include <hpx/iostream.hpp>
#include <hpx/chrono.hpp>
#include <atomic>

namespace Workstealing {

    extern std::size_t thread_count; //hpx::get_os_thread_count
    extern std::uint32_t local_id_num; //record hpx::get_locality_id
    extern unsigned locality_count;
    inline hpx::id_type local_id_type;

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

        std::vector<Record> recordVector;

        std::vector< std::unique_ptr<std::atomic<double>>> workRateVector;

    public:
        SchedulerChannels()=default;
        void init();
        unsigned getSize() const;
        ThreadState getState(unsigned id) const;
        Record getRecord(unsigned id);
        double getWorkRate(unsigned id) const;
        double getWorkRateSum() const;
        double getWorkRateAverage() const;

        void setState(unsigned id,ThreadState threadState);
    };

}

#endif // SCHEDULARCHANNELS_HPP
