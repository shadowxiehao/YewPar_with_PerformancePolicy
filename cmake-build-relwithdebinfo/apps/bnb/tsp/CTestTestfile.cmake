# CMake generated Testfile for 
# Source directory: /cluster/gpg/hao/YewPar/apps/bnb/tsp
# Build directory: /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/tsp
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TSP_SEQ_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/tsp/tsp" "--skeleton" "seq" "--input-file" "/cluster/gpg/hao/YewPar/test/burma14.tsp" "--hpx:threads" "1")
set_tests_properties(TSP_SEQ_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Optimal tour length: 3323" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;10;add_test;/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;0;")
add_test(TSP_DEPTHBOUNDED_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/tsp/tsp" "-d" "1" "--skeleton" "depthbounded" "--input-file" "/cluster/gpg/hao/YewPar/test/burma14.tsp" "--hpx:threads" "1")
set_tests_properties(TSP_DEPTHBOUNDED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Optimal tour length: 3323" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;15;add_test;/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;0;")
add_test(TSP_DEPTHBOUNDED_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/tsp/tsp" "-d" "1" "--skeleton" "depthbounded" "--input-file" "/cluster/gpg/hao/YewPar/test/burma14.tsp" "--hpx:threads" "4")
set_tests_properties(TSP_DEPTHBOUNDED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "Optimal tour length: 3323" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;20;add_test;/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;0;")
add_test(TSP_ORDERED_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/tsp/tsp" "-d" "1" "--skeleton" "ordered" "--input-file" "/cluster/gpg/hao/YewPar/test/burma14.tsp" "--hpx:threads" "1")
set_tests_properties(TSP_ORDERED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Optimal tour length: 3323" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;25;add_test;/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;0;")
add_test(TSP_ORDERED_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/tsp/tsp" "-d" "1" "--skeleton" "ordered" "--input-file" "/cluster/gpg/hao/YewPar/test/burma14.tsp" "--hpx:threads" "4")
set_tests_properties(TSP_ORDERED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "Optimal tour length: 3323" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;30;add_test;/cluster/gpg/hao/YewPar/apps/bnb/tsp/CMakeLists.txt;0;")
