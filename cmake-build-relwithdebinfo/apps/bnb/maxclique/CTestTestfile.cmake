# CMake generated Testfile for 
# Source directory: /cluster/gpg/hao/YewPar/apps/bnb/maxclique
# Build directory: /cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MAXCLIQUE_SEQ_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "seq" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "1")
set_tests_properties(MAXCLIQUE_SEQ_1T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;11;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_ORDERED_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "ordered" "-d" "1" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "1")
set_tests_properties(MAXCLIQUE_ORDERED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;16;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_ORDERED_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "ordered" "-d" "1" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "4")
set_tests_properties(MAXCLIQUE_ORDERED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;21;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_DEPTHBOUNDED_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "-d" "1" "--skeleton" "depthbounded" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "1")
set_tests_properties(MAXCLIQUE_DEPTHBOUNDED_1T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;26;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_DEPTHBOUNDED_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "-d" "1" "--skeleton" "depthbounded" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "4")
set_tests_properties(MAXCLIQUE_DEPTHBOUNDED_4T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;31;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_DEPTHBOUNDED_DECISION_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "-d" "1" "--skeleton" "depthbounded" "--decisionBound" "21" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "1")
set_tests_properties(MAXCLIQUE_DEPTHBOUNDED_DECISION_1T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;36;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_DEPTHBOUNDED_DECISION_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "-d" "1" "--skeleton" "depthbounded" "--decisionBound" "21" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "4")
set_tests_properties(MAXCLIQUE_DEPTHBOUNDED_DECISION_4T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;41;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_STACKSTEALS_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "stacksteal" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "1")
set_tests_properties(MAXCLIQUE_STACKSTEALS_1T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;46;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_STACKSTEALS_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "stacksteal" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "4")
set_tests_properties(MAXCLIQUE_STACKSTEALS_4T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;51;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_BUDGET_1T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "budget" "-b" "1000000" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "1")
set_tests_properties(MAXCLIQUE_BUDGET_1T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;56;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")
add_test(MAXCLIQUE_BUDGET_4T "/cluster/gpg/hao/YewPar/cmake-build-relwithdebinfo/apps/bnb/maxclique/maxclique-16" "--skeleton" "budget" "-b" "1000000" "--input-file" "/cluster/gpg/hao/YewPar/test/brock200_1.clq" "--hpx:threads" "4")
set_tests_properties(MAXCLIQUE_BUDGET_4T PROPERTIES  PASS_REGULAR_EXPRESSION "MaxClique Size = 21" _BACKTRACE_TRIPLES "/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;61;add_test;/cluster/gpg/hao/YewPar/apps/bnb/maxclique/CMakeLists.txt;0;")