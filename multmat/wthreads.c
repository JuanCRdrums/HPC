#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define n 2000
#define NUM_THREADS	n

int mult[n][n];

struct args
{
    int* fst;
    int* sec;
    int i;
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
  int* fst = ((struct args*)arguments)->fst;
  int* sec = ((struct args*)arguments)->sec;
  for(int j = 0; j < n; j++)
  {
    int r = 0;
    for(int k = 0; k < n; k++)
      r += fst[i*n+k] * sec[k*n+j];
    mult[i][j] = r;
    //printf('%d \n',mult[i][j]);
  }
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int *fst = (int *)malloc(n * n * sizeof(int));
  genMatrix(fst);
  
  int *sec = (int *)malloc(n * n * sizeof(int));
  genMatrix(sec);


   pthread_t threads[NUM_THREADS];
   int rc;
   long t = 0;
   clock_t start,end;
  start = clock();
   for(int i = 0; i < n; i++)
   {
      struct args *arguments = (struct args *)malloc(sizeof(struct args));;
      arguments->fst = fst;
      arguments->sec = sec;
      arguments->i = i;
      pthread_create(&threads[t], NULL, valueCell,(void * )arguments);
      t++;
   }
   end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("N value = %d \n\n",n);
    printf("Time used: %f \n",cpu_time_used);
    /*printMat(fst);
    printMat(sec);
    printMat(mult);*/
   pthread_exit(NULL);
}