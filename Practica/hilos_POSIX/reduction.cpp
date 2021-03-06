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

Mat OriginalImage;
Mat ResizedImage;

const int output_height = 480;
const int output_width = 720;

unsigned char* og_image;
unsigned char* dest_image;

int* re;
pthread_t* my_threads;

void* downSizeImage(void* id){

    int H = OriginalImage.rows, W = OriginalImage.cols;
    int h = output_height, w = output_width;
    
    int thread_id = *(int*) id;
    int start = thread_id * ((h * w + total_threads - 1) / total_threads);
    int end = min(h * w, (thread_id + 1) * ((h * w + total_threads - 1) / total_threads));

    for(int i = start; i < end; ++i){
        for(int k = 0; k < 3; ++k){
            *(dest_image + i*3 + k) = *(og_image + (((H * (i / w)) / h)*W + ((W * (i % w)) / w))*3 + k);
        }
    }
    return 0;
}

int main(int argc, char** argv){

    string input_name = "../images/" + string(argv[1]);
    total_threads = atoi(argv[3]);

    ofstream fout;
    fout.open("informe_posix.txt", ios_base::app);
    
    OriginalImage = imread(input_name);
    ResizedImage = Mat(output_height, output_width, CV_8UC3);
    if(!OriginalImage.data){
        return cout << "Couldn't open or find the image: " << input_name << '\n', -1;
    }
    
    size_t og_size = OriginalImage.rows * OriginalImage.cols * 3 * sizeof(unsigned char);
    size_t re_size = output_height * output_width * 3 * sizeof(unsigned char);

    og_image = (unsigned char*) malloc(og_size);
    dest_image = (unsigned char*) malloc(re_size);
    if(!og_image or !dest_image){
        perror("\nError en el malloc de las imagenes");
        exit(-1);
    }
    memcpy(og_image, OriginalImage.ptr(), og_size);

    my_threads = (pthread_t*) malloc(total_threads * sizeof(pthread_t));
    re = (int*) malloc(total_threads * sizeof(int));
    iota(re, re + total_threads, 0);
    if(!my_threads or !re){
        perror("\nError en el malloc de los hilos");
        exit(-1);
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
    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);

    memcpy(ResizedImage.ptr(), dest_image, re_size);    
    imwrite(string(argv[2]), ResizedImage);
    
    fout << fixed << setprecision(9);
    fout << "----------------------------------------------------------------------------\n";
    fout << "Número de hilos: " << total_threads << '\n';
    fout << "Tiempo de respuesta: " << total_time.count() / 1000 << '\n';
    fout << "Dimensiones de la imagen de entrada: " << OriginalImage.cols << "," << OriginalImage.rows << "\n";
    fout << "----------------------------------------------------------------------------\n\n";

    free(re);
    free(my_threads);
    free(og_image);
    free(dest_image);
    return 0;
}