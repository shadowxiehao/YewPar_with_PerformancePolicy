#include "SchedulerChannels.hpp"
#include <hpx/parallel/algorithms/for_loop.hpp>

#include "workstealing/DepthPool.hpp"

namespace Workstealing {

    std::size_t thread_count; //hpx::get_os_thread_count
    std::uint32_t local_id_num; //record hpx::get_locality_id
    unsigned locality_count;

    void SchedulerChannels::init() {
        workRateVector.reserve(thread_count);

        for (unsigned i = 0; i < thread_count; ++i) {
            workRateVector.push_back(std::make_unique<std::atomic<double>>(WORK_RATE_INIT_VALUE));
        }

        recordVector.assign(thread_count, Record());
    }

    void SchedulerChannels::setState(unsigned id, ThreadState threadState) {
        
        if(recordVector[id].threadState == ThreadState::IDLE && threadState == ThreadState::WORKING) {
            //if idle->work 1.record idle time 2.restart timer 3.set ThreadState->work
            recordVector[id].idleTime = recordVector[id].timer.elapsed_microseconds();
            recordVector[id].timer.restart();
            recordVector[id].threadState = threadState;
        }else if(recordVector[id].threadState == ThreadState::WORKING && threadState == ThreadState::IDLE){
            //if work->idle 1.record work time 2.calculate the working rate 3.restart timer 4. set ThreadState->idle
            recordVector[id].workTime = recordVector[id].timer.elapsed_microseconds();
            //use Exponential Weighted Moving Average, EWMA; but int64->double may lose some accuracy, just for speed.
            double workTime = static_cast<double>(recordVector[id].workTime);
            double idleTime = static_cast<double>(recordVector[id].idleTime);
            
            *workRateVector[id] =
                std::log(2.72 + (workTime / (workTime + idleTime))) * std::log(2.72 + (workTime + idleTime) ) * 0.65
                + *workRateVector[id] * 0.35;
            recordVector[id].timer.restart();
            recordVector[id].threadState = threadState;
        }else {
            //set ThreadState
            recordVector[id].threadState = threadState;
        }

    }

    unsigned SchedulerChannels::getSize() const {
            return thread_count;
    }

    ThreadState SchedulerChannels::getState(unsigned id) const {
        return recordVector[id].threadState;
    }

    SchedulerChannels::Record SchedulerChannels::getRecord(unsigned id) {
        return recordVector[id];
    }

    double SchedulerChannels::getWorkRate(unsigned id) const {
        return *workRateVector[id];
    }

    double SchedulerChannels::getWorkRateSum() const {
        double totalWorkRate = 0.0;
        for (unsigned i = 0; i < recordVector.size(); ++i) {
            totalWorkRate += getWorkRate(i);
        }
        return totalWorkRate;
    }

    double SchedulerChannels::getWorkRateAverage() const {
        return getWorkRateSum() / static_cast<double>(thread_count);
    }

}
