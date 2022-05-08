#!/bin/bash
gcc -Wall -O3 -lm main.cpp

if [ $? -eq 0 ]; then
    ./a.out 1> test.ppm
    gpicview test.ppm     
else
    echo "Build Failed"
fi
