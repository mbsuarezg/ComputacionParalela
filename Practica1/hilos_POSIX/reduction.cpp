#include <bits/stdc++.h>
#include <pthread.h>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace chrono;
using namespace cv;

int total_threads;

Mat OriginalIimage;
Mat ResizedImage;

const int output_height = 480;
const int output_width = 720;

unsigned char* og_image;
unsigned char* dest_image;

int* re;
pthread_t* my_threads;

void* downSizeImage(void* id){

    int thread_id = *(int*) id;
    int start = thread_id * (output_height / total_threads);
    int end = (thread_id + 1) * (output_height / total_threads);

    for(int i = start; i < end; ++i){
        for(int j = 0; j < output_width; ++j){

            //representative corner
            int xoffset = (OriginalIimage.rows * i) / output_height;
            int yoffset = (OriginalIimage.cols * j) / output_width;

            for(int k = 0; k < 3; ++k){
                *(dest_image + (i*3*output_width + j*3 + k)) = OriginalIimage.at<Vec3b>(xoffset, yoffset)[k];
            }
        }
    }
    return 0;
}

int main(int argc, char** argv){

    char* input_name = argv[1];
    char* output_name = argv[2];
    char* num_threads = argv[3];
    total_threads = atoi(num_threads);
    string nombre_entrada(input_name);
    nombre_entrada = "../images/" + nombre_entrada;

    ofstream fout;
    fout.open("informe.txt", ios_base::app);
    
    OriginalIimage = imread(nombre_entrada);
    ResizedImage = Mat::zeros(output_height, output_width, CV_8UC3);
    if(!OriginalIimage.data){
        return cout << "Couldn't open or find the image\n", -1;
    }
    
    og_image = (unsigned char*) malloc(OriginalIimage.rows * OriginalIimage.cols * 3 * sizeof(unsigned char));
    dest_image = (unsigned char*) malloc(output_height * output_width * 3 * sizeof(unsigned char));
    if(!og_image or !dest_image){
        perror("\nError en el malloc de las imagenes");
        exit(-1);
    }

    my_threads = (pthread_t*) malloc(total_threads * sizeof(pthread_t));
    re = (int*) malloc(total_threads * sizeof(int));
    iota(re, re + total_threads, 0);
    if(!my_threads or !re){
        perror("\nError en el malloc de los hilos");
        exit(-1);
    }

    for(int i = 0; i < OriginalIimage.rows; ++i){
        for(int j = 0; j < OriginalIimage.cols; ++j){
            for(int k = 0; k < 3; ++k){
                *(og_image + (i*OriginalIimage.cols*3 + j*3 + k)) = OriginalIimage.at<Vec3b>(i, j)[k];
            }
        }
    }

    auto start = high_resolution_clock::now();
    for(int i = 0; i < total_threads; ++i){
        int current_thread = pthread_create(&my_threads[i], NULL, downSizeImage, &re[i]);
        if(current_thread != 0){
            perror("\nError al crear el hilo");
            exit(-1);
        }
    }

    for(int i = 0; i < total_threads; ++i){
        pthread_join(my_threads[i], NULL);
    }

    for(int i = 0; i < output_height; ++i){
        for(int j = 0; j < output_width; ++j){
            for(int k = 0; k < 3; ++k){
                ResizedImage.at<Vec3b>(i, j)[k] = *(dest_image + (i*output_width*3 + j*3 + k));
            }
        }
    }

    imwrite(output_name, ResizedImage);
    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);

    fout << fixed << setprecision(9);
    fout << "----------------------------------------------------------------------------\n";
    fout << "Número de hilos: " << total_threads << '\n';
    fout << "Tiempo de respuesta: " << total_time.count() / 1000 << '\n';
    fout << "Dimensiones de la imagen de entrada: " << OriginalIimage.cols << "," << OriginalIimage.rows << "\n";
    fout << "----------------------------------------------------------------------------\n\n";

    free(re);
    free(my_threads);
    free(og_image);
    free(dest_image);
    return 0;
}