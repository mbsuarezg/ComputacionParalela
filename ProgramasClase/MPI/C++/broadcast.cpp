//mpic++ broadcast.cpp -o br
//mpirun -np 4 ./br

#include <bits/stdc++.h>
#include <mpi.h>
const int MSG_LENGTH = 10;

using namespace std;

int main (int argc, char *argv[]){
    int i, tasks, iam, namelen;
    char message[MSG_LENGTH+2];
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    const int root=0;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    strcpy(message, "Hola Mundo");
    MPI_Bcast(message, MSG_LENGTH, MPI_CHAR, root, MPI_COMM_WORLD);
    if(!iam){
        cout << "\nMensaje enviado" << endl;
    }else{
        cout << "\nnode " << iam << ' ' << message << ' ';                   
        MPI_Get_processor_name(processor_name, &namelen);
        cout << "processor " << processor_name << endl;
    }
    MPI_Finalize();
}
