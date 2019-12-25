# Conducting-matrix-percolation


gcc -lm -fopenmp -std=c99 -o conducting main.c

./conducting 0 50 0.4

OMP_NUM_THREADS=2 ./conducting 1 50 0.4 10
