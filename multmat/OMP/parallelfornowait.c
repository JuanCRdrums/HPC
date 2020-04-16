#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int **fst,**sec,**mult;

double ** allocate_matrix( int size )
{
  /* Allocate 'size' * 'size' doubles contiguously. */
  double * vals = (double *) malloc( size * size * sizeof(double) );

  /* Allocate array of double* with size 'size' */
  double ** ptrs = (double **) malloc( size * sizeof(double*) );

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
    printf(" The result of matrix multiplication or product of the matrices is: \n "); 
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
        printf("%d \t", mat[i][j]);
      printf(" \n ");
    }
}

void multmat(int n)
{
    int tot;
    #pragma omp parallel
    #pragma omp for nowait
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++) {
          tot = tot + fst[i][k] * sec[k][j];
        }
        mult[i][j] = tot;
        tot = 0;
      }
    }
}

int main(int argc, char const *argv[])
{
    int n;
    sscanf (argv[1],"%d",&n);

    fst = allocate_matrix(n);
    sec = allocate_matrix(n);
    mult = allocate_matrix(n);

    genMatrix(fst,n);
    genMatrix(sec,n);



    double start,end;
    start = omp_get_wtime();
    multmat(n);
    end = omp_get_wtime();

    /*printMat(fst,n);
    printMat(sec,n);
    printMat(mult,n);*/

    double cpu_time_used = ((double) (end - start));
    printf("%f,",cpu_time_used);
    if(n == 2000) printf("\n");
    return 0;
}
