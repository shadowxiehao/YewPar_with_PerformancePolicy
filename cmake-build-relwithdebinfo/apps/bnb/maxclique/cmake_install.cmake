# Install script for directory: /cluster/gpg/hao/YewPar/apps/bnb/maxclique

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/cluster/gpg/hao/YewPar/build/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16"
         RPATH "/cluster/gpg/hao/YewPar/build/install/lib:/cluster/gpg/YewParDeps/hpx1.8/lib:/cluster/gpg/YewParDeps/gperftools-2.7/lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16"
         OLD_RPATH "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib:/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing:/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/util:/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing/policies:/cluster/gpg/YewParDeps/hpx1.8/lib:/cluster/gpg/YewParDeps/gperftools-2.7/lib:"
         NEW_RPATH "/cluster/gpg/hao/YewPar/build/install/lib:/cluster/gpg/YewParDeps/hpx1.8/lib:/cluster/gpg/YewParDeps/gperftools-2.7/lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/maxclique-16")
    endif()
  endif()
endif()

