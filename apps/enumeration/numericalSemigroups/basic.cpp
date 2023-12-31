/* Code by Florent Hivert: https://www.lri.fr/~hivert/ */

#include <hpx/hpx_init.hpp>

#include "skeletons/DepthBounded.hpp"

typedef unsigned char uchar;
typedef unsigned short int usint;

struct SemiGroup{
  std::vector<uchar> tab;
  uchar c, m;
  SemiGroup();
  SemiGroup(const SemiGroup& S,uchar x);
  ~SemiGroup();

  template <class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & tab;
    ar & c;
    ar & m;
  }
};

SemiGroup::SemiGroup(){
  c = m = 1;
  tab.resize(c + m);
  tab[0] = 2;
  tab[1] = 1;
}

SemiGroup::~SemiGroup(){
}

SemiGroup::SemiGroup(const SemiGroup& S,uchar x){
  usint i,j;
  uchar nb_irr=0;
  uchar s=S.m+S.c;
  c=x+1;
  m=(x==S.m)?m=S.m+1:S.m;
  uchar irr[m];
  uchar v;
  tab.resize(c + m);
  for(i=0;i<x;++i){
    v=S.tab[i];
    tab[i]=v;
    if(v==1){
      irr[nb_irr++]=i;
    }
  }
  tab[x]=0;
  for(i=x+1;i<c+m;++i){
    v=(i<s)?S.tab[i]:2;
    if(v==1){
      tab[i]=1;
      irr[nb_irr++]=i;
    }
    else{
      for(j=0;j<nb_irr;++j){
        if(tab[i-irr[j]]!=0) break;
      }
      if(j==nb_irr){
        tab[i]=1;
        irr[nb_irr++]=i;
      }
      else tab[i]=2;
    }
  }
}

// Numerical Semigroups don't have a space
struct Empty {};

struct NodeGen : YewPar::NodeGenerator<SemiGroup, Empty> {
  const SemiGroup & group;
  unsigned it;

  NodeGen(const Empty & space, const SemiGroup & s) : group(s) {
    it = group.c;

    // TODO: Factor into SemiGroup functionality
    auto children = 0;
    for (auto i = group.c; i < group.c + group.m; ++i) {
      if (group.tab[i] == 1) {
        children++;
      }
    }
    this->numChildren = children;
  }

  SemiGroup next() override {
    if (group.tab[it] == 1) {
      auto s = SemiGroup(group, it);
      it++;
      return s;
    } else {
      it++;
      return next();
    }
  }
};

struct CountNodes : YewPar::Enumerator<SemiGroup, std::uint64_t> {
  std::uint64_t count;
  CountNodes() = default;

  void accumulate(const SemiGroup & n) override {
    count++;
  }

  void combine(const std::uint64_t & other) override {
    count += other;
  }

  std::uint64_t get() override { return count; }
};

int hpx_main(hpx::program_options::variables_map & opts) {
  auto spawnDepth = opts["spawn-depth"].as<unsigned>();
  auto maxDepth   = opts["until-depth"].as<unsigned>();

  YewPar::Skeletons::API::Params<> searchParameters;
  searchParameters.maxDepth   = maxDepth;
  searchParameters.spawnDepth = spawnDepth;

  auto count = YewPar::Skeletons::DepthBounded<NodeGen,
                                               YewPar::Skeletons::API::Enumeration,
                                               YewPar::Skeletons::API::Enumerator<CountNodes>,
                                               YewPar::Skeletons::API::DepthLimited>
                ::search(Empty(), SemiGroup(), searchParameters);

  std::cout << "Total nodes: " << count << std::endl;
  std::cout << "=====" << std::endl;

  return hpx::finalize();
}

int main(int argc, char* argv[]) {
  hpx::program_options::options_description
    desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

  desc_commandline.add_options()
    ( "spawn-depth,s",
      hpx::program_options::value<unsigned>()->default_value(0),
      "Depth in the tree to spawn until (for parallel skeletons only)"
    )
    ( "until-depth,d",
      hpx::program_options::value<unsigned>()->default_value(0),
      "Depth in the tree to count until"
    );
  hpx::init_params args;
  args.desc_cmdline = desc_commandline;
  return hpx::init(argc, argv, args);
}
