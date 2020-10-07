#!/bin/bash

clear
for i in {0..4} ; do g++ reduccionImagen.cpp -o x `pkg-config --cflags --libs opencv` -lpthread && ./x clouds.jpg clouds_480p.jpg ${i}; done;
cat informe.txt
