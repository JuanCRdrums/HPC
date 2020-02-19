#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define n 5
#define NUM_THREADS	n*n

struct args
{
    int* fst;
    int* sec;
    int* mult;
    int i;
    int j;
};

void *valueCell(void *arguments)
{
  //printf("hola");
  //printf("%d \n",((struct args*)arguments)->fst[24]);
  for(int i = 0; i < n; i++)
  {
    
  }
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int fst[n][n] = {{1,2,3,4,5},
                    {11,12,13,14,15},
                    {21,22,23,24,25},
                    {31,32,33,34,35},
                    {41,42,43,44,45}};

    int sec[n][n] = {{1,2,3,4,5},
                    {6,7,8,9,10},
                    {11,12,13,14,15},
                    {16,17,18,19,20},
                    {21,22,23,24,25},};

    int mult[n][n];
   pthread_t threads[NUM_THREADS];
   int rc;
   long t = 0;
   for(int i = 0; i < n; i++)
   {
       for(int j = 0; j < n; j++)
       {
           struct args *arguments = (struct args *)malloc(sizeof(struct args));;
           arguments->fst = fst;
           arguments->sec = sec;
           arguments->mult = mult;
           //printf("%d \n",arguments->fst[0][0]);
           //printf("hola %d %d\n",i,j);
           pthread_create(&threads[t], NULL, valueCell,(void * )arguments);
           t++;
       }
   }
   /*for(t=0;t<NUM_THREADS;t++){
     printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, valueCell, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
     }*/

   /* Last thing that main() should do */
   pthread_exit(NULL);
}