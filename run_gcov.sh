#!/bin/bash

for filename in `find . -not -path ."/lib/*" -not -path "./CMakeFiles/*" -type f -name \*.cpp`
do
    echo $filename
    gcov -n -o . $filename.cpp > /dev/null
done
