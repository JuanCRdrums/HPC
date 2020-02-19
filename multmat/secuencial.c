#include <stdio.h>

#define n 5

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

    multmat(fst,sec,mult);
    printMat(mult);
    
    return 0;
}
