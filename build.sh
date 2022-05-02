#!/bin/bash
gcc -Wall -lm -O3 -Ofast main.cpp

if [ $? -eq 0 ]; then
    ./a.out 1> test.ppm
    gpicview test.ppm     
else
    echo "Build Failed"
fi
