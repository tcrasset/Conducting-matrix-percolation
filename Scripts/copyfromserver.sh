# ssh-add /home/tom/.ssh/id_rsa.ceci
# scp -i /home/tom/.ssh/id_rsa.ceci tcrasset@nic4.segi.ulg.ac.be:/home/ulg/info0939/tcrasset/project1_out_bis_1.txt /home/tom/Documents/Uliege/Master2/HPC/Project1/Results
# scp -i /home/tom/.ssh/id_rsa.ceci tcrasset@nic4.segi.ulg.ac.be:/home/ulg/info0939/tcrasset/project1_out_bis_2.txt /home/tom/Documents/Uliege/Master2/HPC/Project1/Results
# scp -i /home/tom/.ssh/id_rsa.ceci tcrasset@nic4.segi.ulg.ac.be:/home/ulg/info0939/tcrasset/project1_out_bis_4.txt /home/tom/Documents/Uliege/Master2/HPC/Project1/Results
# scp -i /home/tom/.ssh/id_rsa.ceci tcrasset@nic4.segi.ulg.ac.be:/home/ulg/info0939/tcrasset/project1_out_bis_8.txt /home/tom/Documents/Uliege/Master2/HPC/Project1/Results
# scp -r -i /home/tom/.ssh/id_rsa.ceci tcrasset@nic4.segi.ulg.ac.be:/home/ulg/info0939/tcrasset/schedule_results /home/tom/Documents/Uliege/Master2/HPC/Project1/Results

# cd Results/schedule_results
# rm merged.txt
# cat *.txt > merged.txt
# grep -v "Flag N d M NbThreads AvgTime Schedule_kind Chunk_size Probability" merged.txt > merged_temp.txt
# echo "Flag N d M NbThreads AvgTime Schedule_kind Chunk_size Probability" | cat - merged_temp.txt > temp && mv temp merged.txt

# rm project1_out_*.txt
# rm merged_temp.txt

mkdir Results/new_schedule_results
scp -r -i /home/tom/.ssh/id_rsa.ceci tcrasset@nic4.segi.ulg.ac.be:/home/ulg/info0939/tcrasset/schedule_results/project1_out_*.txt /home/tom/Documents/Uliege/Master2/HPC/Project1/Results/new_schedule_results
cd Results/new_schedule_results
cat *.txt > merged_bis.txt
grep -v "Flag N d M NbThreads AvgTime Schedule_kind Chunk_size Probability" merged_bis.txt > merged_temp.txt
echo "Flag N d M NbThreads AvgTime Schedule_kind Chunk_size Probability" | cat - merged_temp.txt > temp && mv temp merged_bis.txt
rm project1_out_*.txt
rm merged_temp.txt
