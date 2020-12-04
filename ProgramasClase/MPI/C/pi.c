//mpicc pi.c -o pimpi -lm -fopenmp
//mpirun -np 4 ./pimpi

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include <math.h>

#define ITERATIONS 1e09
#define NUMTHREADS 4

int calculatePi(double *pi, int ID, int numprocs)
{
	long int start, end;
	start = (ITERATIONS/numprocs) * ID;
	end = (ITERATIONS/numprocs) * (ID + 1);
	long int i = start;
	do{
		*pi = *pi + (double)(4.0 / ((i*2)+1));
		i++;
		*pi = *pi - (double)(4.0 / ((i*2)+1));
		i++;
	}while(i < end);

	return 0;
}

int main(int argc, char *argv[])
{
	int done = 0, n, processId, numprocs, I, rc, i;
	double PI25DT = 3.1415926535897932384626433;
	double local_pi[NUMTHREADS], global_pi;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	if (processId == 0) printf("\nLaunching with %i processes and %i threads", numprocs, NUMTHREADS);
	global_pi = 0.0;

	#pragma omp parallel num_threads(NUMTHREADS)
	{
		int threadId = omp_get_thread_num();
		int threadsTotal = omp_get_num_threads();
		int globalId = (processId * threadsTotal) + threadId;
		calculatePi(&local_pi[threadId], globalId, threadsTotal*numprocs);
		#pragma omp single
		{
			for(i = 0; i < threadsTotal; i++)
				global_pi = global_pi + local_pi[i];
		}
	}
	MPI_Reduce(local_pi, &global_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (processId == 0) printf("\npi is approximately %.16f, Error is %.16f\n", global_pi, fabs(global_pi - PI25DT));
	MPI_Finalize();
	return 0;
}