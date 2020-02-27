#!/bin/bash
echo "Compilando archivo fuente"
gcc -o secuencial secuencial.c
for j in {1..10}
do
echo iteracion $j
for i in 10 100 200 500 800 1000 2000
do
echo Ejecutando con matrices de $i X $i
./secuencial $i >> resultadosecuencial.csv
done
done
