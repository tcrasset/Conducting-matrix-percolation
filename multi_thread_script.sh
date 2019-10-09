#!/bin/bash
gcc -fopenmp -std=c99 -lm project1_Crasset.c -o conducting
for threads in 1 2 4 8
do
    echo "HPC_Project1 $threads output: project1_out_$threads.txt"
    sbatch --cpus-per-task=$threads project1_Crasset.sh \
project1_out_$threads.txt
done
