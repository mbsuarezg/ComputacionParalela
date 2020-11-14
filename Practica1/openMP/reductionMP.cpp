#include <bits/stdc++.h>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <omp.h>

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

void* downSizeImage(int omp_id){

    int H = OriginalImage.rows;
    int W = OriginalImage.cols;
    int h = output_height;
    int w = output_width;

    int start = omp_id * ((h * w + total_threads - 1) / total_threads);
    int end = min(h * w, (omp_id + 1) * ((h * w + total_threads - 1) / total_threads));

    for(int i = start; i < end; ++i){
        int a = (H * (i / w)) / h;
        int b = (W * (i % w)) / w;
        for(int k = 0; k < 3; ++k){
            *(dest_image + i*3 + k) = *(og_image + (a*W + b)*3 + k);
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
    fout.open("informe_open_mp.txt", ios_base::app);
    
    OriginalImage = imread(nombre_entrada);
    ResizedImage = Mat::zeros(output_height, output_width, CV_8UC3);
    if(!OriginalImage.data){
        return cout << "Couldn't open or find the image\n", -1;
    }
    
    og_image = (unsigned char*) malloc(OriginalImage.rows * OriginalImage.cols * 3 * sizeof(unsigned char));
    dest_image = (unsigned char*) malloc(output_height * output_width * 3 * sizeof(unsigned char));
    if(!og_image or !dest_image){
        perror("\nError en el malloc de las imagenes");
        exit(-1);
    }

    for(int i = 0; i < OriginalImage.rows; ++i){
        for(int j = 0; j < OriginalImage.cols; ++j){
            for(int k = 0; k < 3; ++k){
                *(og_image + (i*OriginalImage.cols*3 + j*3 + k)) = OriginalImage.at<Vec3b>(i, j)[k];
            }
        }
    }

    auto start = high_resolution_clock::now();
    #pragma omp parallel num_threads(total_threads)
    {
        int omp_id = omp_get_thread_num();
        downSizeImage(omp_id);
    }
    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);

    for(int i = 0; i < output_height; ++i){
        for(int j = 0; j < output_width; ++j){
            for(int k = 0; k < 3; ++k){
                ResizedImage.at<Vec3b>(i, j)[k] = *(dest_image + (i*output_width*3 + j*3 + k));
            }
        }
    }
    
    imwrite(output_name, ResizedImage);

    fout << fixed << setprecision(9);
    fout << "----------------------------------------------------------------------------\n";
    fout << "Número de hilos: " << total_threads << '\n';
    fout << "Tiempo de respuesta: " << total_time.count() / 1000 << '\n';
    fout << "Dimensiones de la imagen de entrada: " << OriginalImage.cols << "," << OriginalImage.rows << "\n";
    fout << "----------------------------------------------------------------------------\n\n";
    return 0;
}