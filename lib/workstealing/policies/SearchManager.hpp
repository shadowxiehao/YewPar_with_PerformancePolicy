#ifndef YEWPAR_SEARCHMANAGER_COMPONENT_HPP
#define YEWPAR_SEARCHMANAGER_COMPONENT_HPP

#include <iterator>                                              // for advance
#include <memory>                                                // for allo...
#include <random>                                                // for defa...
#include <vector>                                                // for vector
#include <utility>                                               // for vector
#include <queue>
#include <atomic>
#include <unordered_map>

#include <hpx/include/components.hpp>

#include "hpx/performance_counters/manage_counter_type.hpp"    // for instal...
#include "hpx/lcos/async.hpp"                                    // for async
#include "hpx/lcos/detail/future_data.hpp"                       // for task...
#include "hpx/lcos/detail/promise_lco.hpp"                       // for prom...
#include "hpx/lcos/future.hpp"                                   // for future
#include "hpx/lcos/local/channel.hpp"                            // for future
#include "hpx/lcos/local/mutex.hpp"
#include "hpx/runtime/actions/plain_action.hpp"
#include "hpx/runtime/actions/basic_action.hpp"                  // for make...
#include "hpx/runtime/actions/component_action.hpp"              // for HPX_...
#include "hpx/runtime/actions/transfer_action.hpp"               // for tran...
#include "hpx/runtime/actions/transfer_continuation_action.hpp"  // for tran...
#include "hpx/runtime/components/new.hpp"                        // for loca...
#include "hpx/runtime/components/server/component_base.hpp"      // for comp...
#include "hpx/runtime/components/server/locking_hook.hpp"        // for lock...
#include "hpx/runtime/find_here.hpp"                             // for find...
#include "hpx/runtime/naming/id_type.hpp"                        // for id_type
#include "hpx/runtime/naming/name.hpp"                           // for intr...
#include "hpx/runtime/serialization/binary_filter.hpp"           // for bina...
#include "hpx/runtime/serialization/serialize.hpp"               // for oper...
#include "hpx/runtime/serialization/shared_ptr.hpp"
#include "hpx/runtime/threads/executors/default_executor.hpp"
#include "hpx/runtime/threads/thread_data_fwd.hpp"               // for get_...
#include "hpx/traits/is_action.hpp"                              // for is_a...
#include "hpx/traits/needs_automatic_registration.hpp"           // for need...
#include "hpx/util/bind.hpp"                                     // for bound
#include "hpx/util/function.hpp"                                 // for func...
#include "hpx/util/unused.hpp"                                   // for unus...
#include "hpx/util/tuple.hpp"
#include "hpx/lcos/broadcast.hpp"
#include "hpx/runtime/get_ptr.hpp"
#include "hpx/util/lockfree/deque.hpp"

#include "Policy.hpp"
#include "workstealing/Scheduler.hpp"
#include "util/util.hpp"

namespace Workstealing { namespace Policies {

namespace SearchManagerPerf {
// Performance Counters
std::atomic<std::uint64_t> perf_localSteals(0);
std::atomic<std::uint64_t> perf_distributedSteals(0);
std::atomic<std::uint64_t> perf_failedLocalSteals(0);
std::atomic<std::uint64_t> perf_failedDistributedSteals(0);

std::vector<std::pair<hpx::naming::id_type, bool> > distributedStealsList;

void registerPerformanceCounters();

void printDistributedStealsList();
HPX_DEFINE_PLAIN_ACTION(printDistributedStealsList, printDistributedStealsList_act);

}}}

namespace Workstealing { namespace Policies {

// The SearchManager component allows steals to happen directly within a
// searching thread. The SearchManager maintains a list of active threads and
// uses this to perform steals when work is requested from the scheduler. Thread
// response is generic allowing responses to be enumeration Nodes, B&B Nodes or
// even paths for recompute based skeletons
struct SearchManager : public hpx::components::component_base<SearchManager> {

  // A type erased ptr to the actual component type SearchManagerComp This avoids
  // needing a REGISTER_X macro per type as would be required for a templated
  // component. Instead we use templated actions to constrain the type. The API
  // is a little strange, but that's okay since it should only be called form
  // the library, never user code.
  // This ptr is shared with the local scheduler to ask the manager for more work.
  std::shared_ptr<void> ptr;

  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  void init(bool stealAll) {
    ptr = std::shared_ptr<void>(new SearchManagerComp<SearchInfo, FuncToCall, Args...>(stealAll));
  }

  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  struct InitComponentAct : hpx::actions::make_action<
    decltype(&SearchManager::init<SearchInfo, FuncToCall, Args...>),
    &SearchManager::init<SearchInfo, FuncToCall, Args...>,
    InitComponentAct<SearchInfo, FuncToCall, Args...> >::type {};

  // The actual typed component that we are wrapping
  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  class SearchManagerComp : public Policy {
   private:

    // Information returned on a steal from a thread
    using Task = hpx::util::tuple<SearchInfo, int, hpx::naming::id_type>;

    // We return an empty vector here to signal no tasks
    using Response = std::vector<Task>;

    // Information shared between a thread and the manager. We set the atomic on a steal and then use the channel to await a response
    using SharedState = std::tuple<std::atomic<bool>, hpx::lcos::local::one_element_channel<Response>, bool>;

    // Lock to protect the component
    using MutexT = hpx::lcos::local::mutex;
    MutexT mtx;

    // (internal) Id's of currently running threads, for managing the active map
    std::queue<unsigned> activeIds;

    // Active thread shared states
    std::unordered_map<unsigned, std::shared_ptr<SharedState> > active;

    // Shared states of threads currently being stolen from
    std::unordered_map<unsigned, std::shared_ptr<SharedState> > inactive;

    // Pointers to SearchManagers on other localities
    std::vector<hpx::naming::id_type> distributedSearchManagers;

    // random number generator
    std::mt19937 randGenerator;

    // Are we currently doing a distributed steal?
    bool isStealingDistributed = false;

    // Task Buffer for chunking
    boost::lockfree::deque<Task> taskBuffer;

    // Should we steal all tasks at a level when we perform a steal?
    bool stealAll = false;

    // Last steal optimisation
    hpx::naming::id_type last_remote;

    // Try to steal from a thread on another (random) locality
    Response tryDistributedSteal(std::unique_lock<MutexT> & l) {
      // We only allow one distributed steal to happen at a time (to make sure we
      // don't overload the communication)
      if (isStealingDistributed) {
        return {};
      }

      isStealingDistributed = true;

      // Last steal optimisation
      hpx::naming::id_type victim;
      if (last_remote != hpx::find_here()) {
        victim = last_remote;
      } else {
        auto vic = distributedSearchManagers.begin();

        std::uniform_int_distribution<> rand(0, distributedSearchManagers.size() - 1);
        std::advance(vic, rand(randGenerator));
        victim = *vic;
      }

      l.unlock();
      auto res = hpx::async<GetDistributedWorkAct<SearchInfo, FuncToCall, Args...> >(victim).get();
      l.lock();

      isStealingDistributed = false;

      if (!res.empty()) {
        SearchManagerPerf::distributedStealsList.push_back(std::make_pair(victim, true));
        last_remote = victim;
      } else {
        SearchManagerPerf::distributedStealsList.push_back(std::make_pair(victim, false));
        last_remote = hpx::find_here();
      }

      return res;
    }

   public:

    SearchManagerComp() : SearchManagerComp(false) {}

    SearchManagerComp(bool stealAll) : stealAll(stealAll){
      for (auto i = 0; i < hpx::get_os_thread_count(); ++i) {
        activeIds.push(i);
      }

      std::random_device rd;
      randGenerator.seed(rd());

      last_remote = hpx::find_here();
    }

    // Notify this search manager of the globalId's of potential steal victims
    void registerDistributedManagers(std::vector<hpx::naming::id_type> distributedSearchMgrs) {
      std::lock_guard<MutexT> l(mtx);
      distributedSearchManagers = distributedSearchMgrs;
      distributedSearchManagers.erase(
          std::remove_if(distributedSearchManagers.begin(), distributedSearchManagers.end(), YewPar::util::isColocated),
          distributedSearchManagers.end());
    }

    // Try to get work from a (random) thread running on this locality and wrap it
    // back up for serializing over the network
    Response getDistributedWork() {
      std::unique_lock<MutexT> l(mtx);
      return getLocalWork(l);
    }

    // Try to get work from a (random) thread running on this locality
    Response getLocalWork(std::unique_lock<MutexT> & l) {
      if (active.empty()) {
        return {};
      }
      auto victim = active.begin();

      std::uniform_int_distribution<> rand(0, active.size() - 1);
      std::advance(victim, rand(randGenerator));

      auto pos         = victim->first;
      auto stealReqPtr = victim->second;

      // We remove the victim from active while we steal, so that if we suspend
      // no other thread gets in the way of our steal
      active.erase(pos);
      inactive[pos] = stealReqPtr;

      // Signal the thread that we need work from it and wait for some (or Nothing)
      std::get<0>(*stealReqPtr).store(true);
      auto resF = std::get<1>(*stealReqPtr).get();
      l.unlock();
      auto res = resF.get();
      l.lock();

      // -1 depth signals that the thread we tried to steal from has finished it's search
      if (!res.empty() && hpx::util::get<1>(res[0]) == -1) {
        return {};
      }

      // Allow this thread to be stolen from again
      active[pos] = stealReqPtr;
      inactive.erase(pos);
      return res;
    }

    // Called by the scheduler to ask the searchManager to add more work
    hpx::util::function<void(), false> getWork() override {
      std::unique_lock<MutexT> l(mtx);

      // Return from task buffer first if anything exists
      Task task;
      if (taskBuffer.pop_right(task)) {
        SearchInfo searchInfo; int depth; hpx::naming::id_type prom;
        hpx::util::tie(searchInfo, depth, prom) = task;
        return hpx::util::bind(FuncToCall::fn_ptr(), searchInfo, depth, prom);
      }

      Response maybeStolen;
      if (active.empty()) {
        // No local threads running, steal distributed
        if (!distributedSearchManagers.empty()) {
          maybeStolen = tryDistributedSteal(l);
          if (!maybeStolen.empty()) {
            SearchManagerPerf::perf_distributedSteals++;
          } else {
            SearchManagerPerf::perf_failedDistributedSteals++;
            return nullptr;
          }
        } else {
          SearchManagerPerf::perf_failedLocalSteals++;
          return nullptr;
        }
      } else {
        maybeStolen = getLocalWork(l);
        if (!maybeStolen.empty()) {
          SearchManagerPerf::perf_localSteals++;
        } else {
          SearchManagerPerf::perf_failedLocalSteals++;
          return nullptr;
        }
      }

      if (!maybeStolen.empty()) {
        // Take off the first task and queue up anything else that was returned
        auto first = maybeStolen[0];
        SearchInfo searchInfo; int depth; hpx::naming::id_type prom;
        hpx::util::tie(searchInfo, depth, prom) = first;

        auto itr = maybeStolen.begin();
        ++itr;
        for (itr; itr != maybeStolen.end(); ++itr) {
          taskBuffer.push_left(std::move(*itr));
        }

        return hpx::util::bind(FuncToCall::fn_ptr(), searchInfo, depth, prom);
      }
    }

    // Signal the searchManager that a local thread is now finished working and should be removed from active
    void unregisterThread(unsigned activeId) {
      std::lock_guard<MutexT> l(mtx);
      if (active.find(activeId) != active.end()) {
        active.erase(activeId);
      } else {
        // A steal must be in progress on this id so cancel it before finishing
        // Canceled steals (-1 flag) are already removed from active
        auto & state = inactive[activeId];
        std::vector<Task> noSteal {hpx::util::make_tuple(SearchInfo(), -1, hpx::find_here())};
        std::get<1>(*state).set(noSteal);
        inactive.erase(activeId);
      }
      activeIds.push(activeId);
    }

    // Generate a new stealRequest pair that can be used with an existing thread to add steals to it
    // Used for master-threads initialising work while maintaining a stack
    std::pair<std::shared_ptr<SharedState>, unsigned> registerThread() {
      std::lock_guard<MutexT> l(mtx);
      auto shared_state = std::make_shared<SharedState>();
      auto nextId = activeIds.front();
      activeIds.pop();
      active[nextId] = shared_state;
      return std::make_pair(shared_state, nextId);
    }

    std::vector<hpx::naming::id_type> getAllSearchManagers() {
      std::lock_guard<MutexT> l(mtx);
      std::vector<hpx::naming::id_type> res(distributedSearchManagers);
      res.push_back(this->get_id());
      return res;
    }

    typedef Response Response_t;
    typedef SharedState SharedState_t;

    // Called on each node to set the scheduler policy pointer
    // managerId must exist on the locality this is called on
    static void setLocalPolicy(hpx::naming::id_type managerId) {
      auto searchMgr = hpx::get_ptr<SearchManager>(managerId).get();
      Workstealing::Scheduler::local_policy = searchMgr->getInnerPolicy<SearchInfo, FuncToCall, Args...>();
    }
    struct SetLocalPolicyAct : hpx::actions::make_action<
      decltype(&setLocalPolicy),
      &setLocalPolicy,
      SetLocalPolicyAct>::type {};

    // Helper function to setup the components/policies on each node and register required information
    static void initPolicy(bool stealAll) {
      std::vector<hpx::naming::id_type> searchManagers;
      for (auto const& loc : hpx::find_all_localities()) {
        auto searchManager = hpx::new_<SearchManager>(loc).get();
        hpx::async<InitComponentAct<SearchInfo, FuncToCall, Args...> >(searchManager, stealAll).get();
        hpx::async<SetLocalPolicyAct>(loc, searchManager).get();
        searchManagers.push_back(searchManager);
      }

      for (auto const & mgr : searchManagers) {
        using act = typename SearchManager::RegisterDistributedManagersAct<SearchInfo, FuncToCall, Args...>;
        hpx::async<act>(mgr, searchManagers).get();
      }
    }

  };

  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  std::shared_ptr<SearchManagerComp<SearchInfo, FuncToCall, Args...> > getInnerPolicy() {
    return std::static_pointer_cast<SearchManagerComp<SearchInfo, FuncToCall, Args...> >(ptr);
  }

  // Public component API (managing the types as required)
  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  void registerDistributedManagers(std::vector<hpx::naming::id_type> distributedSearchMgrs) {
    auto p = ptr.get();
    auto comp = static_cast<SearchManagerComp<SearchInfo, FuncToCall, Args...>*>(p);
    comp->registerDistributedManagers(distributedSearchMgrs);
  }
  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  struct RegisterDistributedManagersAct : hpx::actions::make_action<
    decltype(&SearchManager::registerDistributedManagers<SearchInfo, FuncToCall, Args...>),
    &SearchManager::registerDistributedManagers<SearchInfo, FuncToCall, Args...>,
    RegisterDistributedManagersAct<SearchInfo, FuncToCall, Args...> >::type {};

  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  typename SearchManagerComp<SearchInfo, FuncToCall, Args...>::Response_t getDistributedWork() {
    auto p = ptr.get();
    auto comp = static_cast<SearchManagerComp<SearchInfo, FuncToCall, Args...>*>(p);
    return comp->getDistributedWork();
  }
  template <typename SearchInfo, typename FuncToCall, typename ...Args>
  struct GetDistributedWorkAct : hpx::actions::make_action<
    decltype(&SearchManager::getDistributedWork<SearchInfo, FuncToCall, Args...>),
    &SearchManager::getDistributedWork<SearchInfo, FuncToCall, Args...>,
    GetDistributedWorkAct<SearchInfo, FuncToCall, Args...> >::type {};

};

}}

typedef hpx::components::component<Workstealing::Policies::SearchManager> searchMgr_comp;
HPX_REGISTER_COMPONENT(searchMgr_comp);

#endif

