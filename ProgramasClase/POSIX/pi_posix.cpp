#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;
using namespace chrono;
using ll = long long;
using ld = long double;

const int mxn = 1e9;
const int hilos = 16;

double pi = 0.0;
int  re;

void* sumar_pi(void* args){
    int hilo_id = *(int*) args, inicio = hilo_id * (mxn / hilos), fin = (hilo_id + 1) * (mxn / hilos);
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
    //ios_base::sync_with_stdio(0); cin.tie(0);

    pthread_t mis_hilos[hilos];
    re = (int*)malloc(hilos * sizeof(int));
    iota(re, re + hilos, 0);

    //auto start = high_resolution_clock::now();
    for(int i = 0; i < hilos; ++i){
        pthread_create(&mis_hilos[i], nullptr, sumar_pi, &re[i]);
    }
    for(int i = 0; i < hilos; ++i){
        pthread_join(mis_hilos[i], nullptr);
    }
    //auto end = high_resolution_clock::now();
    //duration<double, milli> pi_ms = (end - start) / 1000;

    cout << fixed << setprecision(12);
    cout << "Pi: " << pi << '\n';
    cout << "Número de hilos: " << hilos << '\n';
    //cout << "Tiempo: " << pi_ms.count() << '\n';
    return 0;
}