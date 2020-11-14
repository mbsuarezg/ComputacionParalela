#!/bin/bash
clear
CWD=$(pwd)
cd ..
IWD=$(pwd)
cd $CWD
nvcc reduction.cu -o x -gencode arch=compute_35,code=compute_35 `pkg-config --cflags --libs opencv`
for file in $IWD/images/*
do
    for ((i = 1; i <= 2; i = i * 2))
    do  
    	for((j = 1; j <= 2; j = j * 2))
    	do
    		./x $(basename $file) 480p_version_of_$(basename $file) ${i} ${j}
    	done
    done
done
