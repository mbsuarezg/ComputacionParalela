//mpic++ reduction.cpp `pkg-config --cflags --libs opencv` -fopenmp -o x
//mpirun -np 4 ./x a_720p.jpg test.jpg 1

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

void* downSizeImage(unsigned char* subdest_image, unsigned char* og_ime, int global_id, int thrds_pcs, int num_procs, int thread_id){

    int H = OriginalImage.rows, W = OriginalImage.cols;
    int h = output_height, w = output_width;

    int total_threads = num_procs * thrds_pcs;
    int start = global_id * ((h * w + total_threads - 1) / total_threads);
    int group_size = (h * w) / total_threads;
    int thread_start = thread_id * group_size;
    int thread_end = (thread_id + 1) * group_size;

    for(int i = start, j = thread_start; j < thread_end; ++i, ++j){
        for(int k = 0; k < 3; ++k){
            *(subdest_image + j*3 + k) = *(og_ime + (((H * (i / w)) / h)*W + ((W * (i % w)) / w))*3 + k);
        }
    }
    
    return 0;
}

int main(int argc, char **argv){

    double time = 0.0;
    
    MPI_Init(&argc, &argv);

        int process_id, num_procs;
        MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
        MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

        string input_name = "../images/" + string(argv[1]);
        int threads_per_process = atoi(argv[3]);

        ofstream fout;
        fout.open("informe_mpi.txt", ios_base::app);
        
        OriginalImage = imread(input_name);
        ResizedImage = Mat(output_height, output_width, CV_8UC3);
        
        size_t og_size = OriginalImage.rows * OriginalImage.cols * 3 * sizeof(unsigned char);
        size_t re_size = output_height * output_width * 3 * sizeof(unsigned char);
        size_t subre_size = re_size / num_procs;

        unsigned char* og_image;
        unsigned char* dest_image;
        unsigned char* subdest_image;

        og_image = (unsigned char*) malloc(og_size);
        dest_image = (unsigned char*) malloc(re_size);
        subdest_image = (unsigned char*) malloc(subre_size);
        memcpy(og_image, OriginalImage.ptr(), og_size);

        time -= MPI_Wtime();
        #pragma omp parallel num_threads(threads_per_process)
        {
            int thread_id = omp_get_thread_num();
            int total_threads = omp_get_num_threads();
            int global_id = (process_id * total_threads) + thread_id;
            downSizeImage(subdest_image, og_image, global_id, total_threads, num_procs, thread_id);
        }
        time += MPI_Wtime();

        int tam = output_height * output_width * 3 / num_procs;
        MPI_Gather(subdest_image, tam, MPI_UNSIGNED_CHAR, dest_image, tam, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
        if(process_id == 0){
            memcpy(ResizedImage.ptr(), dest_image, re_size); 
            imwrite(argv[2], ResizedImage);
            fout << fixed << setprecision(9);
            fout << "----------------------------------------------------------------------------\n";
	    fout << "Número de procesos: " << num_procs << '\n';
            fout << "Número de hilos por proceso: " << threads_per_process << '\n';
            fout << "Tiempo de respuesta: " << time << '\n';
            fout << "Dimensiones de la imagen de entrada: " << OriginalImage.cols << "," << OriginalImage.rows << "\n";
            fout << "----------------------------------------------------------------------------\n\n";
        } 
    MPI_Finalize();
    return 0;
}
