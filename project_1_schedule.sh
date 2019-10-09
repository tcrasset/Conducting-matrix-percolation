
#!/bin/bash
gcc -fopenmp -std=c99 -lm project1_Crasset.c -o conducting
mkdir schedule_results
for threads in 1 2 4 8
do
    for kind in static dynamic guided auto
    do
        #Taking M = 1000
        for chunk in $((1000/$threads)) $((512/$threads)) $((256/$threads)) $((128/$threads)) 8 4 2 1
        do
            filename="schedule_results/project1_out_$threads-$kind-$chunk.txt"
            rm $filename
            echo "Flag N d M NbThreads AvgTime Schedule_kind Chunk_size Probability" >> $filename
            echo "Threads: $threads Schedule: '$kind, $chunk' output: $filename"
            sbatch --cpus-per-task=$threads schedule_test.sh $filename $kind $chunk
        done
    done
done