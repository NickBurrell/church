cmake_minimum_required (VERSION 2.6)
project (Church)

include(ExternalProject)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})
set(CMAKE_OUTPUT_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR/bin})

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/lib)

if(TEST)
  ExternalProject_Add(GoogleTest
    DOWNLOAD_DIR ${EXTERNAL_INSTALL_LOCATION}
    GIT_REPOSITORY https://www.github.com/google/googletest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    )
endif()

if(BENCHMARK)
  ExternalProject_Add(GoogleBench
    GIT_REPOSITORY https://github.com/google/benchmark
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
    )
endif()
include_directories(${EXTERNAL_INSTALL_LOCATION}/include)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)
link_directories(${EXTERNAL_INSTALL_LOCATION}/lib64)

add_executable(Test main.cpp)
add_dependencies(Test GoogleTest)
target_link_libraries(Test gtest gtest_main pthread)
