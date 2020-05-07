#include <mpi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int ** allocate_matrix( int size )
{
  /* Allocate 'size' * 'size' ints contiguously. */
  int * vals = (int *) malloc( size * size * sizeof(int) );

  /* Allocate array of int* with size 'size' */
  int ** ptrs = (int **) malloc( size * sizeof(int*) );

  int i;
  for (i = 0; i < size; ++i) {
    ptrs[ i ] = &vals[ i * size ];
  }

  return ptrs;
}

void genMatrix(int **mat, int n)
{
  for(int i = 0; i < n; i++)
  {
    for(int j = 0; j < n; j++)
      mat[i][j] = rand() % n+1;
  }
}


void printMat(int **mat, int n)
{
    printf(" Matrix: \n "); 
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
        printf("%d \t", mat[i][j]);
      printf(" \n ");
    }
}


int main(int argc, char **argv)
{
    int **fst,**sec,**mult,**fstr,**multr;
    int n;
    sscanf (argv[1],"%d",&n);

    //Initialize the MPI environment
    MPI_Init(&argc,&argv);

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

    MPI_Status status;

    //Print off a hello world message
    //printf("Hello world from processor %s, rank %d out of %d process\n",processor_name,world_rank,world_size);
    


    fst = allocate_matrix(n);
    fstr = allocate_matrix(n);
    sec = allocate_matrix(n);
    mult = allocate_matrix(n);
    multr = allocate_matrix(n);

    int *displs = (int *)malloc(world_size*sizeof(int));
    int *scounts = (int *)malloc(world_size*sizeof(int));
    int *vrows = (int *)malloc(world_size*sizeof(int));
    int *offsets = (int *)malloc(world_size*sizeof(int));

    int averow = n / world_size;
    int extra = n % world_size;
    int offset = 0;
    int rows;

    double start = MPI_Wtime();
    for(int dest = 0; dest < world_size; dest++)
    {
      rows = (dest < extra) ? averow + 1 : averow;
      displs[dest] = offset*n;
      scounts[dest] = rows*n;
      vrows[dest] = rows;
      offsets[dest] = offset;
      offset = offset + rows;
    }


    if(world_rank == 0)
    {

      genMatrix(fst,n);
      genMatrix(sec,n);
    }

    MPI_Bcast(&(sec[0][0]),n*n,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatterv(&(fst[offsets[world_rank]][0]),scounts,displs,MPI_INT,&(fstr[offsets[world_rank]][0]),scounts[world_rank],MPI_INT,0,MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);

    for(int k = offsets[world_rank]; k < offsets[world_rank] + vrows[world_rank]; k++)
    {
      for(int i = 0; i < n; i++)
      {
          int sum = 0;
          for(int j = 0; j < n; j++)
          {
              sum = sum + fstr[k][j]*sec[j][i];
          }
          multr[k][i] = sum;
      }
    }

    MPI_Gatherv(&(multr[offsets[world_rank]][0]),scounts[world_rank],MPI_INT,&(mult[offsets[world_rank]][0]),scounts,displs,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);



    if(world_rank == 0)
    {
        double finish = MPI_Wtime();
        double cpu_time_used = finish - start;
        printf("%f,",cpu_time_used);
        if(n == 2000) printf("\n");
    }
    MPI_Finalize();


    return 0;
}