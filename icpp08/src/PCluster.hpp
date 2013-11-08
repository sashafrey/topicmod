#ifndef _PCLUSTER_HPP
#define _PCLUSTER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "iPLSA.hpp"

using namespace std;

typedef vector<int> Cluster;

class PCluster{
public:
	PCluster(const string & matFileName);
	~PCluster();
	
	void analyze(int numCats, int numIters, int testPercentage, int numThreads, int numBlocks, int pos);

	void doCluster(int numCluster,int maxIter,int tries);

	void writeResult();

	inline bool analyzed(){return _analyzed;}
	inline bool clustered(){return _clustered;}

	inline int numClusters(){return _numClusters;}
	inline Cluster result(){return clusterResult;}

	inline iPLSA * getPLSA(){return plsa;}

	inline int numCats() const{
		if(!_analyzed)
			return 0;
		return plsa->numCats();
	}
	inline int numDocs() const{
		if(!_analyzed)
			return 0;
		return plsa->numDocs()-1;
	}
	inline int numWords() const{
		if(!_analyzed)
			return 0;
		return plsa->numWords()-1;
	}

	/// consts
	static const int defaultNumCats=20;
	static const int defaultNumIters=20;
	static const int defaultTestPercentage=10;
private:
	// variables

	/// the matrix file name
	const string _matFileName;

	/// is clustering performed
	bool _clustered;
	/// num of clusters
	int _numClusters;
	/// clustering result
	Cluster clusterResult;

	/// is PLSA performed
	bool _analyzed;
	/// the plsa object
	iPLSA * plsa;
};

#endif	// _PCLUSTER_HPP

