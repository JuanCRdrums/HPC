#!/bin/bash
echo "Compilando archivo fuente"
mpicc -o collective collective.c
echo "Copiando ejecutable a los worker nodes"
scp collective wn1:
scp collective wn2:
scp collective wn3:
scp collective wn4:
for j in {1..10}
do
echo iteracion $j
for i in 10 100 200 500 800 1000 2000
do
echo Ejecutando con matrices de $i X $i y $i procesos
mpiexec -np 5 --host master,wn1,wn2,wn3,wn4 ./collective $i >> resultadoscollective.csv
done
done