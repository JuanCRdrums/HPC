#!/bin/bash
echo "Compilando archivo fuente"
mpicc -fopenmp jacobi1d-hybrid.c -o jacobi1d-hybrid
echo "Copiando ejecutable a los worker nodes"
scp jacobi1d-hybrid wn1:
scp jacobi1d-hybrid wn2:
scp jacobi1d-hybrid wn3:
scp jacobi1d-hybrid wn4:
for j in {1..10}
do
echo iteracion $j
for i in 500000 5000000 10000000 25000000 40000000 50000000 100000000
do
echo Ejecutando con 10 pasos y vectores de $i elementos
mpiexec -np 5 --host master,wn1,wn2,wn3,wn4 ./jacobi1d-hybrid $i 10 >> resultadoshybrid.csv
done
done