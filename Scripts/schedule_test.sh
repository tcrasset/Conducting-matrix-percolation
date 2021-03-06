#!/bin/bash
# Submission script for NIC4
#SBATCH --job-name=HPC_Project1
#SBATCH --time=00:05:00 # hh:mm:ss
#
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=4000 # megabytes
#SBATCH --partition=defq
#
#SBATCH --mail-user=tomcrasset@gmail.com
#SBATCH --mail-type=FAIL
#
#SBATCH --comment=HPCProject-1
#

filename=$1
schedule_type=$2
chunk_size=$3

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_SCHEDULE="$schedule_type,$chunk_size"

for d in $(seq 0 0.1 1);
    do
        ./conducting 1 100 $d 1000 >> $filename
done
