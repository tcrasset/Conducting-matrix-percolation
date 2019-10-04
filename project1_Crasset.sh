#!/bin/bash
# Submission script for NIC4
#SBATCH --job-name=TestRun
#SBATCH --time=00:05:00 # hh:mm:ss
#
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=2625 # megabytes
#SBATCH --partition=defq
#
#SBATCH --mail-user=tomcrasset@gmail.com
#SBATCH --mail-type=ALL
#
#SBATCH --comment=HPCProject-1
#
#SBATCH --outfile=output.txt

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

filename=$1
rm $filename

echo "Flag N d M NbThreads AvgTime Probability" >> $filename
for N in 10 50 #100 200 500 1000
do
  for M in 1 10 #100 1000 10000
  do
      for d in $(seq 0 0.02 1);
      do 
        echo "N: $N d: $d M: $M Threads:$SLURM_CPUS_PER_TASK"
        ./conducting 1 $N $d $M >> $filename
      done
  done
done
