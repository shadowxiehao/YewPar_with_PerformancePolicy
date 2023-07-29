# CMake generated Testfile for 
# Source directory: /cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups
# Build directory: /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/enumeration/numericalSemigroups
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(NS_HIVERT_SEQ_1T "NS-hivert" "--skeleton" "seq" "-g" "31" "--hpx:threads" "1")
set_tests_properties(NS_HIVERT_SEQ_1T PROPERTIES  PASS_REGULAR_EXPRESSION "30: 5646773" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;26;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;0;")
add_test(NS_HIVERT_DEPTHBOUNDED_1T "NS-hivert" "--skeleton" "depthbounded" "-g" "31" "-d" "10" "--hpx:threads" "1")
set_tests_properties(NS_HIVERT_DEPTHBOUNDED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "30: 5646773" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;29;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;0;")
add_test(NS_HIVERT_DEPTHBOUNDED_4T "NS-hivert" "--skeleton" "depthbounded" "-g" "31" "-d" "10" "--hpx:threads" "4")
set_tests_properties(NS_HIVERT_DEPTHBOUNDED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "30: 5646773" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;32;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;0;")
add_test(NS_HIVERT_STACKSTEALS_1T "NS-hivert" "--skeleton" "stacksteal" "-g" "31" "--hpx:threads" "1")
set_tests_properties(NS_HIVERT_STACKSTEALS_1T PROPERTIES  PASS_REGULAR_EXPRESSION "30: 5646773" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;35;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;0;")
add_test(NS_HIVERT_STACKSTEALS_4T "NS-hivert" "--skeleton" "stacksteal" "-g" "31" "--hpx:threads" "4")
set_tests_properties(NS_HIVERT_STACKSTEALS_4T PROPERTIES  PASS_REGULAR_EXPRESSION "30: 5646773" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;38;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/numericalSemigroups/CMakeLists.txt;0;")
