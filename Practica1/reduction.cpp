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

const int xgap = (OriginalIimage.rows + output_height - 1) / output_height;
const int ygap = (OriginalIimage.cols + output_width - 1) / output_width;
const int total = xgap * ygap;
const double fraq = 1.0 / double(total);

int* re;
pthread_t* my_threads;

int fpow(int a, int b){
    int res = 1;
    while(b){
        if(b & 1) res *= a;
        a *= a;
        b >>= 1;
    }
    return res;
}

void* downSizeImage(void* id){

    int thread_id = *(int*) id;
    int start = thread_id * (output_height / total_threads);
    int end = (thread_id + 1) * (output_height / total_threads);

    for(int i = start; i < end; ++i){
        for(int j = 0; j < output_width; ++j){

            int xoffset = (OriginalIimage.rows * i) / output_height;
            int yoffset = (OriginalIimage.cols * j) / output_width;

            ResizedImage.at<Vec3b>(i, j)[0] = OriginalIimage.at<Vec3b>(xoffset, yoffset)[0];
            ResizedImage.at<Vec3b>(i, j)[1] = OriginalIimage.at<Vec3b>(xoffset, yoffset)[1];
            ResizedImage.at<Vec3b>(i, j)[2] = OriginalIimage.at<Vec3b>(xoffset, yoffset)[2];
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
    nombre_entrada = "./images/" + nombre_entrada;

    ofstream fout;
    fout.open("informe.txt", ios_base::app);
    
    OriginalIimage = imread(nombre_entrada);
    ResizedImage = Mat::zeros(output_height, output_width, CV_8UC3);
    if(!OriginalIimage.data){
        return cout << "Couldn't open or find the image\n", -1;
    }
    
    my_threads = (pthread_t*) malloc(total_threads * sizeof(pthread_t));
    re = (int*) malloc(total_threads * sizeof(int));
    iota(re, re + total_threads, 0);

    auto start = high_resolution_clock::now();
    for(int i = 0; i < total_threads; ++i){
        int current_thread =pthread_create(&my_threads[i], NULL, downSizeImage, &re[i]);
        if(current_thread != 0){
            perror("\nError al crear el hilo");
            exit(-1);
        }
    }

    for(int i = 0; i < total_threads; ++i){
        pthread_join(my_threads[i], NULL);
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
    return 0;
}