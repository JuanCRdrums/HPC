#!/bin/bash
echo "Compilando archivo fuente"
gcc -o jacobi1d-serial jacobi1d-serial.c
for j in {1..10}
do
echo iteracion $j
for i in 50 500 1000 2500 4000 5000 10000
do
echo Ejecutando con $i pasos y vectores de $i elementos
./jacobi1d-serial $i $i >> resultadosecuencial.csv
done
done
