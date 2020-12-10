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
	for((i = 1; i <= 4; i = i * 2))
	do
		for ((j = 1; j <= 4; j = j * 2))
	    do  
	    	mpirun -np ${i} --hostfile mpi_hosts ./x $(basename $file) 480p_version_of_$(basename $file) ${j}
	    done
	done
done
