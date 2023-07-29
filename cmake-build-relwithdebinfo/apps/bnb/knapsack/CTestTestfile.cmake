# CMake generated Testfile for 
# Source directory: /cluster/gpg/hao/YewPar/apps/bnb/knapsack
# Build directory: /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/knapsack
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(KNAPSACK_SEQ_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/knapsack/knapsack" "--skeleton" "seq" "--input-file" "/cluster/gpg/hao/YewPar/test/knapsackTest1.kp" "--hpx:threads" "1")
set_tests_properties(KNAPSACK_SEQ_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Final Profit: 6925" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;11;add_test;/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;0;")
add_test(KNAPSACK_DEPTHBOUNDED_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/knapsack/knapsack" "-d" "1" "--skeleton" "depthbounded" "--input-file" "/cluster/gpg/hao/YewPar/test/knapsackTest1.kp" "--hpx:threads" "1")
set_tests_properties(KNAPSACK_DEPTHBOUNDED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Final Profit: 6925" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;16;add_test;/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;0;")
add_test(KNAPSACK_DEPTHBOUNDED_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/knapsack/knapsack" "-d" "1" "--skeleton" "depthbounded" "--input-file" "/cluster/gpg/hao/YewPar/test/knapsackTest1.kp" "--hpx:threads" "4")
set_tests_properties(KNAPSACK_DEPTHBOUNDED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "Final Profit: 6925" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;21;add_test;/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;0;")
add_test(KNAPSACK_ORDERED_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/knapsack/knapsack" "-d" "1" "--skeleton" "ordered" "--input-file" "/cluster/gpg/hao/YewPar/test/knapsackTest1.kp" "--hpx:threads" "1")
set_tests_properties(KNAPSACK_ORDERED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Final Profit: 6925" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;26;add_test;/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;0;")
add_test(KNAPSACK_ORDERED_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/knapsack/knapsack" "-d" "1" "--skeleton" "ordered" "--input-file" "/cluster/gpg/hao/YewPar/test/knapsackTest1.kp" "--hpx:threads" "4")
set_tests_properties(KNAPSACK_ORDERED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "Final Profit: 6925" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;31;add_test;/cluster/gpg/hao/YewPar/apps/bnb/knapsack/CMakeLists.txt;0;")
