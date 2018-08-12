cmake_minimum_required(VERSION 3.0.2)
project(church)

INCLUDE(ExternalProject)

option(BUILD_DOC "Build documentation" OFF)

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/benchmarks)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++17")

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

SET(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/lib)

if(CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]") #Note: do not compile with optimizations
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g --coverage")
elseif(CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror -Wno-ignored-qualifiers")
else()
  MESSAGE(WARNING "No build type specified; defaulting to debug.")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g --coverage")
endif()
SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

LINK_DIRECTORIES(
  ${CMAKE_BINARY_DIR}/lib
  ${CMAKE_BINARY_DIR}/lib/lib
  ${CMAKE_BINARY_DIR}/lib/lib64)
ADD_LIBRARY(church SHARED
  main.cpp)
TARGET_INCLUDE_DIRECTORIES(church SYSTEM PRIVATE
     "${CMAKE_BINARY_DIR}/include"
     "${CMAKE_BINARY_DIR}/parser/include"
     "${CMAKE_BINARY_DIR}/lib/include")

if((TESTING MATCHES "^[Tt][Rr][Uu][Ee]" OR TESTING MATCHES "^[Oo][Nn]") AND CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
 EXTERNALPROJECT_ADD(google-test
   PREFIX lib/google-test
   GIT_REPOSITORY https://github.com/google/googletest
   CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION})
 MESSAGE("Disovering test files:")

 FILE(GLOB test_list tests/*.cpp)
 FOREACH(file ${test_list})
   STRING(REPLACE ".cpp" "" test_name_unclean ${file})
   GET_FILENAME_COMPONENT(test_name ${test_name_unclean} NAME)

   MESSAGE("Test file discovered: ${test_name}. Generating build target ${test_name}")

   ADD_EXECUTABLE(${test_name} ${file} tests/main_stub.cpp)

   ADD_DEPENDENCIES(${test_name} church google-test)

   TARGET_LINK_LIBRARIES(${test_name} libgtest.a libgtest_main.a pthread church)
   TARGET_COMPILE_OPTIONS(${test_name} PUBLIC -pthread)

   TARGET_INCLUDE_DIRECTORIES(${test_name} SYSTEM PRIVATE
     "${CMAKE_BINARY_DIR}/include"
     "${CMAKE_BINARY_DIR}/parser/include"
     "${CMAKE_BINARY_DIR}/lib/include")

   SET_TARGET_PROPERTIES(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")
 ENDFOREACH(file test_list)
endif()

if((BENCHMARK MATCHES "^[Tt][Rr][Uu][Ee]" OR BENCHMARK MATCHES "^[Oo][Nn]") AND CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
  EXTERNALPROJECT_ADD(google-bench
    PREFIX lib/google-bench
    GIT_REPOSITORY https://github.com/google/benchmark
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION})
  MESSAGE("Discovering benchmark files")

  FILE(GLOB benchmark_list benchmarks/*.cpp)
  FOREACH(file ${benchmark_list})
    STRING(REPLACE ".cpp" "" benchmark_name_unclean ${file})
    GET_FILENAME_COMPONENT(benchmark_name ${benchmark_name_unclean} NAME)

    MESSAGE("Benchmark file discovered: ${benchmark_name}. Generating build target ${benchmark_name}_bench")

    ADD_EXECUTABLE(${benchmark_name} ${file})

    ADD_DEPENDENCIES(${benchmark_name} church google-test google-bench)

    TARGET_LINK_LIBRARIES(${benchmark_name} gtest gtest_main pthread church benchmark benchmark_main)
    TARGET_COMPILE_OPTIONS(${benchmark_name} PUBLIC -pthread)

    TARGET_INCLUDE_DIRECTORIES(${benchmark_name} SYSTEM PRIVATE
      "${CMAKE_BINARY_DIR}/lib/include")

    SET_TARGET_PROPERTIES(${benchmark_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks")
  ENDFOREACH(file in test_list)
endif()

if(DOXYGEN_FOUND AND CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" OR BUILD_DOC MATCHES "^[Oo][Nn]" OR BUILD_DOC MATCHES "^[Tt][Rr][Uu][Ee]")
  FIND_PACKAGE(Doxygen)

  SET(DOXYFILE ${CMAKE_BINARY_DIR}/docs/Doxyfile)

  ADD_CUSTOM_TARGET(doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation"
    VERBATIM)
else()
  MESSAGE("Doxygen not found; skipping")
endif()
