# IMPORTANT: remember to keep executable in /shared folder
mpirun -v -np 5 --hostfile ~/mpi_hosts /shared/a.out
#mpirun -v -np 5 --hostfile /deploy/mpi_hostfile /shared/hello
#mpirun -np 2 ./hello

