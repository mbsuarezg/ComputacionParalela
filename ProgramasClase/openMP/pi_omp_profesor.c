//gcc pi_omp_profesor.c -o x -fopenmp -lm && ./x
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define ITERATIONS 2e09
#define THREADS 8
#define PAD 8

int calculatePi(double *piTotal, int ID)
{   int start, end;   

    start = (ITERATIONS/omp_get_num_threads()) * ID;
    end = (ITERATIONS/omp_get_num_threads()) * (ID + 1);
    int i = start;

    do{
        *(piTotal + (ID*PAD)) = *(piTotal + (ID*PAD)) + (double)(4.0 / ((i*2)+1));
        i++;
        *(piTotal + (ID*PAD)) = *(piTotal + (ID*PAD)) - (double)(4.0 / ((i*2)+1));
        i++;
    }while(i < end);

    return 0;
}


int main()
{
    int i, threads = THREADS;
    
    double pi[threads*PAD];
    
    struct timeval tval_before, tval_after, tval_result;

    gettimeofday(&tval_before, NULL);
    
    for(i = 0; i < THREADS; i++)
        pi[i*PAD] = 0;
        
    #pragma omp parallel num_threads(threads)
    {
        int ID = omp_get_thread_num();
        calculatePi(pi, ID);
    }
    
    for(i = 1; i < THREADS; i++)
    {
        *pi = *pi + *(pi + (i*PAD));
    }
    gettimeofday(&tval_after, NULL);

    timersub(&tval_after, &tval_before, &tval_result);

    printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    printf("pi: %2.10f   \n", pi[0]);
}
