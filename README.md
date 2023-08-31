# YewPar_with_PerformancePolicy

This project is based on the YewPar(https://github.com/BlairArchibald/YewPar) which is a Collection of High Performance Parallel Skeletons for Tree Search Problems.

The aim of this project is to add a workstealing policy based on the performance of every node.

The results shows that by using this new policy, the application based on the YewPar sometimes can have more than 13% speedup.

Then code mainly modify the codes in "YewPar\lib\workstealing" dir, affecting the skeletons using the DepthPool.

## Installation guide

There are a number of ways to install this, 
one way is to install it manually, 
which requires you to install cmake,boost_1_79_0,gperftools-2.7,libopenmpi-dev,libasio-dev in a linux environment.

Then download hpx-1.8 or hpx-1.8.1 source code and compile and install hpx through cmake, 
command is as follows (The contents of the "\{\}" need to be replaced,these are guidelines only and you can change them as needed):

```bash
cd {path to hpx source code dir} ;
cd build ;
{path to cmake dir}/cmake -DCMAKE_BUILD_TYPE={Release or RelWithDebInfo} \
       -DCMAKE_INSTALL_PREFIX={path to hpx source code dir}/build/install \
       -DHPX_WITH_PARCELPORT_MPI=ON \
       -DHPX_WITH_EXAMPLES=OFF \
       -DHPX_WITH_TESTS=OFF  \
       -DHPX_WITH_DISABLED_SIGNAL_EXCEPTION_HANDLERS=TRUE \
       -DBOOST_ROOT={path to boost}/boost_1_79_0/ \
       -DBOOST_LIBRARYDIR={path to boost}/boost_1_79_0/lib \
       -DTCMALLOC_LIBRARY{path to gperftools}/gperftools-2.7/lib/ \
       -DTCMALLOC_INCLUDE_DIR={path to gperftools}/gperftools-2.7/include \
        ../
	  make -j{cpu cores you have} ; \
	  make install
```

then install YewPar's apps:

```bash
cd {path to YewPar dir} ;
mkdir -p build ;
cd build ;

{path to cmake dir}/cmake -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX={path to YewPar dir}/build/install \
    -DHPX_DIR={path to hpx source code dir}/build/install/lib/cmake/HPX \
    -DBoost_INCLUDE_DIR={path to hpx installed dir}/include \
    -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=On \
    -DYEWPAR_BUILD_BNB_APPS_MAXCLIQUE_NWORDS=16 \
    -DYEWPAR_BUILD_BNB_APPS_KNAPSACK_NITEMS=220 \
    -DYEWPAR_BUILD_APPS_SIP_NWORDS=128 \

make -j{cpu cores you have} ;
make install ;
```
Then you should be able to see apps of YewPar in the "{path to YewPar dir}/build/install/bin"

## example commands of apps

example commands of NS-hivert app:

run in one server node:

```bash
{path to the app}/NS-hivert --skeleton budget -b 1000000 -g 39 --hpx:threads {cores you have}
```

run in cluster:

```bash
mpiexec.openmpi -n {number of nodes you have} --host {nodes' names, use comma apart} {path to the app}/NS-hivert --skeleton budget -b 1000000 -g 39 --hpx:threads {cores you have per node}
```
