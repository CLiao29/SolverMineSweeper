#!/bin/bash
# (See https://arc-ts.umich.edu/greatlakes/user-guide/ for command details)

# Set up batch job settings
#SBATCH --job-name=congmingupload
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=36
#SBATCH --exclusive
#SBATCH --time=00:05:00
#SBATCH --account=eecs587f23_class
#SBATCH --partition=standard


export OMP_NUM_THREADS=36
./minesweeper -h 1000 -w 1000 -n 100 -m 1 -p 
# { time ./a3 > aaa_9.stdout ; } 2> a3_9_time.txt
