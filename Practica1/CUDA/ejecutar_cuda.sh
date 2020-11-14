#!/bin/bash
clear
cd ..
CWD=$(pwd)
echo $CWD
cd CUDA/
nvcc reduction.cu -o x -gencode arch=compute_35,code=compute_35 `pkg-config --cflags --libs opencv`
for file in $CWD/images/*
do
    for ((i = 1; i <= 16; i = i * 2))
    do  
    	for((j = 1; j <= 128; j = j * 2))
    	do
    		./x $(basename $file) 480p_version_of_$(basename $file) ${i} ${j}
    	done
    done
done
