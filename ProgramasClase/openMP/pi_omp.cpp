//g++ -fopenmp -lm pi_omp.cpp -o x && ./x
#include <bits/stdc++.h>
#include <omp.h>

using namespace std;
using namespace chrono;

const int mxn = 2e9;
const int hilos = 8;

double pi = 0.0;

void* sumar_pi(int mp_id){
    int inicio = mp_id * (mxn / hilos);
    int fin = (mp_id + 1) * (mxn / hilos);
    double sum_id = 0.0;
    while(inicio < fin){
        sum_id += 4.0 / (inicio + inicio + 1);
        ++inicio;
        sum_id -= 4.0 / (inicio + inicio + 1);
        ++inicio;
    }
    pi += sum_id;
    return 0;
}

int main(){
    auto start = high_resolution_clock::now();
    #pragma omp parallel num_threads(hilos)
    {
        int mp_id = omp_get_thread_num();
        sumar_pi(mp_id); 
    }
    auto end = high_resolution_clock::now();
    duration<double, milli> pi_ms = (end - start) / 1000;

    cout << fixed << setprecision(12);
    cout << "Pi: " << pi << '\n';
    cout << "Número de hilos: " << hilos << '\n';
    cout << "Tiempo: " << pi_ms.count() << '\n';
    return 0;
}