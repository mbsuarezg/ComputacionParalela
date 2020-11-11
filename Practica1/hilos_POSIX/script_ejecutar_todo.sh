#!/bin/bash
clear
cd ..
CWD=$(pwd)
echo $CWD
cd hilos_POSIX/
for file in $CWD/images/*
do
    for ((i = 1; i <= 16; i = i * 2))
    do  
        g++ reduction.cpp -o x `pkg-config --cflags --libs opencv` -lpthread && ./x $(basename $file) 480p_version_of_$(basename $file) ${i}
    done
done
