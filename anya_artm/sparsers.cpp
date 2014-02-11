#include "stdafx.h"
#include "types.h"


int cmpUp (const void * elem1, const void * elem2) {
	return *(double *)elem1 - *(double *)elem2 < 0? -1 :
		   *(double *)elem1 - *(double *)elem2 > 0? +1 : 0; 
}

int cmpDown (const void * elem1, const void * elem2) {
	return *(double *)elem1 - *(double *)elem2 < 0? +1 :
		   *(double *)elem1 - *(double *)elem2 > 0? -1 : 0; 
}

// ��������� ���������� ��������, ������ � ����� ������������ ���� (������� ����������� �� ����)
double sparse_tail(double sumRatio, double maxRatio, long double** toZeroize, long double* toReduce, int LEN, int NUM) {
    double averageLevel = 0;
	// ����� �������� ���������� �������� �����, ���������� � ����� ������������ ����
	double* arr = new double [LEN];
	for (int d = 0; d < NUM; ++d) {
		long double myRealSum = 0;
		int i = 0;
		for (int t = 0; t < LEN; ++t) {
			if (toZeroize[t][d] > 0) {
				arr[i++] = toZeroize[t][d];
				myRealSum += toZeroize[t][d];
			}
		}
		qsort(arr, i, sizeof(double), cmpUp); // ��������������� ������ ��������� ���������

		int boarder = 0;
		for (double sum = 0; boarder < i && sum < sumRatio * myRealSum && boarder < maxRatio * LEN; ++boarder) {
			sum += arr[boarder];
		}

		boarder -= 2; // ���������� �� ���������, �� � ���������-�� ��� ��������
		if (boarder >= 0) {
			double level = arr[boarder];
            averageLevel += level;
			for (int t = 0; t < LEN; ++t) {
				if (toZeroize[t][d] <= level) { 
					toReduce[d] -= toZeroize[t][d];
					assert(toReduce[d] > - MP);
					if (!(toReduce[d] >= 0)) {
						toReduce[d] = 0;
					}

					toZeroize[t][d] = 0;
				}
			}
		}
	}
    averageLevel /= NUM;
    return averageLevel;
	delete [] arr;
}

// ��������� ������������ ���� ���������� ��������
double sparse_raws_old(double ratio, long double** toZeroize, long double* toReduce, int LEN, int NUM) {
    double averageLevel = 0;
	// ����� �������� ������������ ���� ��������� �����
	double* arr = new double [LEN];
	for (int d = 0; d < NUM; ++d) {
		int i = 0;
		for (int t = 0; t < LEN; ++t) {
			if (toZeroize[t][d] > 0) {
				arr[i++] = toZeroize[t][d];
			}
		}
		qsort(arr, i, sizeof(double), cmpUp);
		int boarder = int(ratio * i) - 1; // ������ ��������. ������� ���, ��� <= ��� �����������.
		if (boarder >= 0) { // ����� ��� ������ ��������. ��� �������� �� �������� �� �����.
			double level = arr[boarder];
            averageLevel += level;
			for (int t = 0; t < LEN; ++t) {
				if (toZeroize[t][d] <= level) {
					toReduce[d] -= toZeroize[t][d];
					assert(toReduce[d] > -MP);
					if (toReduce[d] < 0)
						toReduce[d] = 0;
					toZeroize[t][d] = 0;
				}
			}
		}
	}
	delete [] arr;
    averageLevel /= NUM;
    return averageLevel;
}


// ��������� ������������ ���� ���������� ��������
double sparse_raws(double ratio, double* distribution, long double** toZeroize, long double* toReduce, int LEN, int NUM) {
    double averageLevel = 0;
	// ����� �������� ������������ ���� ��������� �����
	double* arr = new double [LEN];
	for (int d = 0; d < NUM; ++d) {
		int i = 0;
		for (int t = 0; t < LEN; ++t) {
			if (toZeroize[t][d] > 0) {
				arr[i++] = toZeroize[t][d] / distribution[t];
			}
		}
		qsort(arr, i, sizeof(double), cmpUp);
		int boarder = int(ratio * i) - 1; // ������ ��������. ������� ���, ��� <= ��� �����������.
		if (boarder >= 0) { // ����� ��� ������ ��������. ��� �������� �� �������� �� �����.
			double level = arr[boarder];
            averageLevel += level;
			for (int t = 0; t < LEN; ++t) {
				if (toZeroize[t][d] <= level * distribution[t]) {
					toReduce[d] -= toZeroize[t][d];
					assert(toReduce[d] > -MP);
					if (toReduce[d] < 0)
						toReduce[d] = 0;
					toZeroize[t][d] = 0;
				} else {
                    toZeroize[t][d] -= level * distribution[t];
                    assert(toZeroize[t][d] > 0);
                    toReduce[d] -= level * distribution[t];
                    assert(toReduce[d] > 0);
                }
			}
		}
	}
	delete [] arr;
    averageLevel /= NUM;
    return averageLevel;
}



// ��������� ������������ ���� ���������� ��������
double sparse_matrix(double ratio, long double** toZeroize, long double* toReduce, int LEN, int NUM) {
    double level = 0;
	// ����� �������� ������������ ���� ��������� �����
	double* arr = new double [LEN * NUM];
	int notZeroCount = 0;
	for (int d = 0; d < NUM; ++d) {
		for (int t = 0; t < LEN; ++t) {
			if (toZeroize[t][d] > 0) {
				arr[notZeroCount++] = toZeroize[t][d];
			}
		}
	}
	qsort(arr, notZeroCount, sizeof(double), cmpUp);
	int boarder = int(ratio * notZeroCount) - 1; // ������ ��������. ������� ���, ��� <= ��� �����������.
	if (boarder >= 0) { // ����� ��� ������ ��������. ��� �������� �� �������� �� �����.
		level = arr[boarder];
		for (int d = 0; d < NUM; ++d) {
			for (int t = 0; t < LEN; ++t) {
				if (toZeroize[t][d] <= level) {
					toReduce[d] -= toZeroize[t][d];
					assert(toReduce[d] > -MP);
					if (toReduce[d] < 0)
						toReduce[d] = 0;
					toZeroize[t][d] = 0;
				}
			}
		}
	}
    delete [] arr;
    return level;
}
/*
// ������� ��� p(t|d,w) � PLSA
void collection::sparseHdwt(double ratio, long double Zdw) {
	// ����� �������� ������������ ���� ��������� p
	int i = 0; 	double sum = 0;
	for (int t = 0; t < CAT; ++t) {
		if (p[t] > 0) {
			pHdwt[i++] = p[t];
		}
	} // � i ����� �������
	qsort(pHdwt, i, sizeof(double), cmpUp);
	int boarder = int(ratio * i) - 1;
	if (boarder >= 0) {
		double level = pHdwt[boarder];
		for (int t = 0; t < CAT; ++t) {
			if (p[t] <= level) {
				p[t] = 0;
			} else {
				sum += p[t];
			}
		}
		assert (sum > 0);
		for (int t = 0; t < CAT; ++t) {
			p[t] = p[t] * Zdw / sum;
		}
	}
}
*/
// ������� ��� p(t|d,w) � PLSA
void collection::sparseHdwt(double ratio, long double Zdw) { // ������ dobule ratio ��������� ��� s ���� ��������
	// ����� �������� ������������ ���� ��������� p
	int i = 0; 	double sum = 0;
	for (int t = 0; t < CAT; ++t) {
		if (p[t] > 0) {
			pHdwt[i++] = p[t];
		}
	} // � i ����� �������
	qsort(pHdwt, i, sizeof(double), cmpDown); // ��������������� �� �������� Hdwt
	
	double level = pHdwt[(ratio <= i)? int(ratio-1): i-1];

	for (int t = 0; t < CAT; ++t) {
		if (p[t] < level) { // �������� s ���� ��� ������
			p[t] = 0;
		} else {
			sum += p[t];
		}
	}
	assert (sum > 0);
	for (int t = 0; t < CAT; ++t) {
		p[t] = p[t] * Zdw / sum;
	}
	
}
