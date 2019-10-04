for threads in 1 2 4 8 16
do
    echo "HPC_Project1 $threads output: project1_out_$threads.txt"
    sbatch --cpus-per-task=$threads project1_Crasset.sh project1_out_$threads.txt
done
