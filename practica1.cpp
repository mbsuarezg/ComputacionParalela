//g++ practica1.cpp -o p -lpthread -lX11 && /p test.jpg
#include <bits/stdc++.h>
#include <pthread.h>
#include <CImg.h>

using namespace std;
using namespace chrono;
using namespace cimg_library;

double** random_matrix;
double** conv;
double* kernel;

const double PI = 3.14159265358979323846;
const int r_start = 0;
const int r_end = 1000;
const int size = 5;
const int knl_size = 3;

mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

double gaussianVal(int x, double dev){    
    double _x = (double)x;
    return exp(-0.5*_x*_x/(dev*dev)) / ((PI + PI)*dev*dev);
}

double randnum(){
    unsigned seed = system_clock::now().time_since_epoch().count();
    static default_random_engine generator (seed);
    uniform_real_distribution<double> distribution (r_start, r_end);
    return distribution(generator);
}

void print_matrix(double** m){
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            cout << *(*(m + i) + j) << ' ';
        }
        cout << '\n';
    }
}

int valid(int a){
    return a < 0 ? 0 : (a > size - 1 ? size - 1 : a);
}

void convolution(double** matrix, double** res, double* kernel){
    int offset = knl_size/2;
    for(int x = 0; x < size; ++x){
        for(int y = 0; y < size; ++y){
            //convolution over the pixel (i,j)
            double cur = 0.0;
            for(int i = 0; i < knl_size; ++i){
                for(int j = 0; j < knl_size; ++j){
                    int newx = valid(x + i - offset);
                    int newy = valid(y + j - offset);
                    double val = (*(kernel + i)) * (*(kernel + j)); //current value of the kernel at pos(i,j)
                    cur += (val * matrix[newx][newy]);
                }
            }
            *(*(res + x) + y) = cur;
        }
    }
}

int main(int argc, const char** argv){

    //memory
    random_matrix = (double**)malloc(size * sizeof(double*));
    conv = (double**)malloc(size * sizeof(double*));
    kernel = (double*)malloc(knl_size * sizeof(double));

    for(int i = 0; i < size; ++i){
        *(random_matrix + i) = (double*)malloc(size * sizeof(double));
        *(conv + i) = (double*)malloc(size * sizeof(double));
    }

    for(int i = 0; i < knl_size; ++i){
        *(kernel + i) = gaussianVal(i - knl_size/2, 0.398);
    }
    
    for(int i = 0; i < size; ++i){
        for(int j = 0; j < size; ++j){
            *(*(random_matrix + i) + j) = randnum();
            *(*(conv + i) + j) = 0;
        }
    }
    cout << "\nRandom matrix:\n";
    print_matrix(random_matrix);
    cout << "\nInitial conv:\n";
    print_matrix(conv);
    cout << "\nKernel vector:\n";
    convolution(random_matrix, conv, kernel);
    for(int i = 0; i < knl_size; ++i){
        for(int j = 0; j < knl_size; ++j){
            cout << *(kernel + i) * *(kernel + j) << ' ';
        }
        cout << '\n';
    }
    cout << "\n\nFinal conv:\n";
    print_matrix(conv);
    cout << '\n';
    free(random_matrix);
    free(conv);
    free(kernel);
    return 0;
}