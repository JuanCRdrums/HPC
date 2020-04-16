#!/bin/bash
echo "Compilando archivo fuente"
gcc -o parallelrandomaccess parallelrandomaccess.c -fopenmp
for j in {1..10}
do
echo iteracion $j
for i in 10 100 200 500 800 1000 2000
do
echo Ejecutando con matrices de $i X $i
./parallelrandomaccess $i >> resultadoparallelrandomaccess.csv
done
done
