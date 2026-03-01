# CMake generated Testfile for 
# Source directory: W:/Programmation/Epistemotron
# Build directory: W:/Programmation/Epistemotron/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(EpistemotronTests "W:/Programmation/Epistemotron/build/bin/Debug/test_simulation.exe")
  set_tests_properties(EpistemotronTests PROPERTIES  _BACKTRACE_TRIPLES "W:/Programmation/Epistemotron/CMakeLists.txt;66;add_test;W:/Programmation/Epistemotron/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(EpistemotronTests "W:/Programmation/Epistemotron/build/bin/Release/test_simulation.exe")
  set_tests_properties(EpistemotronTests PROPERTIES  _BACKTRACE_TRIPLES "W:/Programmation/Epistemotron/CMakeLists.txt;66;add_test;W:/Programmation/Epistemotron/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(EpistemotronTests "W:/Programmation/Epistemotron/build/bin/MinSizeRel/test_simulation.exe")
  set_tests_properties(EpistemotronTests PROPERTIES  _BACKTRACE_TRIPLES "W:/Programmation/Epistemotron/CMakeLists.txt;66;add_test;W:/Programmation/Epistemotron/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(EpistemotronTests "W:/Programmation/Epistemotron/build/bin/RelWithDebInfo/test_simulation.exe")
  set_tests_properties(EpistemotronTests PROPERTIES  _BACKTRACE_TRIPLES "W:/Programmation/Epistemotron/CMakeLists.txt;66;add_test;W:/Programmation/Epistemotron/CMakeLists.txt;0;")
else()
  add_test(EpistemotronTests NOT_AVAILABLE)
endif()
