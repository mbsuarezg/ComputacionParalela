//nvcc test.cu -o x -gencode arch=compute_35,code=compute_35 `pkg-config --cflags --libs opencv`
//./x
#include <bits/stdc++.h>

// OpenCV and I/O libraries
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

Mat OriginalIimage;
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
    int val = w / all_threats;
    int start = idx * val;
    int end = (idx + 1) * val;
    
    if(end < w){
        for(int i = start; i < end; ++i){
            for(int j = 0; j < h; ++j){
            
                int xoffset = (idx * H) / h;
                int yoffset = (idx * W) / w;
                #pragma unroll
                for(int k = 0; k < 3; ++k){
                    *(resized + (i*3*output_width + j*3 + k)) = *(original + (xoffset*3*output_width + yoffset*3 + k))                
                }
            }
        }    
    }
}

/* Host main routine */
int main(int argc, char** argv)
{
    //command line input
    char* input_name = argv[1];
    char* output_name = argv[2];
    char* num_blocks = argv[3];
    char* num_threads = argv[4];
    total_blocks = atoi(num_blocks);
    total_threads = atoi(num_threads);
    string nombre_entrada(input_name);
    nombre_entrada = "../images/" + nombre_entrada;

    ofstream fout;
    fout.open("informe.txt", ios_base::app);
    cudaError_t err = cudaSuccess;
    
    //read images
    OriginalIimage = imread(nombre_entrada);
    ResizedImage = Mat::zeros(output_height, output_width, CV_8UC3);
    if(!OriginalIimage.data){
        return cout << "\nCouldn't open or find the image\n", -1;
    }
    size_t og_size = OriginalIimage.cols * OriginalIimage.rows * 3 * sizeof(unsigned char);
    size_t re_size = output_height * output_width * 3 * sizeof(unsigned char);

    // Allocate the host variables
    h_Original = (unsigned char*) malloc(og_size);
    h_Resized = (unsigned char*) malloc(re_size);
    if(!og_image or !dest_image){
        perror("\nError en el malloc de las imagenes");
        exit(-1);
    }

    // Initialize the host input array
    for(int i = 0; i < OriginalIimage.rows; ++i){
        for(int j = 0; j < OriginalIimage.cols; ++j){
            for(int k = 0; k < 3; ++k){
                *(h_Original + (i*OriginalIimage.cols*3 + j*3 + k)) = OriginalIimage.at<Vec3b>(i, j)[k];
            }
        }
    }

    // Allocate the device input vector A
    unsigned char* d_Original;
    unsigned char* d_Resized;
    err = cudaMalloc((void **)&d_Original, OriginalIimage.rows * OriginalIimage.cols * 3 * sizeof (unsigned char));
    my_cudaError(err, "Fallo el malloc en el device de el array de la imagen original");

    err = cudaMalloc((void **)&d_Resized, output_height * output_width * 3 * sizeof(unsigned char));
    my_cudaError(err, "Fallo el malloc en el device de el array de la imagen de salida");    

    // Copy the host input vectors A and B in host memory to the device input vectors in device memory
    err = cudaMemcpy(d_Original, h_Original, og_size, cudaMemcpyHostToDevice);
    my_cudaError(err, "Fallo en el memcpy del devie para la imagen original");

    err = cudaMemcpy(d_Resized, d_Original, re_size, cudaMemcpyHostToDevice);
    my_cudaError(err, "Fallo en el memcpy del devie para la imagen de salida");

    //-------------------------------------- Launch the downsize CUDA Kernel-----------------------------------------
    int all_threats = total_blocks * total_threads;
    downSizeImage<<<total_blocks, total_threads>>>(d_Original, OriginalIimage.cols, OriginalIimage.rows, output_width, output_height, all_threats);

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

    // Free device global memory
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    // Free host memory
    free(h_A);
    free(h_B);
    free(h_C);

    printf("Done\n");
    return 0;
}

