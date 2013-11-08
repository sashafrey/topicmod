#include "PCluster.hpp"
#include "NMF.hpp"

#include "time.h"

PCluster::PCluster(const string & matFileName):_matFileName(matFileName), _clustered(false), _numClusters(0), 
		_analyzed(false), plsa(NULL){

}

PCluster::~PCluster(){
	if(_analyzed)
		delete plsa;
}

void PCluster::analyze(int numCats, int numIters, int testPercentage, int numThreads,int numBlocks, int pos){
	if(_analyzed)
		delete plsa;

	plsa=new iPLSA(_matFileName, testPercentage, numCats, numIters, 1, 1, 0.6, 0, 0.92, numThreads, numBlocks, pos);

//double start,end;
//start=get_time();

	plsa->run();

//end=get_time();
//cout<<"time used:"<<end-start<<endl;

	_analyzed=true;
}

void PCluster::doCluster(int numCluster,int maxIter,int tries){
	_numClusters=numCluster;
	if(!_analyzed)
		analyze(defaultNumCats,defaultNumIters,defaultTestPercentage,1,1, 0);
	clusterResult=KMEANS(plsa->get_p_d_z(),plsa->numDocs(),plsa->numCats(),numCluster,maxIter,tries);
	_clustered=true;
}

