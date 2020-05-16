#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "time.h"

void write_vector(double *u,int offset,int n)
{
    for(int i = offset; i < n+offset; i++)
        printf("%f ",u[i]);
    printf("\n");
}

void process(int offset, int n,double* u, double* utmp,double* f,double h2)
{
    for(int i = offset; i < offset + n; i++)
        u[i] = (utmp[i-1] + utmp[i+1] + h2*f[i])/2;
}


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

    int averow = (n-1) / 4;
    int extra = (n-1) % 4;
    int offset = 1;
    int rows;
    int offsets[4],rowsvector[4];
    for(int i = 0; i < 4; i++)
    {
        int rows = (i < extra) ? averow + 1 : averow;
        offsets[i] = offset;
        offset = offset + rows;
        rowsvector[i] = rows;
    }

    for (sweep = 0; sweep < nsweeps; sweep += 2) {
        
        /* Old data in u; new data in utmp */
        #pragma omp parallel for schedule(dynamic,1)
        for (i = 0; i < 4; i++)
        {
            process(offsets[i],rowsvector[i],utmp,u,f,h2);
        }
        #pragma omp barrier

        /* Old data in utmp; new data in u */
        #pragma omp parallel for schedule(dynamic,1)
        for (i = 0; i < 4; i++)
        {
            process(offsets[i],rowsvector[i],u,utmp,f,h2);
        }
        #pragma omp barrier

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


int main(int argc, char** argv)
{
    int i;
    int n, nsteps;
    double* u;
    double* f;
    double h;
    double tstart, tend;
    char* fname;

    /* Process arguments */
    n      = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    fname  = (argc > 3) ? argv[3] : NULL;
    h      = 1.0/n;

    /* Allocate and initialize arrays */
    u = (double*) malloc( (n+1) * sizeof(double) );
    f = (double*) malloc( (n+1) * sizeof(double) );
    memset(u, 0, (n+1) * sizeof(double));
    for (i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Run the solver */
    tstart = omp_get_wtime();
    jacobi(nsteps, n, u, f);
    tend = omp_get_wtime();

    double cpu_time_used = tend - tstart;

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

    free(f);
    free(u);
    return 0;
}
