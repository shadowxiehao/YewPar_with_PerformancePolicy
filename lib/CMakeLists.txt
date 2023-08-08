add_subdirectory(workstealing)
add_subdirectory(skeletons)
add_subdirectory(util)

add_hpx_library(YewPar
  SOURCES
  YewPar.hpp
  YewPar.cpp
  workstealing/Scheduler.hpp
  workstealing/Scheduler.cpp
  workstealing/policies/Workpool.hpp
  workstealing/policies/Workpool.cpp
  workstealing/policies/PriorityOrdered.hpp
  workstealing/policies/PriorityOrdered.cpp
#  workstealing/policies/DepthPoolPolicy.hpp
#  workstealing/policies/DepthPoolPolicy.cpp
  workstealing/policies/PerformancePolicy.hpp
  workstealing/policies/PerformancePolicy.cpp
  workstealing/policies/PerformanceMonitor.hpp
  workstealing/policies/PerformanceMonitor.cpp
  workstealing/channels/SchedulerChannels.hpp
  workstealing/channels/SchedulerChannels.cpp
  util/util.hpp
  util/util.cpp

  COMPONENT_DEPENDENCIES
  Workqueue
  PriorityWorkqueue
  DepthPool
  Incumbent
  SearchManager
  DistSetOnceFlag
  iostreams
  SchedulerChannels
  #add unordered is important for using unordered_map's .register_as, i dont know why they need to add these but tell us nothing in hpx doc
  unordered
)
