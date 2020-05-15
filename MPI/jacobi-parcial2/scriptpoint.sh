#!/bin/bash
echo "Compilando archivo fuente"
mpicc -o jacobi1d-point jacobi1d-point.c
echo "Copiando ejecutable a los worker nodes"
scp jacobi1d-point wn1:
scp jacobi1d-point wn2:
scp jacobi1d-point wn3:
scp jacobi1d-point wn4:
for j in {1..10}
do
echo iteracion $j
for i in 50 500 1000 2500 4000 5000 10000
do
echo Ejecutando con $i pasos y vectores de $i elementos
mpiexec -np 5 --host master,wn1,wn2,wn3,wn4 ./jacobi1d-point $i $i >> resultadospoint.csv
done
done