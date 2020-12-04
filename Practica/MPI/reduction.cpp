//
//

#include <bits/stdc++.h>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <mpi.h>
#include <omp.h>

using namespace std;
using namespace chrono;
using namespace cv;

int total_threads;

const int output_height = 480;
const int output_width = 720;

unsigned char* og_image;
unsigned char* dest_image;

void *downSizeImage(int *acc, int ID, int totalprocs){

	int H = OriginalImage.rows;
    int W = OriginalImage.cols;
    int h = output_height;
    int w = output_width;


	return 0;
}

int main(int argc, char **argv){

	char* input_name = argv[1];
    char* output_name = argv[2];
    char* num_threads = argv[3];
    total_threads = atoi(num_threads);
    string nombre_entrada(input_name);
    nombre_entrada = "../images/" + nombre_entrada;

    ofstream fout;
    fout.open("informe_open_mp.txt", ios_base::app);
    
    OriginalImage = imread(nombre_entrada);
    ResizedImage = Mat(output_height, output_width, CV_8UC3);
    if(!OriginalImage.data){
        return cout << "Couldn't open or find the image\n", -1;
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

    auto start = high_resolution_clock::now();
    MPI_Init(&argc, &argv);
    //LOGIC
    MPI_Finalize();
    auto end = high_resolution_clock::now();
    duration<double, milli> total_time = (end - start);

    memcpy(ResizedImage.ptr(), dest_image, re_size); 
    imwrite(output_name, ResizedImage);

    fout << fixed << setprecision(9);
    fout << "----------------------------------------------------------------------------\n";
    fout << "Número de hilos: " << total_threads << '\n';
    fout << "Tiempo de respuesta: " << total_time.count() / 1000 << '\n';
    fout << "Dimensiones de la imagen de entrada: " << OriginalImage.cols << "," << OriginalImage.rows << "\n";
    fout << "----------------------------------------------------------------------------\n\n";
    return 0;
}