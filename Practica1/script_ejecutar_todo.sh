#!/bin/bash
clear
<<<<<<< HEAD
CWD=$(pwd)
echo $CWD
for file in $CWD/images/*
do
    for ((i = 1; i <= 16; i = i * 2))
    do  
        g++ reduction.cpp -o x `pkg-config --cflags --libs opencv` -lpthread && ./x $(basename $file) 480p_version_of_$(basename $file) ${i}
    done
done
cat informe.txt
=======
for i in {0..4} ; do g++ reduction.cpp -o x `pkg-config --cflags --libs opencv` -lpthread && ./x waterfalls_4k.jpg waterfalls_480p.jpg ${i}; done;
cat informe.txt
>>>>>>> 24c18ee7408086320a85c9a141bea5b57b0d43ef
