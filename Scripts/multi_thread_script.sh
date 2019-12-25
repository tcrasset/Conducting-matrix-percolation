#!/bin/bash
gcc -fopenmp -std=c99 -lm project1_Crasset.c -o conducting
for threads in 1 2 4 8
do
    filename=project1_out_bis_$threads.txt
    echo "HPC_Project1 $threads output: $filename"
    sbatch --cpus-per-task=$threads project1_Crasset.sh $filename
done
