#
#	PLSA Program  by HONG Chuntao
#		2006.127	
#
#	This script runs the PLSA program on three datasets using 1,2,4,8,16 threads, and
#	dividing the co-occurrence into 4,8,16 blocks.
#
#	The datasets are available for download from http://glaros.dtc.umn.edu/gkhome/cluto/cluto/download
#
#	You are free to change the parameters for the program. But please DO NOT set the 
#	number of maximum iterations to more than 12, or the program will crash.
#

for dataset in new3 la12 cranmed; do
	echo "dataset= ${dataset}"
	for thread in 1; do
		echo "thread_num = ${thread}"
		for block in 4; do
			echo "block_num = ${block}"
			bin/PLSACluster data/${dataset}.mat 6 50 10 ${thread} ${block} 0
			echo ""
			sleep 1
		done	
	done
done
