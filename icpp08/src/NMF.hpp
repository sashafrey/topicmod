#include <vector>
using namespace std;

double IntraSims(double** centroids,int k);

int* kmeans(double** data, int m, int n, int k,  int maxiter, int* init_flags,double* intras);

vector<int> KMEANS(double** data,int m, int n, int k,int maxiter, int tries);

void normalize(double* vec, int k);

