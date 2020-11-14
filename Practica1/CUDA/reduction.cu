//GTX 750, ( 4) Multiprocessors, (192) CUDA Cores/MP: 768 CUDA Cores
// OpenCV and I/O libraries
#include <bits/stdc++.h>
#include <fstream>
#include <opencv2/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>
#include <helper_cuda.h>

using namespace std;
using namespace chrono;
using namespace cv;

int total_blocks;
int total_threads;
int all_threats;

Mat OriginalImage;
Mat ResizedImage;

const int output_height = 480;
const int output_width = 720;

unsigned char* h_Original;
unsigned char* h_Resized;

void my_cudaError(cudaError_t err, string errorMessage){
    if(err != cudaSuccess){
        fprintf(stderr, "\nError: %s", errorMessage, cudaGetErrorString(err));
    }
}

__global__ void downSizeImage(const unsigned char *original, unsigned char *resized, int W, int H, int w, int h, int all_threats){

    int idx = blockDim.x * blockIdx.x + threadIdx.x;
    int start = idx * ((h * w + all_threats - 1) / all_threats); 
    int end = min(h * w, (idx + 1) * ((h * w + all_threats - 1) / all_threats));

    for(int i = start; i < end; ++i){
        #pragma unroll
        for(int k = 0; k < 3; ++k){
            *(resized + i*3 + k) = *(original + (((H * (i / w)) / h)*W + ((W * (i % w)) / w))*3 + k);
        }
    }

}

/* Host main routine */
int main(int argc, char** argv){

    // Command line input
    char* input_name = argv[1];
    char* output_name = argv[2];
    char* num_blocks = argv[3];
    char* num_threads = argv[4];
    total_blocks = atoi(num_blocks);
    total_threads = atoi(num_threads);
    all_threats = total_blocks * total_threads;
    string nombre_entrada(input_name);
    nombre_entrada = "../images/" + nombre_entrada;

    ofstream fout;
    fout.open("informe_cuda.txt", ios_base::app);
    cudaError_t err = cudaSuccess;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    
    // Read images
    OriginalImage = imread(nombre_entrada);
    ResizedImage = Mat::zeros(output_height, output_width, CV_8UC3);
    if(!OriginalImage.data){
        return cout << "\nCouldn't open or find the image\n", -1;
    }
    size_t og_size = OriginalImage.cols * OriginalImage.rows * 3 * sizeof(unsigned char);
    size_t re_size = output_height * output_width * 3 * sizeof(unsigned char);

    // Allocate the host variables
    h_Original = (unsigned char*) malloc(og_size);
    h_Resized = (unsigned char*) malloc(re_size);
    if(!h_Original or !h_Resized){
        perror("\nError en el malloc de las imagenes en host");
        exit(-1);
    }

    // Initialize the host input array
    for(int i = 0; i < OriginalImage.rows; ++i){
        for(int j = 0; j < OriginalImage.cols; ++j){
            for(int k = 0; k < 3; ++k){
                *(h_Original + (i*OriginalImage.cols*3 + j*3 + k)) = OriginalImage.at<Vec3b>(i, j)[k];
            }
        }
    }

    // Allocate the device input arrays
    unsigned char* d_Original;
    unsigned char* d_Resized;
    err = cudaMalloc((void **)&d_Original, og_size);
    my_cudaError(err, "Fallo el malloc en el device del array de la imagen original");
    err = cudaMalloc((void **)&d_Resized, re_size);
    my_cudaError(err, "Fallo el malloc en el device del array de la imagen de salida");    

    // Copy the host input vectors A and B in host memory to the device input vectors in device memory
    err = cudaMemcpy(d_Original, h_Original, og_size, cudaMemcpyHostToDevice);
    my_cudaError(err, "Fallo en el memcpy del devie para la imagen original");
    err = cudaMemcpy(d_Resized, d_Original, re_size, cudaMemcpyHostToDevice);
    my_cudaError(err, "Fallo en el memcpy del devie para la imagen de salida");

    //-------------------------------------- Launch the downsize CUDA Kernel-----------------------------------------
    cudaEventRecord(start);
    downSizeImage<<<total_blocks, total_threads>>>(d_Original, d_Resized, OriginalImage.cols, OriginalImage.rows, output_width, output_height, all_threats);
    cudaEventRecord(stop);
    //----------------------------------------------------------------------------------------------------------------

    // Copy the device result array in device memory to the host result array in host memory
    err = cudaMemcpy(h_Resized, d_Resized, re_size, cudaMemcpyDeviceToHost);
    my_cudaError(err, "Fallo al traer la imagen del device");
    for(int i = 0; i < output_height; ++i){
        for(int j = 0; j < output_width; ++j){
            for(int k = 0; k < 3; ++k){
                ResizedImage.at<Vec3b>(i, j)[k] = *(h_Resized + (i*output_width*3 + j*3 + k));
            }
        }
    }
    imwrite(output_name, ResizedImage);

    //Gather cuda time
    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    // Free device global memory
    err = cudaFree(d_Original);
    my_cudaError(err, "Error al liberar la memoria global de device");
    err = cudaFree(d_Resized);
    my_cudaError(err, "Error al liberar la memoria global de device");

    // Free host memory
    free(h_Original);
    free(h_Resized);

    // Prints
    fout << fixed << setprecision(12);
    fout << "----------------------------------------------------------------------------\n";
    fout << "Número de bloques: " << total_blocks << '\n';
    fout << "Número de hilos por bloque: " << total_threads << '\n';
    fout << "Número total de hilos: " << all_threats << '\n';
    fout << "Tiempo de respuesta (CUDA): " << milliseconds / 1000 << '\n';
    fout << "Dimensiones de la imagen de entrada: " << OriginalImage.cols << "," << OriginalImage.rows << "\n";
    fout << "----------------------------------------------------------------------------\n\n";
    return 0;
}