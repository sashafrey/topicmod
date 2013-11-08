#include <iostream>
#include <stdlib.h>
#include <string>
#include "iPLSA.hpp"
#include "PCluster.hpp"
#include <fstream>
#include <omp.h>
//#include "sampling.h"

using namespace std;

int main(int argc, char * argv[]){
//	VTPauseSampling();

	if(argc!=8){
		cout<<"usage: PLSACluster <file> <numClusters> <numLS> <numIters> <numThreads> <numBlocks> <pos>"<<endl;
		return 1;
	}

	PCluster pp(argv[1]);

	int numClusters=atoi(argv[2]);
	int numLS=atoi(argv[3]);
	int numIters=atoi(argv[4]);
	int numThreads=atoi(argv[5]);
	int numBlocks=atoi(argv[6]);
	int pos=atoi(argv[7]);
	omp_set_num_threads(numThreads);

	pp.analyze(numLS,numIters,10,numThreads,numBlocks,pos);
/*
	pp.doCluster(numClusters,5,3);
	Cluster c=pp.result();
	
	ofstream out("cresult.txt");	

	for(Cluster::iterator it=c.begin();it!=c.end();it++)
		out<<*it<<endl;

	out.close();
*/
	return 0;
}
