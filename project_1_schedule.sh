export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK



for threads in 1 2 4 8
do
    for schedule_type in static dynamic guided auto
    do
        #Taking M = 1000
        for chunk_size in $((1000/$threads)) $((512/$threads)) $((256/$threads)) $((128/$threads)) 8 4 2 1
        do
            filename="project1_out_schedule.txt"
            echo "Threads: $threads Schedule: '$schedule_type, $chunk_size' output: $filename"
            sbatch --cpus-per-task=$threads schedule_test.sh $filename $schedule_type $chunk_size
        done
    done
done