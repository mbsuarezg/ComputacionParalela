#!/bin/bash

clear
for i in {0..4} ; do g++ reduction.cpp -o x `pkg-config --cflags --libs opencv` -lpthread && ./x waterfalls_4k.jpg clouds_480p.jpg ${i}; done;
cat informe.txt
