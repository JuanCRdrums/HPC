#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n 2000

void genMatrix(int mat[n][n])
{
  for(int i = 0; i < n; i++)
  {
    for(int j = 0; j < n; j++)
      mat[i][j] = rand() % n+1;
  }
}

void printMat(int mul[n][n])
{
    printf(" The result of matrix multiplication or product of the matrices is: \n "); 
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
        printf("%d \t", mul[i][j] );
      printf(" \n ");
    }
}

void multmat(int fst[n][n],int sec[n][n],int mul[n][n])
{
    int tot;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++) {
          tot = tot + fst[i][k] * sec[k][j];
        }
        mul[i][j] = tot;
        tot = 0;
      }
    }
}

int main(int argc, char const *argv[])
{
    int *fst = (int *)malloc(n * n * sizeof(int));
    genMatrix(fst);
    
    int *sec = (int *)malloc(n * n * sizeof(int));
    genMatrix(sec);

    int *mult = (int *)malloc(n * n * sizeof(int));
    clock_t start,end;
    start = clock();
    multmat(fst,sec,mult);
    end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("N value = %d \n\n",n);
    printf("Time used: %f \n",cpu_time_used);
    /*printMat(fst);
    printMat(sec);
    printMat(mult);*/
    return 0;
}
