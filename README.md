# matmul-mpi
compile the code with command 

mpicc matmul.c -o matmul

run the codes with command

mpirun -np <NP> ./matmul <N> \"notes\"\n"

where NP is the number of cores, N is the size N of square matrix, notes is the strings you needs for example student names and their ID
