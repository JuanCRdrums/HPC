#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "time.h"

/* --
 * Do nsweeps sweeps of Jacobi iteration on a 1D Poisson problem
 * 
 *    -u'' = f
 *
 * discretized by n+1 equally spaced mesh points on [0,1].
 * u is subject to Dirichlet boundary conditions specified in
 * the u[0] and u[n] entries of the initial vector.
 */
void jacobi(int nsweeps, int n, double* u, double* f)
{
    int i, sweep;
    double h  = 1.0 / n;
    double h2 = h*h;
    double* utmp = (double*) malloc( (n+1) * sizeof(double) );

    /* Fill boundary conditions into utmp */
    utmp[0] = u[0];
    utmp[n] = u[n];

    for (sweep = 0; sweep < nsweeps; sweep += 2) {
        
        /* Old data in u; new data in utmp */
        for (i = 1; i < n; ++i)
            utmp[i] = (u[i-1] + u[i+1] + h2*f[i])/2;
        
        /* Old data in utmp; new data in u */
        for (i = 1; i < n; ++i)
            u[i] = (utmp[i-1] + utmp[i+1] + h2*f[i])/2;
    }

    free(utmp);
}


void write_solution(int n, double* u, const char* fname)
{
    int i;
    double h = 1.0 / n;
    FILE* fp = fopen(fname, "w+");
    for (i = 0; i <= n; ++i)
        fprintf(fp, "%g %g\n", i*h, u[i]);
    fclose(fp);
}

void write_vector(double *u,int offset,int n)
{
    for(int i = offset; i < n+offset; i++)
        printf("%f ",u[i]);
    printf("\n");
}


int main(int argc, char** argv)
{
    int i;
    int n, nsteps;
    double* u;
    double* f;
    double h;
    char* fname;

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

    MPI_Status status,statuses[num_workers];


    /* Process arguments */
    n      = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    fname  = (argc > 3) ? argv[3] : NULL;
    h      = 1.0/n;

    int sweep;
    h  = 1.0 / n;
    double h2 = h*h;
    double* utmp = (double*) malloc( (n+1) * sizeof(double) );

    int averow,extra,offset,rows;

    /* Allocate and initialize arrays */
    u = (double*) malloc( (n+1) * sizeof(double) );
    f = (double*) malloc( (n+1) * sizeof(double) );

    MPI_Request request[num_workers];

    if(world_rank == 0)
    {
        memset(u, 0, (n+1) * sizeof(double));
        for (i = 0; i <= n; ++i)
            f[i] = i * h;


        /* Fill boundary conditions into utmp */
        utmp[0] = u[0];
        utmp[n] = u[n];

        /* Measure start time */
        double start = MPI_Wtime();


        /* Fill boundary conditions into utmp */
        utmp[0] = u[0];
        utmp[n] = u[n];

        averow = (n-1) / num_workers;
        extra = (n-1) % num_workers;
        offset = 1;

        for (sweep = 0; sweep < nsteps; sweep += 2) {
        
            /* Old data in u; new data in utmp */
            offset = 1;
            for(int dest = 1; dest <= num_workers; dest++)
            {
                rows = (dest <= extra) ? averow + 1 : averow;
                MPI_Send(&offset,1,MPI_INT,dest,0,MPI_COMM_WORLD);
                MPI_Send(&rows,1,MPI_INT,dest,1,MPI_COMM_WORLD);
                MPI_Send(&u[offset],rows,MPI_DOUBLE,dest,2,MPI_COMM_WORLD);
                MPI_Send(&f[offset],rows,MPI_DOUBLE,dest,3,MPI_COMM_WORLD);
                MPI_Send(&u[offset-1],1,MPI_DOUBLE,dest,4,MPI_COMM_WORLD);
                MPI_Send(&u[offset+rows],1,MPI_DOUBLE,dest,5,MPI_COMM_WORLD);
                offset = offset + rows;
            }
            for(int source = 1; source <= num_workers; source++)
            {
                MPI_Recv(&offset, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
                MPI_Irecv(&utmp[offset], rows, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &request[source-1]);
            }
            
            MPI_Waitall(num_workers,request,statuses);
            offset = 1;
            /* Old data in utmp; new data in u */
            for(int dest = 1; dest <= num_workers; dest++)
            {
                rows = (dest <= extra) ? averow + 1 : averow;
                MPI_Send(&offset,1,MPI_INT,dest,0,MPI_COMM_WORLD);
                MPI_Send(&rows,1,MPI_INT,dest,1,MPI_COMM_WORLD);
                MPI_Send(&utmp[offset],rows,MPI_DOUBLE,dest,2,MPI_COMM_WORLD);
                MPI_Send(&f[offset],rows,MPI_DOUBLE,dest,3,MPI_COMM_WORLD);
                MPI_Send(&utmp[offset-1],1,MPI_DOUBLE,dest,4,MPI_COMM_WORLD);
                MPI_Send(&utmp[offset+rows],1,MPI_DOUBLE,dest,5,MPI_COMM_WORLD);
                offset = offset + rows;
            }

            for(int source = 1; source <= num_workers; source++)
            {
                MPI_Recv(&offset, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
                MPI_Irecv(&u[offset], rows, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &request[source-1]);
            }
            MPI_Waitall(num_workers,request,statuses);

        }


        double end = MPI_Wtime();
        double cpu_time_used = end - start;

        /* Run the solver */    
        /*printf("n: %d\n"
            "nsteps: %d\n"
            "Elapsed time: %g s\n", 
            n, nsteps, cpu_time_used);*/
        
        printf("%f,",cpu_time_used);
        if(n == 10000) printf("\n");

        /* Write the results */
        if (fname)
            write_solution(n, u, fname);
    }


    if(world_rank != 0)
    {
        for (sweep = 0; sweep < nsteps; sweep += 2) 
        {
            MPI_Recv(&offset,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
            MPI_Recv(&rows,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
            MPI_Recv(&u[offset],rows,MPI_DOUBLE,0,2,MPI_COMM_WORLD,&status);
            MPI_Recv(&f[offset],rows,MPI_DOUBLE,0,3,MPI_COMM_WORLD,&status);
            MPI_Recv(&u[offset-1],1,MPI_DOUBLE,0,4,MPI_COMM_WORLD,&status);
            MPI_Recv(&u[offset+rows],1,MPI_DOUBLE,0,5,MPI_COMM_WORLD,&status);

            


            int top = offset + rows;
            for(int i = offset;i < top;i++)
                utmp[i] = (u[i-1] + u[i+1] + h2*f[i])/2;


                        
            MPI_Send(&offset,1,MPI_INT,0,0,MPI_COMM_WORLD);
            MPI_Send(&rows,1,MPI_INT,0,1,MPI_COMM_WORLD);
            MPI_Send(&utmp[offset],rows,MPI_DOUBLE,0,2,MPI_COMM_WORLD);


            MPI_Recv(&offset,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
            MPI_Recv(&rows,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
            MPI_Recv(&utmp[offset],rows,MPI_DOUBLE,0,2,MPI_COMM_WORLD,&status);
            MPI_Recv(&f[offset],rows,MPI_DOUBLE,0,3,MPI_COMM_WORLD,&status);
            MPI_Recv(&utmp[offset-1],1,MPI_DOUBLE,0,4,MPI_COMM_WORLD,&status);
            MPI_Recv(&utmp[offset+rows],1,MPI_DOUBLE,0,5,MPI_COMM_WORLD,&status);


            top = offset + rows;
            for(int i = offset;i < top;i++)
                u[i] = (utmp[i-1] + utmp[i+1] + h2*f[i])/2;


            MPI_Send(&offset,1,MPI_INT,0,0,MPI_COMM_WORLD);
            MPI_Send(&rows,1,MPI_INT,0,1,MPI_COMM_WORLD);
            MPI_Send(&u[offset],rows,MPI_DOUBLE,0,2,MPI_COMM_WORLD);


        }

        
    }






    return 0;
}
