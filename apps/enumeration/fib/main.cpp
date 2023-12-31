#include <hpx/hpx_init.hpp>

#include <vector>
#include <memory>
#include <chrono>

#include "skeletons/Seq.hpp"
#include "skeletons/DepthBounded.hpp"
#include "skeletons/StackStealing.hpp"
#include "util/NodeGenerator.hpp"

// Fib doesn't have a space
struct Empty {};

struct NodeGen : YewPar::NodeGenerator<std::uint64_t, Empty> {
  std::uint64_t n;
  unsigned i = 1;

  NodeGen() = default;

  NodeGen(const Empty & space, const std::uint64_t & n) : n(n) {
    this->numChildren = 2;
  }

  std::uint64_t next() override {
    auto res = n - i;
    ++i;
    return res;
  }
};

NodeGen generateChildren(const Empty & space, const std::uint64_t & n) {
  return NodeGen(space, n);
}

struct CountNodes : YewPar::Enumerator<std::uint64_t, std::uint64_t> {
  std::uint64_t count;
  CountNodes() : count(0) {}

  void accumulate(const std::uint64_t & n) override {
    count++;
  }

  void combine(const std::uint64_t & other) override {
    count += other;
  }

  std::uint64_t get() override { return count; }
};

#define MAX_DEPTH 50


int hpx_main(hpx::program_options::variables_map & opts) {
  auto spawnDepth = opts["spawn-depth"].as<unsigned>();
  auto maxDepth   = opts["until-depth"].as<unsigned>();
  auto skeleton   = opts["skeleton-type"].as<std::string>();

  auto start_time = std::chrono::steady_clock::now();

  std::uint64_t count;
  if (skeleton == "seq") {
    YewPar::Skeletons::API::Params<> searchParameters;
    searchParameters.maxDepth = maxDepth;
    count = YewPar::Skeletons::Seq<NodeGen,
                                    YewPar::Skeletons::API::Enumeration,
                                    YewPar::Skeletons::API::Enumerator<CountNodes>,
                                    YewPar::Skeletons::API::DepthLimited>
             ::search(Empty(), maxDepth - 1, searchParameters);
  } else if (skeleton == "depthbounded"){
    YewPar::Skeletons::API::Params<> searchParameters;
    searchParameters.maxDepth   = maxDepth;
    searchParameters.spawnDepth = spawnDepth;
    count = YewPar::Skeletons::DepthBounded<NodeGen,
                                    YewPar::Skeletons::API::Enumeration,
                                    YewPar::Skeletons::API::Enumerator<CountNodes>,
                                    YewPar::Skeletons::API::DepthLimited>
             ::search(Empty(), maxDepth - 1, searchParameters);
  } else if (skeleton == "stacksteal"){
    YewPar::Skeletons::API::Params<> searchParameters;
    searchParameters.maxDepth = maxDepth;
    count = YewPar::Skeletons::StackStealing<NodeGen,
                                              YewPar::Skeletons::API::Enumeration,
                                              YewPar::Skeletons::API::Enumerator<CountNodes>,
                                              YewPar::Skeletons::API::DepthLimited>
             ::search(Empty(), maxDepth - 1, searchParameters);
  }

  auto overall_time = std::chrono::duration_cast<std::chrono::milliseconds>
                      (std::chrono::steady_clock::now() - start_time);

  std::cout << "Total nodes: " << count << std::endl;
  std::cout << "=====" << std::endl;
  std::cout << "cpu = " << overall_time.count() << std::endl;

  return hpx::finalize();
}

int main(int argc, char* argv[]) {
  hpx::program_options::options_description
      desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

  desc_commandline.add_options()
      ( "skeleton-type",
        hpx::program_options::value<std::string>()->default_value("seq"),
        "Which skeleton to use"
        )
      ( "spawn-depth,s",
        hpx::program_options::value<unsigned>()->default_value(0),
        "Depth in the tree to spawn until (for parallel skeletons only)"
        )
      ( "until-depth,d",
        hpx::program_options::value<unsigned>()->default_value(0),
        "Depth in the tree to count until"
        );

  hpx::register_startup_function(&Workstealing::Policies::SearchManagerPerf::registerPerformanceCounters);

  hpx::init_params args;
  args.desc_cmdline = desc_commandline;
  return hpx::init(argc, argv, args);
}
