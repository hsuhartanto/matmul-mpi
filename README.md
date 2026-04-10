# matmul-mpi
compile the code with command 

mpicc matmul.c -o matmul

when there is no error, you will get an executable file named "matmul", then 
run the code  with a command

mpirun -np <NP> ./matmul <N> \"notes\"\n"

where NP is the number of cores, N is the size N of square matrix, notes is the strings you needs for example student names and their ID,  in a cluster environment, NP is should not exceed the number of cores in a node,

use a batch file run_matmul.sh with a command

sbatch run_matmul.sh

to submit the jobs in a cluster environment, particularly when NP is larger than the number of cores in a node;

to see the submitted jobs status, use a command 

squeue-1

 
