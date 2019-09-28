#!/bin/bash
filename="output3.csv"
rm $filename
M=1000

gcc -fopenmp -std=c99 -Wall project1_Crasset.c -o conducting
echo "Flag N d M NbThreads AvgTime Probability" >> $filename

for N in 10 50 100 200 #500 1000 2000 5000 10000 20000
do
    for d in $(seq 0 0.01 1);
    do 
      echo "N: $N d: $d"
      ./conducting 1 $N $d $M >> $filename
    done
done