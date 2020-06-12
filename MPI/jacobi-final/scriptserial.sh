#!/bin/bash
echo "Compilando archivo fuente"
gcc -o jacobi1d-serial jacobi1d-serial.c
for j in {1..10}
do
echo iteracion $j
for i in 500000 5000000 10000000 25000000 40000000 50000000 100000000
do
echo Ejecutando con 10 pasos y vectores de $i elementos
./jacobi1d-serial $i 10 >> resultadosecuencial.csv
done
done
