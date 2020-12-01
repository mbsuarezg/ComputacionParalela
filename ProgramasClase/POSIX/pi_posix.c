#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#define MXN 2e9
#define NUM 32

double * pi;
int * re;
long long int t1, t2;
float responseTime, period;

typedef long long timestamp_t;
static timestamp_t get_timestamp(){
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

void * calcPi(void * args){
    int t_id = *(int *) args;
    int first = t_id * (MXN/NUM), last = (t_id + 1) * (MXN/NUM);
    *(pi + t_id) = 0.0;
    double ret = 0.0;
    for(int i = first; i < last;){
        ret += 4.0 / (double)(i + i + 1);
        ++i;
        ret -= 4.0 / (double)(i + i + 1);
        ++i;
    }
    *(pi + t_id) += ret; //BUG FIXED
    return 0;
}

int main(){

    pi = malloc(NUM * sizeof(double));
    re = malloc(NUM * sizeof(int));
    int tot_threads = (int) NUM;
    pthread_t threads[NUM];
    period = 1.0 / CLOCKS_PER_SEC;

    t1 = get_timestamp();
    for(int i = 0; i < NUM; ++i){
        *(re + i) = i;
        int h = pthread_create(&threads[i], NULL, (void *)calcPi, &(*(re + i)));
        if(h){
            perror("Error en la creación del hilo\n");
            exit(-1);
        }
    }

    for(int i = 0; i < NUM; ++i){
        pthread_join(threads[i], NULL);
    }
    
    t2 = get_timestamp();
    responseTime = (float)(t2 - t1);
    for(int i = 1; i < NUM; ++i){
        *(pi) += *(pi + i);
    }

    printf("Pi: %2.12f\n", *(pi));
    printf("Número de hilos: %d\n", tot_threads);
    printf("Response time: %2.12f\n", responseTime * period);
    free(pi);
    free(re);
}