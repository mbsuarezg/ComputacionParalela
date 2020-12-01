//g++ -fopenmp pi_omp_noFalseSharing.cpp -o x && ./x
#include <bits/stdc++.h>
#include <omp.h>

using namespace std;
using namespace chrono;

const int mxn = int(2e09);
const int threads = 16;
const int padding = 8;

double* pi;

void* sumar_pi(int opm_id){
    int inicio = opm_id * (mxn / threads);
    int fin = (opm_id + 1) * (mxn / threads);
    double sum_id = 0.0;
    while(inicio < fin){
        sum_id += 4.0 / (inicio + inicio + 1);
        ++inicio;
        sum_id -= 4.0 / (inicio + inicio + 1);
        ++inicio;
    }
    int gap = opm_id * padding;
    *(pi + gap) += sum_id;
    return 0;
}

int main(){
    pi = (double*)malloc(padding * threads * sizeof(double));
    auto start = high_resolution_clock::now();
    #pragma omp parallel num_threads(threads)
    {
        int opm_id = omp_get_thread_num();
        sumar_pi(opm_id); 
    }
    for(int i = 1; i < threads; ++i){
        *pi += *(pi + (i * padding));
    }
    auto end = high_resolution_clock::now();
    duration<double, milli> pi_ms = (end - start) / 1000;

    cout << fixed << setprecision(12);
    cout << "Pi: " << *pi << '\n';
    cout << "Número de hilos: " << threads << '\n';
    cout << "Tiempo: " << pi_ms.count() << '\n';

    free(pi);
    return 0;
}