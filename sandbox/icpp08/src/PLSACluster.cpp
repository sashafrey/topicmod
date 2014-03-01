#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <algorithm>
#include "iPLSA.hpp"
#include "PCluster.hpp"
#include <fstream>
#include <omp.h>
//#include "sampling.h"

using namespace std;

int main(int argc, char * argv[]){
//	VTPauseSampling();

	if(argc!=9){
		cout<<"usage: PLSACluster <file> <dictionary> <numClusters> <numLS> <numIters> <numThreads> <numBlocks> <pos>"<<endl;
		return 1;
	}

    int iarg = 1;
	PCluster pp(argv[iarg++]);
    iarg++; // skip dictionary;
    int numClusters=atoi(argv[iarg++]);
	int numLS=atoi(argv[iarg++]);
	int numIters=atoi(argv[iarg++]);
	int numThreads=atoi(argv[iarg++]);
	int numBlocks=atoi(argv[iarg++]);
	int pos=atoi(argv[iarg++]);
	omp_set_num_threads(numThreads);

	pp.analyze(numLS,numIters,10,numThreads,numBlocks,pos);


    iPLSA* plsa = pp.getPLSA();
    cout << "Weights of categories : " ;
    for (int i = 0; i < plsa->numCats(); i++) {
        cout << plsa->get_p_z()[i] << " ";
    }
    cout << endl;

    cout << "Top 10 words per category:\n";
    int numWords = plsa->numWords();

    std::vector<std::string> dictionary;
    ifstream matFile( argv[2] );
     // should we handle exception here?
    if ( !matFile.good() )
    {
        cerr << "Error opening dictionary file." << endl;
        matFile.close();
        return 1;
    }

    char buff[128];
    for (int iWord = 0; iWord < numWords; ++iWord) {
        matFile.getline(buff, 128);
        dictionary.push_back(buff);
    }

    int wordsToSort = 10;
    for (int i = 0; i < plsa->numCats(); i++) {
        cout << "#" << (i+1) << ": ";
        std::vector<std::pair<double, int> > p_w;
        for (int iWord = 0; iWord < numWords; iWord++) {
            p_w.push_back(std::pair<double, int>(plsa->get_p_w_z()[iWord][i], iWord));
        }

        std::sort(p_w.begin(), p_w.end());
        for (int iWord = numWords - 1; (iWord >= 0) && (iWord >= numWords - wordsToSort); iWord--) {
           cout << dictionary[p_w[iWord].second] << " ";
        }

        cout << endl;
    }
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
