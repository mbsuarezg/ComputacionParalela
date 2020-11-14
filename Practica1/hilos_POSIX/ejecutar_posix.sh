#!/bin/bash
clear
CWD=$(pwd)
cd ..
IWD=$(pwd)
cd $CWD
g++ reduction.cpp -o x `pkg-config --cflags --libs opencv` -lpthread
for file in $IWD/images/*
do
    for ((i = 1; i <= 16; i = i * 2))
    do  
        ./x $(basename $file) 480p_version_of_$(basename $file) ${i}
    done
done
