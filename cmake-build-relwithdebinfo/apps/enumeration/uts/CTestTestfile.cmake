# CMake generated Testfile for 
# Source directory: /cluster/gpg/hao/YewPar/apps/enumeration/uts
# Build directory: /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/enumeration/uts
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(UTS_SEQ_1T "uts" "--skeleton" "seq" "--uts-t" "geometric" "--uts-a" "2" "--uts-d" "10" "--uts-b" "4" "--uts-r" "19" "--hpx:threads" "1")
set_tests_properties(UTS_SEQ_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Total Nodes: 4130071" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;13;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;0;")
add_test(UTS_DEPTHBOUNDED_1T "uts" "-s" "3" "--skeleton" "depthbounded" "--uts-t" "geometric" "--uts-a" "2" "--uts-d" "10" "--uts-b" "4" "--uts-r" "19" "--hpx:threads" "1")
set_tests_properties(UTS_DEPTHBOUNDED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Total Nodes: 4130071" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;16;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;0;")
add_test(UTS_DEPTHBOUNDED_4T "uts" "-s" "3" "--skeleton" "depthbounded" "--uts-t" "geometric" "--uts-a" "2" "--uts-d" "10" "--uts-b" "4" "--uts-r" "19" "--hpx:threads" "4")
set_tests_properties(UTS_DEPTHBOUNDED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "Total Nodes: 4130071" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;19;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;0;")
add_test(UTS_STACKSTEAL_1T "uts" "--skeleton" "stacksteal" "--uts-t" "geometric" "--uts-a" "2" "--uts-d" "10" "--uts-b" "4" "--uts-r" "19" "--hpx:threads" "1")
set_tests_properties(UTS_STACKSTEAL_1T PROPERTIES  PASS_REGULAR_EXPRESSION "Total Nodes: 4130071" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;22;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;0;")
add_test(UTS_STACKSTEAL_4T "uts" "--skeleton" "stacksteal" "--uts-t" "geometric" "--uts-a" "2" "--uts-d" "10" "--uts-b" "4" "--uts-r" "19" "--hpx:threads" "4")
set_tests_properties(UTS_STACKSTEAL_4T PROPERTIES  PASS_REGULAR_EXPRESSION "Total Nodes: 4130071" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;25;add_test;/cluster/gpg/hao/YewPar/apps/enumeration/uts/CMakeLists.txt;0;")
