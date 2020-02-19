#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define n 100
#define NUM_THREADS	n*n

int result[n][n];

struct args
{
    int* fst;
    int* sec;
    int r;
    int i;
    int j;
};

void genMatrix(int mat[n][n])
{
  for(int i = 0; i < n; i++)
  {
    for(int j = 0; j < n; j++)
      mat[i][j] = rand() % n+1;
  }
}

void printMat(int mat[n][n])
{
    printf(" The result of matrix multiplication or product of the matrices is: \n "); 
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
        printf("%d \t", mat[i][j] );
      printf(" \n ");
    }
}

void *valueCell(void *arguments)
{
  //printf("hola");
  //printf("%d \n",((struct args*)arguments)->fst[24]);
  int i = ((struct args*)arguments)->i;
  int j = ((struct args*)arguments)->j;
  int* fst = ((struct args*)arguments)->fst;
  int* sec = ((struct args*)arguments)->sec;
  int* mult = ((struct args*)arguments)->sec;
  int  r = ((struct args*)arguments)->r;
  for(int k = 0; k < n; k++)
  {
    r += fst[i*n+k] * sec[k*n+j];
  }
  result[i][j] = r;
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int *fst = (int *)malloc(n * n * sizeof(int));
  genMatrix(fst);
  
  int *sec = (int *)malloc(n * n * sizeof(int));
  genMatrix(sec);

  int *mult = (int *)malloc(n * n * sizeof(int));
   pthread_t threads[NUM_THREADS];
   int rc;
   long t = 0;
   clock_t start,end;
  start = clock();
   for(int i = 0; i < n; i++)
   {
       for(int j = 0; j < n; j++)
       {
           struct args *arguments = (struct args *)malloc(sizeof(struct args));;
           arguments->fst = fst;
           arguments->sec = sec;
           arguments->i = i;
           arguments->j = j;
           arguments->r = 0;
           pthread_create(&threads[t], NULL, valueCell,(void * )arguments);
           t++;
       }
   }
   end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used: %f \n",cpu_time_used);
   //printMat(result);
   pthread_exit(NULL);
}