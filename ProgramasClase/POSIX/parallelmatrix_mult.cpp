#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;
using namespace chrono;

const int threads = 4;
const int dimension = 512;
const int r_start = 0;
const int r_end = 1000;

double ** A;
double ** B;
double ** C;
int * re;

mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

void multiply_matrix(){
    for(int i = 0; i < dimension; ++i){
        for(int j = 0; j < dimension; ++j){
            for(int k = 0; k < dimension; ++k){
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }    
}

void* multiply_parallel(void* args){
    //cada hilo un número de vectores en concreto (blockwise)--> threads < n
    int t_id = *(int*)args;
    int start = t_id * (dimension/threads), end = (t_id + 1) * (dimension/threads);
    for(int i = start; i < end; ++i){
        for(int j = 0; j < dimension; ++j){
            for(int k = 0; k < dimension; ++k){
                *(*(C + i) + j) += ( (*(*(A + i) + k)) * (*(*(B + k) + j)) );
            }
        }
    }   
    /*printf("hilo número: %d, inicio: %d, fin: %d\n", t_id, start, end); //debug*/
    return 0;
}

void print_matrix(double** matrix){
    for(int i = 0; i < dimension; ++i){
        for(int j = 0; j < dimension; ++j){
            cout << *(*(matrix + i) + j) << ' ';
        }
        cout << '\n';
    }
}

double randnum(){
    unsigned seed = system_clock::now().time_since_epoch().count();
    static default_random_engine generator (seed);
    uniform_real_distribution<double> distribution (r_start, r_end);
    return distribution(generator);
}

int main(){
    ios_base::sync_with_stdio(0); cin.tie(0); // USE CAST IN MALLOC IF YOU ARE USING C++

    //memory
    A = (double**)malloc(dimension * sizeof(double*));
    for(int i = 0; i < dimension; ++i){
        A[i] = (double*)malloc(dimension * sizeof(double));
    }
    B = (double**)malloc(dimension * sizeof(double*));
    for(int i = 0; i < dimension; ++i){
        B[i] = (double*)malloc(dimension * sizeof(double));
    }
    C = (double**)malloc(dimension * sizeof(double*));
    for(int i = 0; i < dimension; ++i){
        C[i] = (double*)malloc(dimension * sizeof(double));
    }
    re = (int*)malloc(threads * sizeof(int));
    iota(re, re + threads, 0);

    //filling A and B with random double values in: [r_start, r_end]
    for(int i = 0; i < dimension; ++i){
        for(int j = 0; j < dimension; ++j){
            A[i][j] = randnum();
        }
    }   
    for(int i = 0; i < dimension; ++i){
        for(int j = 0; j < dimension; ++j){
            B[i][j] = randnum();
        }
    }

    pthread_t my_threads[threads];

    auto start = high_resolution_clock::now();
    //multiply_matrix(); ---> programa no paralelizado
    for(int i = 0; i < threads; ++i){
        int current_thread = pthread_create(&my_threads[i], NULL, multiply_parallel, &re[i]);
        if(current_thread != 0){
            perror("Error al crear el hilo\n");
            exit(-1);
        }
    }
    for(int i = 0; i < threads; ++i){
        pthread_join(my_threads[i], NULL);
    }
    auto end = high_resolution_clock::now();
    duration<double, milli> t_total = (end - start);

    cout << fixed << setprecision(6);
    /*
    print_matrix(A);
    cout << '\n';
    print_matrix(B);
    cout << '\n';
    print_matrix(C);
    cout << '\n';
    */

    //memory and print
    cout << "Dimension de las matrices: " << dimension << '\n';
    cout << "Número de hilos: " << threads << '\n';
    cout << "Tiempo total: " << t_total.count() / 1000 << " segundos\n";
    free(A);
    free(B);
    free(C);
    return 0;
}