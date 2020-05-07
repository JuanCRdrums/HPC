#!/bin/bash
echo "Compilando archivo fuente"
mpicc -o point-to-point point-to-point.c
echo "Copiando ejecutable a los worker nodes"
scp point-to-point wn1:
scp point-to-point wn2:
scp point-to-point wn3:
scp point-to-point wn4:
for j in {1..10}
do
echo iteracion $j
for i in 10 100 200 500 800 1000 2000
do
echo Ejecutando con matrices de $i X $i y $i procesos
mpiexec -np 5 --host master,wn1,wn2,wn3,wn4 ./point-to-point $i >> resultadospoint.csv
done
done