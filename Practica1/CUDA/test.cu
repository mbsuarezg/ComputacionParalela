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

int total_threads;

Mat OriginalImage;
Mat ResizedImage;

const int output_height = 480;
const int output_width = 720;

__global__ void vectorAdd(const float *A, const float *B, float *C, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        C[i] = A[i] + B[i];
    }
}

/* Host main routine */
int main(int argc, char** argv)
{
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
    // Print the vector length to be used, and compute its size
    int numElements = 50000;
    size_t size = numElements * sizeof(float);
    printf("[Vector addition of %d elements]\n", numElements);

    // Allocate the host variables
    float *h_A, *h_B, *h_C;
    h_A = (float *)malloc(size);
    h_B = (float *)malloc(size);
    h_C = (float *)malloc(size);

    // Initialize the host input vectors
    for (int i = 0; i < numElements; ++i)
    {
        h_A[i] = rand()/(float)RAND_MAX;
        h_B[i] = rand()/(float)RAND_MAX;
    }

    // Allocate the device input vector A
    float *d_A, *d_B, *d_C;
    cudaMalloc((void **)&d_A, size);
    cudaMalloc((void **)&d_B, size);
    cudaMalloc((void **)&d_C, size);
    

    // Copy the host input vectors A and B in host memory to the device input vectors in device memory
    printf("Copy input data from the host memory to the CUDA device\n");
    cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    //-------------------------------------- Launch the Vector Add CUDA Kernel-----------------------------------------
    int threadsPerBlock = 128;
    int blocksPerGrid =(numElements + threadsPerBlock - 1) / threadsPerBlock;
    printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
    vectorAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, numElements);

    // Copy the device result vector in device memory to the host result vector in host memory
    printf("Copy output data from the CUDA device to the host memory\n");
    cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    printf("Test PASSED\n");

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

