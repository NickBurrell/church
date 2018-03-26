cmake_minimum_required (VERSION 2.6)
project (Church)

include(ExternalProject)

option(BUILD_DOC "Build documentation" OFF)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++17")

if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
elseif(CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Werror")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

if(TEST MATCHES "^[Tt][Rr][Uu][Ee]" OR TEST MATCHES "^[Oo][Nn]")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -lgcov ")
endif()

if(TEST MATCHES "^[Tt][Rr][Uu][Ee]" OR TEST MATCHES "^[Oo][Nn]")
  ExternalProject_Add(GoogleTest
    DOWNLOAD_DIR ${EXTERNAL_INSTALL_LOCATION}
    GIT_REPOSITORY https://www.github.com/google/googletest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    )
endif(TEST)

if(BENCHMARK MATCHES "^[Tt][Rr][Uu][Ee]" OR BENCHMARK MATCHES "^[Oo][Nn]")
  ExternalProject_Add(GoogleBench
    GIT_REPOSITORY https://github.com/google/benchmark
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    )
endif(BENCHMARK)

find_package(Doxygen)
if(DOXYGEN_FOUND AND CMAKE_BUILD_TYPE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]")

  #set(DOXYGEN_IN ${CMAKE_BINARY_DIR}/docs/Doxyfile.in)
  #set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/docs/Doxyfile)

  #configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})

  set(DOXYFILE ${CMAKE_BINARY_DIR}/docs/Doxyfile)

  add_custom_target( doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation"
    VERBATIM )

else()
  MESSAGE("Doxygen not found; skipping")
endif()

include_directories(${EXTERNAL_INSTALL_LOCATION}/src/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib64)
link_directories(${CMAKE_BINARY_DIR}/build)

add_executable(church main.cpp)

add_library(church_lang SHARED main.cpp)

if(TEST MATCHES "^[Tt][Rr][Uu][Ee]" OR TEST MATCHES "^[Oo][Nn]" )

  MESSAGE("Begun building tests")

  file(GLOB test_list tests/*.cpp)
  FOREACH(file ${test_list})
    string(REPLACE ".cpp" "" test_name_unclean ${file})
    get_filename_component( test_name ${test_name_unclean} NAME)

    MESSAGE("Begun building test: ${test_name}")

    add_executable( ${test_name} ${file})

    add_dependencies(${test_name} GoogleTest)
    target_link_libraries(${test_name} gtest gtest_main pthread)

    set_target_properties( ${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests")

  ENDFOREACH( file test_list)

add_dependencies(church_lang GoogleTest)
add_dependencies(church GoogleTest)

target_link_libraries(church gtest gtest_main pthread)
target_link_libraries(church gtest gtest_main pthread)

endif()

if(BENCHMARK OR "^[Tt][Rr][Uu][Ee]" OR BENCHMARK MATCHES "^[Oo][Nn]")

  MESSAGE("Begun building benchmarks")

  file(GLOB benchmark_list benchmarks/*.cpp)
  FOREACH(file ${benchmark_list})
    string(REPLACE ".cpp" "" benchmark_name_unclean ${file})
    get_filename_component( test_name ${benchmark_name_unclean} NAME)

    MESSAGE("Begun building benchmark: ${benchmark_name}")

    add_executable( ${benchmark_name} ${file})

    add_dependencies(${benchmark_name} GoogleTest)
    add_dependencies(${benchmark_name} GoogleBench)

    target_link_libraries(${benchmark_name} gtest gtest_main benchmark pthread)

    set_target_properties( ${benchmark_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmarks")

  ENDFOREACH(file in test_list)


add_dependencies(church_lang GoogleTest)
add_dependencies(church GoogleTest)
add_dependencies(church_lang GoogleBench)
add_dependencies(church GoogleBench)

target_link_libraries(church benchmark gtest gtest_main pthread)
target_link_libraries(church_lang gtest benchmark gtest_main pthread)



endif()
