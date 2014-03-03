#include "stdafx.h"
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
extern double MP;

// ������ �����: DOC \n WOR (� �������� ������ ������� ��������, �� �������� ������ ������ �������) \n N1 \n w1 ....wN1 \n n1 .... \n .....

class node {
public:
	node* next;
	int wordIdx;
	double phiValue;

	node(int w, double p, node* n) {
		wordIdx = w;
		phiValue = p;
		next = n;
	}
	~node() {}
};


struct IdProb {
	int id;
	double prob;

    IdProb() {}

    IdProb(int _id, double _prob) {
        id = _id;
        prob = _prob;
    }
};



enum t_col {RuDisOld, RuDisOldNW, NIPSOld, RuDis, RuDisNWS, NIPS, NIPSNWS};
// ������ ��������� ������� 1 � 9, ����� ���� �� �������� ���
// ����� ��������� ������� 1 � 1, ������� NWS �������� ������� ����� ����, ������ ��� ��� ���� � ������� ����� ����, ������������� � ��������
enum t_fun {P, S}; // ���������������� ������������� ��� ������������� ��� ����� p(t|d,w)
enum t_init {uniSam, rnd}; // ������������� �������������� �� ������������ ������������� ��� ���������
enum t_piStep {add, mult}; // ���������� ��� ����������������� ��� ���������� pi_dw
enum t_sFun {sRaws, sTails, sMatrix}; // ��������� ��������� ��������������� ������������
enum t_sDistr {sOld, sUniform, sBackTrain, sBackLit}; // ��������� ������������� ��� ������������ ��� �����������

class document {
public:
	int Nd; // ����� ��������� ����
	//��� ��������� ������ ����� Nd
	int * words; // ������ ����
	int * numbs; // ������� ��� ����������� � ��������� (��� �������� - � ������������� ��������)
	int * rest; // ��� �������� �� ������������, ��� �������� ������� ��� ����������� � ����������� ��������
	
	double* Tscores; // ������������� ����
	double* Nscores; // ��� ����

	double * lastOne; // ���� �� Nd. ������������ �������� ��� ������� ����� �� ���������� ��������
	int ** z; // ��������� ������ ������ Nd * sam_num ��� �������� ����������� ���

	document();
	~ document();

	// ������� ������ �� ������. ����� ������, ������ ��� ���� �������� � �����������.
	void str2arr(std::string line, int* arr);
	void read(std::ifstream & f);
};

// ������ ��������� ���������, ������ �����������
// ��������� ����� ������ ������������ ������ � ��������� (t) ��� ������ � ����������� (c) ����������
class collection { 
public:
	int DOC, WOR, CAT; // WOR �� �������� -- ������ ������� ��������, �� �������� -- ����� ������ �������
	int NW; // ����� ����� ���� (c) 
	double N; // ����� ��������� (��� �������� - ��� ������������� �����)
	
	document * corpus;
	
	long double** nwt; // WOR * CAT
	long double** ntd; // CAT * DOC
	long double* nt; // CAT
	long double* nd; // DOC
	
	double** phi; // CAT * WOR (t)
	double* phiNW; // CAT (c)
	double** theta; // DOC * CAT

    double* phiSum; // WOR
    double* pt; // CAT - not normed p(t), for training only
    double* backDistribution;
    double* ones;


    // robust staff
	double** pi; // DOC * Nd
	double* fo; // WOR - � �������� ������ pi_w - ������� ���������� (t)
	double** nu; // DOC * Nd
	double** mu; // DOC * Nd - � �������� ������ nu'_dw (t)
	double* nude; // DOC
	double* muwe; // WOR (t)
	long double nu_sum;
	long double mu_sum; // (t)

	double* p; // CAT (��������������� ������ ��� �������� �������������)

	double *pHdwt;
	void sparseHdwt(double ratio, long double Zdw);

	collection();
	~collection();

	//S
	void one_gs_iteration   (bool robust, bool accumulated, double** t_phi, double* t_fo, double alpha, double beta, double dzeta, double eps, double HdwtSparsity, t_piStep piStep);
	//strict parts for tokens
	void one_gs_sp_iteration(bool robust, bool accumulated, double** t_phi, double* t_fo, double alpha, double beta, double gamma, double eps, double HdwtSparsity, t_piStep piStep);
	//P
	void one_plsa_iteration(bool robust, bool accumulated, double** t_phi, double* t_fo, double alpha, double beta, double gamma, double eps, double HdwtSparsity, t_piStep piStep);
	// ������ ���������
	int read(const char* filename);
	void init(t_fun fun, double alpha, double beta, double gamma, double eps, t_init init, double HdwtSparsity, bool robust, int _CAT, int mode, double portionOfTest);
	// ��������� ����������� uniSam
	void uniSam_dist(double alpha, double beta, double gamma, double eps, bool robust, int mode);
	// ��������� ����������� rnd
	void rnd_dist(bool robust, int mode);
	
	//��������� �������� �� 2 ��������� �������� - � numbs �������� ������������� ��������, � rest - ����������� (����� ������ ��� ����������� ���������)
	void random_half_resting(double portion);
	void exact_half_resting(); // ��������� �� 2 ������ �������� - ��� ndw �������.
	void not_half_resting(); // ��������� numbs � rest. �����, ���� �� ����� ������ ���������������, � �������� � ��� �������.
	
	void Tscores_cntl(std::fstream &f, int i, int d);
    node* BuildTopWordsList(int topic, int num);
	void topWordsOut(std::fstream &f, int num);
	void topDocsOut(std::fstream &f, int num);
	void topBackgroundOut(std::fstream &f, int num);
	void topNoiseOut(std::fstream &f, int num);
    void pt_update();
	void print_phi(const char* filename);
	void print_theta(const char* filename);
	void sumdt_cntl(const char* filename);
	void sumtw_cntl(const char* filename);

    void collection::ContrastPurity(std::fstream &f);

	// ��������������� �������, ���������� �� one_***_iteration ��� �������������� ����������, ����������� � �����������
	void noise_count(long double Z, double gamma, int d, int w);
	void back_count(long double Z, double eps, int d, int w);
};

class model {
public:
	t_fun fun; // ������������ �������� P ��� S
//	bool AssignmentToNoise; // ���� ��, �� ��������� �� ��������� 2.5. ���� ���� ������ ��-�������� �����-�� ��������� ����� � �������
	t_sDistr smoothDistr;
    double alpha;
	double beta;
    int anticorrStart;
    double anticorrCoef;
    int anticorrInc;
	double gamma;
	double eps;
	t_init init;
	double HdwtSparsity; // GS: 0->ndw �������, ����� min(x, ndw). PLSA: 0->�������, ����� ��������� ���� x ��������� �������� p[t].
	t_piStep piStep;
	t_sFun sFun;
    t_sDistr sparseDistr;
	int sStart, sStep;
	double sRatioTheta, sRatioPhi;
	int CAT, ITER;
    int swCAT;
	bool robust;
    bool sparse;
    bool anticorrelated;
	double delta;
    double tHeadSumRatio, tHeadItemsRatio;
	collection training;
	collection control;

    double** wordCooccurences; // WOR * WOR, using training+control, for coherence
    double* wordOccurences; // WOR
	
	//model();
	model(t_fun _fun, t_sDistr _smoothDistr, double _alpha, double _beta, double _gamma, double _eps, t_init _init, double _HdwtSparsity, t_piStep _piStep, 
		   t_sFun _sFun, t_sDistr _sparseDistr, int _sStart, int _sStep, double _sRatioTheta, double _sRatioPhi, double _delta, int _CAT, int _swCAT, int _ITER,
           double _tHeadSumRatio, double _tHeadItemsRatio, int _anticorrStart, double _anticorrCoef, int _anticorrInc);
	~ model ();
	 	
	void initialization(double portionOfTest);
    void CalculateWordOccurencesCooccurences(collection* col);
    void PrintWordCooccurences(const char* filename);
    void ReadWordCooccurences(const char* filename);
	void main_proc(const char* name); // �������� �������
    double CalculateZdw(collection* col, int mode, int d, int w); // ��� ����������
	void perplexity_estimation(collection* col, std::fstream &f, int mode); // ������� ���������� �� ������� �� �������� (mode==0) ��� �������� (mode==1)
	void perplexity_control_learning(std::fstream &f, int iTrain); // �������� ������ �� ������������� �������� �������� � ����� pl_estimation ��� ��������
	void sparsity_estimation(std::fstream &f);
    vector< vector<IdProb> > contrast_purity_estimation(std::fstream &f, bool getKernelWords);
    void coherence_estimation(std::fstream &f, int num);
    void model::KernelWordsOut(std::fstream &f, vector< vector<IdProb> > kernelWords);
    void terms_sparsity_histogram(std::fstream &f);
    void phi_update(int iteration);
	void theta_update(collection* col);
};

// �������

// ��������� ��������� ����� �� ���������������� �������������
template <class T> 
int multigen(T p, int LEN) { // �������� ��� ������� � ��� �����
	double u = ((rand() + 1.0) / (RAND_MAX + 1.0)) * p[LEN - 1]; // ����������
	int a = -1, b = LEN-1, x; 
	while ((b - a) != 1) {
		x = (a + b) / 2;
		if (u > p[x]) {
			a = x;
		} else {
			b = x;
		}
	}
	return b; // �������
}


void launch(t_col col, t_fun fun, double alpha, double beta, double gamma, double eps, t_init init,
	        double HdwtSparsity, t_piStep piStep, t_sFun sFun, int sStart, int sStep, double sRatioTheta, double sRatioPhi, 
			int CAT, int ITER, const char* comment, double delta = 0,
            double tHeadSumRatio = 0.5, double tHeadItemsRatio = 0.02, t_sDistr sparseDistr = sOld, int swCAT = 0, t_sDistr smoothDistr = sUniform,
            int anticorrStart = 0, double anticorrCoef = 0, int anticorrInc = 1);
void LaunchDecember2013(t_col col, int CAT, int swCAT, int ITER,
                         t_sDistr smoothDistr, double alpha, double beta, 
                         t_sFun sFun, t_sDistr sparseDistr, int sStart, int sStep, double sRatioTheta, double sRatioPhi, 
                         int anticorrStart, double anticorrCoef, int anticorrInc, double tHeadSumRatio, double tHeadItemsRatio, const char* comment);
void CalculateWordCooccurences(t_col col);
int cmpUp (const void * elem1, const void * elem2);
int cmpDown (const void * elem1, const void * elem2);
void count_matrix_sparsity(std::fstream &f, double** param, int H, int W);
void ckeck_sum_matrix(std::fstream &f, double** param, int H, int W);
void count_matrix_sparsity_long(std::fstream &f, long double** param, int H, int W);
void count_pi_sparsity_and_sum(std::fstream &f, collection* collection);
double sparse_raws_old(double ratio, long double** toZeroize, long double* toReduce, int LEN, int NUM);
double sparse_raws(double ratio, double* distribution, long double** toZeroize, long double* toReduce, int LEN, int NUM);
double sparse_tail(double sumRation, double maxRatio, long double** toZeroize, long double* toReduce, int LEN, int NUM);
double sparse_matrix(double ratio, long double** toZeroize, long double* toReduce, int LEN, int NUM);
void fillRandomly(double* param, int LEN);

