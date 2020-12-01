#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define ITERATIONS 1e9
#define THREADS 1024

double piTotal[THREADS];
long long int t1, t2;
float responseTime, period;

typedef long long timestamp_t;
static timestamp_t get_timestamp(){
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

void *calculatePi(void *arg)
{
    int initIteration, endIteration, threadId = *(int *)arg;

    initIteration = (ITERATIONS/THREADS) * threadId;
    endIteration = initIteration + ((ITERATIONS/THREADS) - 1);
    piTotal[threadId] = 0.0;
    do{
        piTotal[threadId] = piTotal[threadId] + (double)(4.0 / ((initIteration*2)+1));
        initIteration++;
        piTotal[threadId] = piTotal[threadId] - (double)(4.0 / ((initIteration*2)+1));
        initIteration++;
    }while(initIteration < endIteration);
    return 0;
}

int main()
{
    int threadId[THREADS], i, *retval;
    pthread_t thread[THREADS];
    period = 1.0 / CLOCKS_PER_SEC;

    t1 = get_timestamp();
    for(i = 0; i < THREADS; i++){
        threadId[i] = i;
        pthread_create(&thread[i], NULL, (void *)calculatePi, &threadId[i]);
    
    }

    for(i = 0; i < THREADS; i++){
        pthread_join(thread[i], (void **)&retval);
    }
    t2 = get_timestamp();
    responseTime = (float)(t2 - t1);

    for(i = 1; i < THREADS; i++){
        piTotal[0] = piTotal[0] + piTotal[i];
    }

    printf("\npi: %2.12f \n", piTotal[0]);
    printf("Response time: %f\n", responseTime * period);
}