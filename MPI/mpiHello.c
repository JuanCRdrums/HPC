#include <mpi.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    //Initialize the MPI environment
    MPI_Init(NULL,NULL);

    //Get the number of proccesses
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Get the rank of the proccesses
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    //Get the name of the proccesor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    //Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d process\n",processor_name,world_rank,world_size);
    return 0;
}
