#!/bin/bash
#SBATCH -o out_matmul.txt
#SBATCH -p batch
#SBATCH -N 2
#SBATCH --nodelist=node-06,node-08

PROG=/home/user01/matmul
INFO="my name and my IDs number"


mpirun --mca btl_tcp_if_exclude docker0,lo -np 8 $PROG 512 "$INFO"
mpirun --mca btl_tcp_if_exclude docker0,lo -np 16 $PROG 512 "$INFO"
mpirun --mca btl_tcp_if_exclude docker0,lo -np 8 $PROG 1024 "$INFO"
mpirun --mca btl_tcp_if_exclude docker0,lo -np 16 $PROG 1024 "$INFO"

