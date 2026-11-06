# CMake generated Testfile for 
# Source directory: /home/apal/Рабочий стол/Learning_cpp
# Build directory: /home/apal/Рабочий стол/Learning_cpp/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/home/apal/Рабочий стол/Learning_cpp/build/chat_tests[1]_include.cmake")
add_test([=[compile_fail_test]=] "/usr/bin/cmake" "--build" "/home/apal/Рабочий стол/Learning_cpp/build" "--target" "chat_compile_fail_test")
set_tests_properties([=[compile_fail_test]=] PROPERTIES  WILL_FAIL "TRUE" _BACKTRACE_TRIPLES "/home/apal/Рабочий стол/Learning_cpp/CMakeLists.txt;71;add_test;/home/apal/Рабочий стол/Learning_cpp/CMakeLists.txt;0;")
