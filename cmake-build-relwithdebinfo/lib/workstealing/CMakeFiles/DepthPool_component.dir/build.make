# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /cluster/gpg/YewParDeps/cmake-3.25.3-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /cluster/gpg/YewParDeps/cmake-3.25.3-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /cluster/gpg/hao/YewPar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo

# Include any dependencies generated for this target.
include lib/workstealing/CMakeFiles/DepthPool_component.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lib/workstealing/CMakeFiles/DepthPool_component.dir/compiler_depend.make

# Include the progress variables for this target.
include lib/workstealing/CMakeFiles/DepthPool_component.dir/progress.make

# Include the compile flags for this target's objects.
include lib/workstealing/CMakeFiles/DepthPool_component.dir/flags.make

lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o: lib/workstealing/CMakeFiles/DepthPool_component.dir/flags.make
lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o: /cluster/gpg/hao/YewPar/lib/workstealing/DepthPool.cpp
lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o: lib/workstealing/CMakeFiles/DepthPool_component.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o"
	cd /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o -MF CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o.d -o CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o -c /cluster/gpg/hao/YewPar/lib/workstealing/DepthPool.cpp

lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/DepthPool_component.dir/DepthPool.cpp.i"
	cd /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /cluster/gpg/hao/YewPar/lib/workstealing/DepthPool.cpp > CMakeFiles/DepthPool_component.dir/DepthPool.cpp.i

lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/DepthPool_component.dir/DepthPool.cpp.s"
	cd /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /cluster/gpg/hao/YewPar/lib/workstealing/DepthPool.cpp -o CMakeFiles/DepthPool_component.dir/DepthPool.cpp.s

# Object files for target DepthPool_component
DepthPool_component_OBJECTS = \
"CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o"

# External object files for target DepthPool_component
DepthPool_component_EXTERNAL_OBJECTS =

lib/workstealing/libhpx_DepthPool.so: lib/workstealing/CMakeFiles/DepthPool_component.dir/DepthPool.cpp.o
lib/workstealing/libhpx_DepthPool.so: lib/workstealing/CMakeFiles/DepthPool_component.dir/build.make
lib/workstealing/libhpx_DepthPool.so: /cluster/gpg/YewParDeps/hpx1.8/lib/libhpx.so.1.8.1
lib/workstealing/libhpx_DepthPool.so: /cluster/gpg/YewParDeps/hpx1.8/lib/libhpx_core.so
lib/workstealing/libhpx_DepthPool.so: /cluster/gpg/YewParDeps/gperftools-2.7/lib/libtcmalloc_minimal.so
lib/workstealing/libhpx_DepthPool.so: /usr/lib/x86_64-linux-gnu/libhwloc.so
lib/workstealing/libhpx_DepthPool.so: lib/workstealing/CMakeFiles/DepthPool_component.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libhpx_DepthPool.so"
	cd /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/DepthPool_component.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/workstealing/CMakeFiles/DepthPool_component.dir/build: lib/workstealing/libhpx_DepthPool.so
.PHONY : lib/workstealing/CMakeFiles/DepthPool_component.dir/build

lib/workstealing/CMakeFiles/DepthPool_component.dir/clean:
	cd /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing && $(CMAKE_COMMAND) -P CMakeFiles/DepthPool_component.dir/cmake_clean.cmake
.PHONY : lib/workstealing/CMakeFiles/DepthPool_component.dir/clean

lib/workstealing/CMakeFiles/DepthPool_component.dir/depend:
	cd /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /cluster/gpg/hao/YewPar /cluster/gpg/hao/YewPar/lib/workstealing /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/lib/workstealing/CMakeFiles/DepthPool_component.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/workstealing/CMakeFiles/DepthPool_component.dir/depend

