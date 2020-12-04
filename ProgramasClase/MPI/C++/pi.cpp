//mpic++ pi.cpp -o pimpi -fopenmp
//mpirun -np 4 pimpi

#include <bits/stdc++.h>
#include <mpi.h>
#include <omp.h>

using namespace std;

const int ITERATIONS = 1e9;
const int NUMTHREADS = 4;

int calculatePi(double *pi, int ID, int totalprocs){
	int start = ID * (ITERATIONS / totalprocs);
	int end = (ID + 1) * (ITERATIONS / totalprocs);
	while(start < end){
		*pi += 4.0 / (start + start + 1);
		++start;
		*pi -= 4.0 / (start + start + 1);
		++start;
	}
	return 0;
}

int main(int argc, char *argv[]){
	int processId, numprocs;
	double PI25DT = 3.1415926535897932384626433;
	double local_pi[NUMTHREADS], global_pi = 0.0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	if(!processId) cout << "\nLaunching with " << numprocs << " processes and " << NUMTHREADS << " threads";
	#pragma omp parallel num_threads(NUMTHREADS)
	{
		int threadId = omp_get_thread_num();
		int threadsTotal = omp_get_num_threads();
		int globalId = (processId * threadsTotal) + threadId;
		calculatePi(&local_pi[threadId], globalId, threadsTotal*numprocs);
		#pragma omp single
		{
			for(int i = 0; i < threadsTotal; i++)
				global_pi += local_pi[i];
		}
	}
	MPI_Reduce(local_pi, &global_pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if(!processId) cout << fixed << setprecision(12) << "\npi is approximately " << global_pi << ", Error is " << fabs(global_pi - PI25DT) << '\n';
	MPI_Finalize();
	return 0;
}