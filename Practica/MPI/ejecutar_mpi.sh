#!/bin/bash
clear
CWD=$(pwd)
cd ..
IWD=$(pwd)
cd $CWD
mpic++ reduction.cpp `pkg-config --cflags --libs opencv` -fopenmp -o x
clear
for file in $IWD/images/*
do
    for ((i = 1; i <= 8; i = i * 2))
    do  
    	mpirun -np 4 ./x $(basename $file) 480p_version_of_$(basename $file) ${i}
    done
done