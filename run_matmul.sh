#!/bin/bash
#SBATCH -o out_matmul.txt
#SBATCH -p batch
#SBATCH -N 2
#SBATCH --nodelist=node-06,node-08

INFO="my name and my IDs number"


mpirun --mca btl_tcp_if_exclude docker0,lo -np 8 ./matmul  512 "$INFO"
mpirun --mca btl_tcp_if_exclude docker0,lo -np 16 ./matmul  512 "$INFO"
mpirun --mca btl_tcp_if_exclude docker0,lo -np 8 ./matmul  1024 "$INFO"
mpirun --mca btl_tcp_if_exclude docker0,lo -np 16 ./matmul  1024 "$INFO"

