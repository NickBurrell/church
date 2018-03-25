cmake_minimum_required (VERSION 2.6)
project (Church)

include(ExternalProject)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_OUTPUT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror -Wextra -std=c++17")

if(CMAKE_BUILD_TYPE EQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
elseif(CMAKE_BUILD_TYPE EQUAL "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

if(TEST)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -lgcov ")
endif()

if(TEST)
  ExternalProject_Add(GoogleTest
    DOWNLOAD_DIR ${EXTERNAL_INSTALL_LOCATION}
    GIT_REPOSITORY https://www.github.com/google/googletest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    )
endif(TEST)

if(BENCHMARK)
  ExternalProject_Add(GoogleBench
    GIT_REPOSITORY https://github.com/google/benchmark
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    )
endif(BENCHMARK)

include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib64)

add_executable(Test main.cpp)

if(TEST)
  file(GLOB test_list tests/*.cpp)
  FOREACH(file ${test_list})
    string(REPLACE ".cpp" "" test_name_unclean ${file})
    get_filename_component( test_name ${test_name_unclean} NAME)
    add_executable( ${test_name} ${file})

    add_dependencies(${test_name} GoogleTest)
    target_link_libraries(${test_name} gtest gtest_main pthread)

  ENDFOREACH( file test_list)

endif()

if(BENCHMARK)

  file(GLOB test_list bench/*.cpp)
  FOREACH(file ${test_list})
    string(REPLACE ".cpp" "" test_name_unclean ${file})
    get_filename_component( test_name ${test_name_unclean} NAME)
    add_executable( ${test_name} ${file})

    add_dependencies(${test_name} GoogleTest)
    add_dependencies(${test_name} GoogleBench)
    target_link_libraries(${test_name} gtest gtest_main benchmark pthread)

  ENDFOREACH(file in test_list)

endif()
