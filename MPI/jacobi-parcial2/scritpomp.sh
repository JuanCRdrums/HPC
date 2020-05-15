#!/bin/bash
echo "Compilando archivo fuente"
gcc -o jacobi1d-omp jacobi1d-omp.c -fopenmp
for j in {1..10}
do
echo iteracion $j
for i in 50 500 1000 2500 4000 5000 10000
do
echo Ejecutando con $i pasos y vectores de $i elementos
./jacobi1d-omp $i $i >> resultadosomp.csv
done
done