#include<cmath>
#include<cstring>
#include<memory>
#include<iostream>
#include <vector>
#include "NMF.hpp"
using namespace std;

#include "mkl.h"

extern "C" double cblas_ddot(const int N, const double *X, const int incX,
const double *Y, const int incY);

extern "C" double cblas_dnrm2(const int N, const double *X, const int incX);

extern "C" void cblas_dscal(const int N, const double alpha, double *X, const int incX);

extern "C" void cblas_daxpy(const int N, const double alpha, const double *X, const int
incX, double *Y, const int incY);

double IntraSims(double** centroids,int k, int n)
{
	int i;
	double sum=0.0;
	double temp=0.0;
	for(i=0;i<k;i++){
		temp = cblas_dnrm2(n, centroids[i], 1);
		sum+=temp;
	}
	sum/=(double)k;
	return sum;
}

void normalize(double* vec, int k){
	double sum=0.0;
	sum = cblas_dnrm2(k, vec, 1);

	if(sum ==0)
		return;
	cblas_dscal(k, 1.0/sum, vec, 1);
}

int* kmeans(double** data, int m, int n, int k, int maxiter, int* init_flags,double* intras){
	int i,j,it;

	//Stores final results.
	int* flags=new int[m];

	//The number of docs in each cluster.
	int* cluster_count=new int[k];

	//Centroids of each cluster.
	double** centroids;
	centroids =new double*[k];

	//Allocate space for each centroid.
	for(i=0;i<k;i++)
		centroids[i]=new double[n];
	
	//Init clusters copy from init_flags.
	memcpy(flags,init_flags,m*sizeof(int));

	double max_sim;
	int max_loc;
	double sim;

	//flag indicating whether iterations have 
	//reached a convergence point.
	int convergent=0;

	for(it=0;it<maxiter;it++)
	{
		cout<<"Iteration "<<it<<endl;

		//First recompute centroids.

		//Reset centroids.
		for(i=0;i<k;i++)
		{
			cluster_count[i]=0;
			//centroids[i].SetAll(0.0);
			for(int j=0; j<n; j++){
				centroids[i][j]=0.0;
			}
		}

		for(i=0;i<m;i++)
		{
			if(flags[i]>=0)
			{
				cluster_count[flags[i]]++;
/*				for(int j=0;j<n;j++){
					centroids[flags[i]][j]+=data[i][j];
				}*/
				cblas_daxpy(n, 1.0, data[i], 1, centroids[flags[i]], 1);
			}
		}
		for(i=0;i<k;i++)
		{	
			if(cluster_count[i]!=0){
				/*for(int j=0;j<n;j++){
					centroids[i][j] /=cluster_count[i];
				}*/
				cblas_dscal(n, 1.0/cluster_count[i], centroids[i], 1);
			}
			//	centroids[i].DivideAll((double)cluster_count[i]);
		}

		*intras=IntraSims(centroids,k,n);

		for(i=0;i<k;i++)
		{	
			normalize(centroids[i], n);
			//clabs_
			//centroids[i].normalize();
		}

		//For every column vector, find the centroid with the max 
		//similarity, stored in <code>flags</code>.

		//Assume that convergent point has been reached.
		convergent=1;
		for(i=0;i<m;i++)
		{
			max_sim=0.0;
			max_loc=-1;
			for(j=0;j<k;j++)
			{
				//sim=A.CVDotProduct(i,centroids[j]);
			//	sim =0.0;
			/*	for(int l=0;l<n;l++)
					sim+=data[i][l]*centroids[j][l];*/
				sim = cblas_ddot( n, data[i], 1, centroids[j], 1);

				if(sim>max_sim)
				{
					max_sim=sim;
					max_loc=j;
					//cout<<"doc i from 
				}
			}
			if(flags[i]!=max_loc)
			{
				flags[i]=max_loc;
				convergent=0;
			}
		}
		if(convergent==1)
		{
			cout<<"Convergent in k-means, exit"<<endl;
			break;
		}
	}

	delete[] cluster_count;
	for(i=0;i<k;i++)
		delete[] centroids[i];
	delete[] centroids;

	return flags;
}


//from <= rand < to
int GenRand(int from,int to)
{
	int r=rand();
	return r%(to-from)+from;
}

int* RandomPoints(int count, int from, int to)
{
	int* rp=new int[count];
	int i,j;
	int dup;
	int r;
	for(i=0;i<count;i++)
	{
		dup=1;
		while(dup==1)
		{
			dup=0;
			r=GenRand(from,to);
			for(j=0;j<i;j++)
			{
				if(r==rp[j])
				{
					dup=1;
					break;
				}
			}
			if(dup==0)
			{
				rp[i]=r;
			}
		}
	}
	return rp;
}

vector<int> KMEANS(double** data,int m, int n, int k,int maxiter, int tries)
{
	int** results=new int*[tries];
	double* intras=new double[tries];
	
	int i,j;
	int *init_points;
	int *init_flags=new int[m];

	for(i=0;i<tries;i++)
	{
		for(j=0;j<m;j++)
			init_flags[j]=-1;

		init_points=RandomPoints(k,0,m);
		for(j=0;j<k;j++)
			init_flags[init_points[j]]=j;

		cout<<"Tries "<<i<<endl;
		results[i]=kmeans(data,m,n,k,maxiter,init_flags,&intras[i]);
		cout<<"Intra:"<<intras[i]<<endl;
		delete[] init_points;
	}
	
	double max_sim=0.0;
	int max_loc=-1;
	for(i=0;i<tries;i++)
	{
		if(intras[i]>max_sim)
		{
			max_sim=intras[i];
			max_loc=i;
		}
	}

	vector<int> result;
	for(int i = 0; i<m ; i++){
		result.push_back(results[max_loc][i]);
	}

	for(i=0;i<tries;i++)
		delete[] results[i];
	delete[] results;
	delete[] intras;
	delete[] init_flags;

	return result;
}
