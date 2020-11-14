#!/bin/bash
clear
cd ..
CWD=$(pwd)
echo $CWD
cd openMP/
g++ reductionMP.cpp `pkg-config --cflags --libs opencv` -fopenmp -o x
for file in $CWD/images/*
do
    for ((i = 1; i <= 16; i = i * 2))
    do  
        ./x $(basename $file) 480p_version_of_$(basename $file) ${i}
    done
done