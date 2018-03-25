#!/bin/bash

for filename in `find . -name "*.cpp" ! -name "*CMake*"`
do
    echo $filename
    gcov -n -o . $filename.cpp > /dev/null
done
