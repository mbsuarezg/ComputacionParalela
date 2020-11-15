#!/bin/bash
clear
CWD=$(pwd)
cd ..
IWD=$(pwd)
cd $CWD
g++ reductionMP.cpp `pkg-config --cflags --libs opencv` -fopenmp -o x
for file in $IWD/images/*
do
    for ((i = 1; i <= 16; i = i * 2))
    do  
        ./x $(basename $file) 480p_version_of_$(basename $file) ${i}
    done
done