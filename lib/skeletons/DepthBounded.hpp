#ifndef SKELETONS_DEPTHSPAWN_HPP
#define SKELETONS_DEPTHSPAWN_HPP

#include <iostream>
#include <vector>
#include <cstdint>

#include <boost/format.hpp>

#include "API.hpp"

#include <hpx/collectives/broadcast.hpp>
#include <hpx/iostream.hpp>

#include "util/NodeGenerator.hpp"
#include "util/Registry.hpp"
#include "util/Incumbent.hpp"
#include "util/func.hpp"

#include "Common.hpp"

#include "workstealing/Scheduler.hpp"
#include "workstealing/policies/Workpool.hpp"
//#include "workstealing/policies/DepthPoolPolicy.hpp"
#include "workstealing/policies/PerformancePolicy.hpp"

namespace YewPar { namespace Skeletons {

namespace DepthBounded_ {

template <typename Generator, typename ...Args>
struct SubtreeTask;

}

// This skeleton allows spawning all tasks into a workqueue based policy based on some depth limit
template <typename Generator, typename ...Args>
struct DepthBounded {
  typedef typename Generator::Nodetype Node;
  typedef typename Generator::Spacetype Space;

  typedef typename API::skeleton_signature::bind<Args...>::type args;

  static constexpr bool isEnumeration = parameter::value_type<args, API::tag::Enumeration_, std::integral_constant<bool, false> >::type::value;
  static constexpr bool isOptimisation = parameter::value_type<args, API::tag::Optimisation_, std::integral_constant<bool, false> >::type::value;
  static constexpr bool isDecision = parameter::value_type<args, API::tag::Decision_, std::integral_constant<bool, false> >::type::value;
  static constexpr bool isDepthLimited = parameter::value_type<args, API::tag::DepthLimited_, std::integral_constant<bool, false> >::type::value;
  static constexpr bool pruneLevel = parameter::value_type<args, API::tag::PruneLevel_, std::integral_constant<bool, false> >::type::value;

  typedef typename parameter::value_type<args, API::tag::Verbose_, std::integral_constant<unsigned, 0> >::type Verbose;
  static constexpr unsigned verbose = Verbose::value;

  typedef typename parameter::value_type<args, API::tag::BoundFunction, nullFn__>::type boundFn;
  typedef typename boundFn::return_type Bound;
  typedef typename parameter::value_type<args, API::tag::ObjectiveComparison, std::greater<Bound> >::type Objcmp;
  typedef typename parameter::value_type<args, API::tag::Enumerator, IdentityEnumerator<Node>>::type Enum;

  //typedef typename parameter::value_type<args, API::tag::DepthBoundedPoolPolicy, Workstealing::Policies::DepthPoolPolicy>::type Policy;
  typedef typename parameter::value_type<args, API::tag::DepthBoundedPoolPolicy, Workstealing::Policies::PerformancePolicy>::type Policy;

  static void printSkeletonDetails(const API::Params<Bound> & params) {
    hpx::cout << "Skeleton Type: DepthBounded\n";
    hpx::cout << "d_cutoff: " << params.spawnDepth << "\n";
    hpx::cout << "Enumeration : " << std::boolalpha << isEnumeration << "\n";
    hpx::cout << "Optimisation: " << std::boolalpha << isOptimisation << "\n";
    hpx::cout << "Decision: " << std::boolalpha << isDecision << "\n";
    hpx::cout << "DepthLimited: " << std::boolalpha << isDepthLimited << "\n";
    if constexpr(!std::is_same<boundFn, nullFn__>::value) {
        hpx::cout << "Using Bounding: true\n";
        hpx::cout << "PruneLevel Optimisation: " << std::boolalpha << pruneLevel << "\n";
      } else {
      hpx::cout << "Using Bounding: false\n";
    }
    if constexpr (std::is_same<Policy, Workstealing::Policies::Workpool>::value) {
      hpx::cout << "Workpool: Deque\n";
    } else {
      hpx::cout << "Workpool: DepthPool\n";
    }
    hpx::cout << std::flush;
  }

  static void expandWithSpawns(const Space & space,
                               const Node & n,
                               const API::Params<Bound> & params,
                               Enum & acc,
                               std::vector<hpx::future<void> > & childFutures,
                               const unsigned childDepth) {
    Generator newCands = Generator(space, n);

    if constexpr(isDepthLimited) {
        if (childDepth == params.maxDepth) {
          return;
        }
    }

    for (auto i = 0; i < newCands.numChildren; ++i) {
      auto c = newCands.next();

      auto pn = ProcessNode<Space, Node, Args...>::processNode(params, space, c, acc);
      if (pn == ProcessNodeRet::Exit) { return; }
      else if (pn == ProcessNodeRet::Break) { break; }
      //default continue

      // Spawn new tasks for all children (that are still alive after pruning)
      childFutures.push_back(createTask(childDepth + 1, c));
    }
  }

  static void expandNoSpawns(const Space & space,
                             const Node & n,
                             const API::Params<Bound> & params,
                             Enum & acc,
                             const unsigned childDepth) {
    auto reg = Registry<Space, Node, Bound, Enum>::gReg;
    Generator newCands = Generator(space, n);

    if constexpr(isDecision) {
        if (reg->stopSearch) {
          return;
        }
      }

    if constexpr(isDepthLimited) {
        if (childDepth == params.maxDepth) {
          return;
        }
    }

    for (auto i = 0; i < newCands.numChildren; ++i) {
      auto c = newCands.next();

      auto pn = ProcessNode<Space, Node, Args...>::processNode(params, space, c, acc);
      if (pn == ProcessNodeRet::Exit) { return; }
      else if (pn == ProcessNodeRet::Prune) { continue; }
      else if (pn == ProcessNodeRet::Break) { break; }

      expandNoSpawns(space, c, params, acc, childDepth + 1);
    }
  }

  static void subtreeTask(const Node taskRoot,
                          const unsigned childDepth,
                          const hpx::id_type donePromiseId) {
    auto reg = Registry<Space, Node, Bound, Enum>::gReg;

    Enum acc;
    std::vector<hpx::future<void> > childFutures;

    if (childDepth <= reg->params.spawnDepth) {
      expandWithSpawns(reg->space, taskRoot, reg->params, acc, childFutures, childDepth);
    } else {
      expandNoSpawns(reg->space, taskRoot, reg->params, acc, childDepth);
    }

    // Atomically updates the (process) local enumerator
    if constexpr (isEnumeration) {
      reg->updateEnumerator(acc);
    }

    termination_wait_act act;
    hpx::apply(act, hpx::find_here(), std::move(childFutures), donePromiseId);
  }

  static hpx::future<void> createTask(const unsigned childDepth,
                                      const Node & taskRoot) {
    hpx::distributed::promise<void> prom;
    auto pfut = prom.get_future();
    auto pid  = prom.get_id();

    DepthBounded_::SubtreeTask<Generator, Args...> t;
    hpx::distributed::function<void(hpx::id_type)> task;
    task = hpx::bind(t, hpx::placeholders::_1, taskRoot, childDepth, pid);

    auto workPool = std::static_pointer_cast<Policy>(Workstealing::Scheduler::local_policy);
    if constexpr (std::is_same<Policy, Workstealing::Policies::Workpool>::value) {
      workPool->addwork(task);
    } else {
      workPool->addwork(task, childDepth - 1);
    }

     return pfut;
  }

  static auto search (const Space & space,
                      const Node & root,
                      const API::Params<Bound> params = API::Params<Bound>()) {
    if constexpr (verbose) {
        printSkeletonDetails(params);
    }

    hpx::wait_all(hpx::lcos::broadcast<InitRegistryAct<Space, Node, Bound, Enum> >(
        hpx::find_all_localities(), space, root, params));

    Policy::initPolicy();

    auto threadCount = hpx::get_os_thread_count() == 1 ? 1 : hpx::get_os_thread_count() - 1;
    hpx::wait_all(hpx::lcos::broadcast<Workstealing::Scheduler::startSchedulers_act>(
        hpx::find_all_localities(), threadCount));

    if constexpr(isOptimisation || isDecision) {
      auto inc = hpx::new_<Incumbent>(hpx::find_here()).get();
      hpx::wait_all(hpx::lcos::broadcast<UpdateGlobalIncumbentAct<Space, Node, Bound, Enum> >(
          hpx::find_all_localities(), inc));
      initIncumbent<Space, Node, Bound, Enum, Objcmp, Verbose>(root, params.initialBound);
    }

    // Ensure the root node is accumulated if required
    if constexpr(isEnumeration) {
        Enum acc;
        acc.accumulate(root);
        Registry<Space, Node, Bound, Enum>::gReg->updateEnumerator(acc);
    }

    createTask(1, root).get();

    hpx::wait_all(hpx::lcos::broadcast<Workstealing::Scheduler::stopSchedulers_act>(
        hpx::find_all_localities()));

    // Return the right thing
    if constexpr(isEnumeration) {
      return combineEnumerators<Space, Node, Bound, Enum>();
    } else if constexpr(isOptimisation || isDecision) {
      auto reg = Registry<Space, Node, Bound, Enum>::gReg;

      typedef typename Incumbent::GetIncumbentAct<Node, Bound, Objcmp, Verbose> getInc;
      return hpx::async<getInc>(reg->globalIncumbent).get();
    } else {
      static_assert(isEnumeration || isOptimisation || isDecision, "Please provide a supported search type: Enumeration, Optimisation, Decision");
    }
  }
};

namespace DepthBounded_{

template <typename Generator, typename ...Args>
struct SubtreeTask : hpx::actions::make_action<
  decltype(&DepthBounded<Generator, Args...>::subtreeTask),
  &DepthBounded<Generator, Args...>::subtreeTask,
  SubtreeTask<Generator, Args...>>::type {};

}

}}

namespace hpx { namespace traits {

template <typename Generator, typename ...Args>
struct action_stacksize<YewPar::Skeletons::DepthBounded_::SubtreeTask<Generator, Args...> > {
  static constexpr threads::thread_stacksize value = threads::thread_stacksize::huge;
};

}}

#endif
