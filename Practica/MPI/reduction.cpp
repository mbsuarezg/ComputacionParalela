//mpic++ reduction.cpp `pkg-config --cflags --libs opencv` -fopenmp -o x
//mpirun -np 1 ./x a_720p.jpg test.jpg 4

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

Mat OriginalImage;
Mat ResizedImage;

const int output_height = 480;
const int output_width = 720;

void* downSizeImage(unsigned char* subdest_image, unsigned char* og_image, int globalId, int total_threads){

    int H = OriginalImage.rows, W = OriginalImage.cols;
    int h = output_height, w = output_width;

    int start = globalId * ((h * w + total_threads - 1) / total_threads);
    int end = min(h * w, (globalId + 1) * ((h * w + total_threads - 1) / total_threads));

    for(int i = start, j = 0; i < end; ++i, ++j){
        for(int k = 0; k < 3; ++k){
            *(subdest_image + j*3 + k) = *(og_image + (((H * (i / w)) / h)*W + ((W * (i % w)) / w))*3 + k);
        }
    }
    
    return 0;
}

int main(int argc, char **argv){

    double time = 0.0;
    
    MPI_Init(&argc, &argv);

        int processId, numprocs;
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &processId);

        string input_name = "../images/" + string(argv[1]);
        int threads_per_process = atoi(argv[3]);

        ofstream fout;
        fout.open("informe_mpi.txt", ios_base::app);
        
        OriginalImage = imread(input_name);
        ResizedImage = Mat(output_height, output_width, CV_8UC3);
        
        size_t og_size = OriginalImage.rows * OriginalImage.cols * 3 * sizeof(unsigned char);
        size_t re_size = output_height * output_width * 3 * sizeof(unsigned char);
        size_t subre_size = re_size / numprocs;

        unsigned char* og_image;
        unsigned char* dest_image;
        unsigned char* subdest_image;

        og_image = (unsigned char*) malloc(og_size);
        dest_image = (unsigned char*) malloc(re_size);
        subdest_image = (unsigned char*) malloc(subre_size);
        memcpy(og_image, OriginalImage.ptr(), og_size);

        time -= MPI_Wtime();
        downSizeImage(subdest_image, og_image, processId, numprocs);
        time += MPI_Wtime();

        int tam = output_height * output_width * 3 / numprocs;
        MPI_Gather(subdest_image, tam, MPI_UNSIGNED_CHAR, dest_image, tam, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
        if(processId == 0){
            memcpy(ResizedImage.ptr(), dest_image, re_size); 
            imwrite(argv[2], ResizedImage);
            fout << fixed << setprecision(9);
            fout << "----------------------------------------------------------------------------\n";
            fout << "Número de hilos por proceso: " << threads_per_process << '\n';
            fout << "Número de procesos: " << numprocs << '\n';
            fout << "Tiempo de respuesta: " << time << '\n';
            fout << "Dimensiones de la imagen de entrada: " << OriginalImage.cols << "," << OriginalImage.rows << "\n";
            fout << "----------------------------------------------------------------------------\n\n";
        } 
    MPI_Finalize();
    return 0;
}