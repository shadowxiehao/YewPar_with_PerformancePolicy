#include "DepthPool.hpp"

namespace workstealing {

    DepthPool::fnType DepthPool::steal() {
      DepthPool::fnType task;
      std::unique_lock lp(poolsMutex);
      for (auto i = 0; i <= lowest; ++i) {
        if (!pools[i].empty()) {
          auto task = pools[i].front();
          pools[i].pop();
          --tasks_count;

          return task;
        }
      }
      return nullptr;
    }

    DepthPool::fnType DepthPool::getLocal() {
      DepthPool::fnType task;
      std::unique_lock lp(poolsMutex);
     
      while (true) {
          if (!pools[lowest].empty()) {
              task = pools[lowest].front();
              pools[lowest].pop();
              --tasks_count;
              return task;
          }
          if (lowest == 0) {
              break;
          }
          --lowest;
      }
      return nullptr;
    }

    void DepthPool::addWork(DepthPool::fnType task, unsigned depth) {
      // Resize if we need to. We don't want this to happen too often, so we double it if we need to.
        std::unique_lock lp(poolsMutex);
        if (depth >= max_depth) {
        max_depth = max_depth * 2;
        pools.resize(max_depth);
      }

      pools[depth].push(task);
      ++tasks_count;

      if (depth > lowest) {
        lowest = depth;
      }
    }

    unsigned DepthPool::getTasksCount() {
        return tasks_count;
    }

    double DepthPool::getWorkRate() {
        return workRate;
    }

    void DepthPool::setWorkRate(double workRate) {
        this->workRate = workRate;
    }
}

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::component<workstealing::DepthPool> DepthPool_type;

HPX_REGISTER_COMPONENT(DepthPool_type, DepthPool);

HPX_REGISTER_ACTION(workstealing::DepthPool::getLocal_action, DepthPool_getLocal_action);
HPX_REGISTER_ACTION(workstealing::DepthPool::steal_action, DepthPool_steal_action);
HPX_REGISTER_ACTION(workstealing::DepthPool::addWork_action, DepthPool_addWork_action);

HPX_REGISTER_ACTION(workstealing::DepthPool::getTasksCount_action, DepthPool_getTasksCount_action);
HPX_REGISTER_ACTION(workstealing::DepthPool::getWorkRate_action, DepthPool_getWorkRate_action);
HPX_REGISTER_ACTION(workstealing::DepthPool::setWorkRate_action, DepthPool_setWorkRate_action);
