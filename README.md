# Conducting-matrix-percolation

Below is just a small exerpt of the full statement [Statement.pdf](Statement.pdf).

>In this project, you are asked to study the influence of the density of conducting fibers in the
>non-conducting matrix on the electrical conductivity of the resulting composite material
>using a simple percolation model describe below. This algorithm must be parallelized with
>the OpenMP library.

In the `Scripts/` directory are multiple bash scripts and Slurm scripts to run the program on a cluster. 

A full analysis can be seen in the Report [Report.pdf](Report.pdf).

## Getting Started

This can be run on a local machine or on a cluster using a Slurm script.

### Prerequisites

Make sure that you have gcc installed using the command below:
```
sudo apt-get install gcc
```
OpenMP comes with the gcc compiler.

This project was compiled and run using `gcc (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0`.
If you want to make sure it works, I invite you to install the same one.
You can check your version with 
```
gcc --version
```

### Compilation

#### Locally
To compile the program and run it locally (on your machine), run the following commands in the terminal.

```
gcc -lm -fopenmp -std=c99 -o conducting main.c
```
and run the program using the following templates:

Single-threaded environment
```
./conducting FLAG N d [M]
```

Multi-threaded environment
```
OMP_NUM_THREADS=NB_OF_THREADS ./conducting FLAG N d [M]
```

Replace `NB_OF_THREADS` with the number of threads you wish to run it with, `FLAG` equal to 0 for the intermediate deadline or 1 for the final deadline, the grid size `N` , the fiber density `d`, and the number of monte carlo simulations `M`.

If `FLAG == 1`, you can (and must) specify the number of montecarlo simulations M.
If `FLAG == 0`, you only do one run.


For example,

running the program on a single thread using the intermediate version with a grid of size 50, a fiber density of 0.4 :
```
./conducting  0 50 0.4
```

running the program with two threads, using the final deadline version with a grid of size 50, a fiber density of 0.4 and 10 simulations :
```
OMP_NUM_THREADS=2 ./conducting  1 50 0.4 10
```

#### On a cluster with Slurm

If you want to fully use the capabilities of the program, we suggest you run this code on a cluster managed by the Slurm Workload Manager.

Sample scripts are provided in the `Scripts/` directory.

## Authors
* **Tom Crasset ** - *Implementation of the problem, scalability and stability analysis*

* **Anthony Royer ** - *Creation of statement*

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

Thanks to Pr. C. Geuzaine for his course of High Perfomance Scientific Computing at the University of Liège as well as his assistant, Anthony Royer.
Moreover, we would like to thank the CECI(Consortium des Équipements de Calcul Intensif) for the cluster they provided.
