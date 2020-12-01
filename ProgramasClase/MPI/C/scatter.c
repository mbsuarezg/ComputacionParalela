//mpicc scatter.c -o sc -lm
//mpirun -np 4 ./sc

#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#define MAXTASKS 32

int main (int argc, char *argv[])
{
    int i, tag=0, tasks, iam, root=0, buff2send[MAXTASKS], buff2recv;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    if (iam == 0) {
        for(i = 0; i < tasks; i++)
            buff2send[i] = i + 'A';
        MPI_Scatter((void *)buff2send, 1, MPI_INT, (void *)&buff2recv, 1, MPI_INT, root, MPI_COMM_WORLD);

     } else {
        MPI_Comm_rank(MPI_COMM_WORLD, &iam);
        MPI_Scatter((void *)buff2send, 1, MPI_INT, (void *)&buff2recv, 1, MPI_INT, root, MPI_COMM_WORLD);
        printf("received: %i\n", buff2recv);
    }
       MPI_Finalize();
}

