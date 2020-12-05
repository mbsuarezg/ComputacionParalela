//g++ -fopenmp -lm pi_omp_reduction.cpp -o x && ./x
#include <bits/stdc++.h>
#include <omp.h>

using namespace std;
using namespace chrono;

const int mxn = 2e9;
const int hilos = 32;

int main(){
    double pi = 0.0;
    auto start = high_resolution_clock::now();
    #pragma omp parallel for num_threads(hilos) reduction(+:pi)
    for(int i = 0; i < mxn; ++i){
        pi += 4.0 * (i%2 ? -1 : 1) / (2.0*i + 1);
    }
    auto end = high_resolution_clock::now();
    duration<double, milli> pi_ms = (end - start) / 1000;

    cout << fixed << setprecision(12);
    cout << "Pi: " << pi << '\n';
    cout << "Número de hilos: " << hilos << '\n';
    cout << "Tiempo: " << pi_ms.count() << '\n';
    return 0;
}