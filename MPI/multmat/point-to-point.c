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
    int **fst,**sec,**mult;
    int n;
    sscanf (argv[1],"%d",&n);

    //Initialize the MPI environment
    MPI_Init(&argc,&argv);

    //Get the number of proccesses
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int num_workers = world_size - 1;

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
    
    int ext = n / world_size;
    int averow,extra,offset,rows;

    fst = allocate_matrix(n);
    sec = allocate_matrix(n);
    mult = allocate_matrix(n);
    //MASTER
    if(world_rank == 0)
    {

        genMatrix(fst,n);
        genMatrix(sec,n);
        //printMat(fst,n);
        //printMat(sec,n);

        averow = n / num_workers;
        extra = n % num_workers;
        offset = 0;

        /* Measure start time */
        double start = MPI_Wtime();

        for(int dest = 1; dest <= num_workers; dest++)
        {
            rows = (dest <= extra) ? averow + 1 : averow;
            MPI_Send(&offset,1,MPI_INT,dest,0,MPI_COMM_WORLD);
            MPI_Send(&rows,1,MPI_INT,dest,1,MPI_COMM_WORLD);
            MPI_Send(&(fst[0][0]),n*n,MPI_INT,dest,2,MPI_COMM_WORLD);
            MPI_Send(&(sec[0][0]),n*n,MPI_INT,dest,3,MPI_COMM_WORLD);
            offset = offset + rows;
        }

        for(int source = 1; source <= num_workers; source++)
        {
          MPI_Recv(&offset, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
          MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
          MPI_Recv(&(mult[offset][0]), rows * n, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
        }
        //printMat(mult,n);

        double finish = MPI_Wtime();

        double cpu_time_used = finish - start;
        printf("%f,",cpu_time_used);
        if(n == 2000) printf("\n");
    }

    if(world_rank != 0)
    {
      int number;
      MPI_Recv(&offset,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
      MPI_Recv(&rows,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
      MPI_Recv(&(fst[0][0]),n*n,MPI_INT,0,2,MPI_COMM_WORLD,&status);
      MPI_Recv(&(sec[0][0]),n*n,MPI_INT,0,3,MPI_COMM_WORLD,&status);

      int top = offset + rows;
      for(int i = offset;i < top;i++)
      {
        for(int j = 0; j < n; j++)
        {
            int r = 0;
            for(int k = 0; k < n; k++)
                r += fst[i][k] * sec[k][j];
            mult[i][j] = r;
        }
      }
      
      MPI_Send(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&(mult[offset][0]),rows*n,MPI_INT,0,2,MPI_COMM_WORLD);


    }

    MPI_Finalize();

    return 0;
}
