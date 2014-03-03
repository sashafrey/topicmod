1. Files and Directories

	|-- Makefile (makefile of the project)
	|
	|-- bin	(directory where the binary files are placed)
	|
	|-- data (directory containing datasets)
	|
	|-- README.txt (the file you are currently reading)
	|
	|-- run.sh (script for running the PLSA program)
	|
	`-- src (source directory)


2. Compiling the Program

To compile the program, you will need to do the fowllowing:	
	a. install a C++ compiler with OpenMP support such as Intel C++ Compiler,
	   and the MKL library.
	b. modify src/Makefile so that it fits the configuration of your system.
	c. run "make" in the directory where this file exists.

If all configurations are set right, you will get an executable named
PLSACluster and some .o files in the bin directory.


3. Running the Program

To run the program, change your working directory to the directory
where run.sh exists. And run "./run.sh".

You will get some output as:
	dataset= new3
	thread_num = 1
	block_num = 4
	time used:14.2374

	block_num = 8
	time used:9.89454
	...

You can change the parameters by modifying the file run.sh, or you can
run the program using command line. The command format is:

PLSACluster [datasets] [numClusters] [sizeZ] [numIters] [numThreads]
 [numBlocks] [pos]

	datasets
	 the dataset used, should be in the same format as those mat files 
	 provided by Cluto

	numClusters
	 how many clusters the dataset will be divided into
	 Note that we commented out the clustering code, so this parameter 
	 is of no use, unless you uncomment the clustering code

	sizeZ
	 number of latent semantic variables used in PLSA

	numIters
	 maximum number of iterations. If the algorithm doesn't convergent
	 after this amount of iterations, the program will be terminated.
	 DO NOT set this to more than 12, or the program may crash.

	numThreads
	 number of threads to run the program
	 The OMP_NUM_THREADS enviroment variable is ignored.

	numBlocks
	 how many blocks will the matrix be divided into.
	 Actually the matrix will be divided into numBlocks*numBlocks.

	pos
	 the CPU affinity of the threads. 
	 If pos=0, the nth thread are attached to CPU number n.
	 If pos=1, the nth thread are attached to CPU number n*16/numThreads

An example would be:

	# PLSACluster new3.mat 6 50 10 4 16 0


4. Input and Output
The sample input files are placed in the data directory. The input files are 
formated so that:
    The first line contains three numbers: the number of documents, number
      of words in dictionary, and the number of non-zero elements in the 
      word-document matrix.
    The ith line of the file stands for the vector representation of document 
      i-1, stored in compact row format, i.e. the column number N followed by 
      the value in row i-1, column N.
We don't save the output of the algorithm to files. If you need to dump the 
result, modify the file PLSACluster and dump the content of the matrix you 
want.
