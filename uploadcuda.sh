#!/bin/bash
#SBATCH --job-name=congmingproj
#SBATCH --nodes=1
#SBATCH --gres=gpu:1
#SBATCH --time=00:05:00
#SBATCH --account=eecs587f23_class
#SBATCH --partition=gpu


#make sure to load the cuda module before running
#module load cuda
#make sure to compile your program using nvcc
#nvcc -o example1 example1.cu


# ./minesweeper -h height -w width -n numMines -m 0,1,2,3 (seq, cuda, cuda w shared mem, openmp) -p (print) -t (testing) 
./minesweeper -h 100 -w 100 -n 20 -m 1  

