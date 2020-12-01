//mpic++ gather.cpp -o gt
//mpirun -np 4 ./gt

#include <bits/stdc++.h>
#include <mpi.h>
const int MAXTASKS = 32;

using namespace std;

int main (int argc, char *argv[]){
    int i, tasks, iam, root=0, buff2send, buff2recv[MAXTASKS];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    buff2send = iam;
    MPI_Gather((void *)&buff2send, 1, MPI_INT, buff2recv, 1, MPI_INT, root, MPI_COMM_WORLD);
    if(!iam){
        for(i = 0; i < tasks; i++){
            cout << buff2recv[i] << '\n';
        }
    }
    MPI_Finalize();
}
