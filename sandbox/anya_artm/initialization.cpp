#include "stdafx.h"
#include "types.h"

double MP = 0.00000001; //  числа, мЕньшие по модулю считаются нулем.
//int NW = 0; //249;// - nips halves //0; //35; // число слов в контрольной коллекции, которых не было в обучении.
// Для моей коллекции я его просто знаю (оно равно 35). Когда будет время, посчитатю в программе.


//**********************КОНСТРУКТОРЫ И ДЕСТРУКТОРЫ КЛАССОВ****************************

document::document() {
	Nd = 0;
	words = NULL; numbs = NULL; rest = NULL; lastOne = NULL; z = NULL; Tscores = NULL; Nscores = NULL;
}

document::~document() {
	if (words) 
		delete [] words;
	if (numbs)
		delete [] numbs;
	if (rest)
		delete [] rest;
	if (z) {
		for (int w = 0; w < Nd; ++w) {
			if (z[w]) { // если ndw == 0, то массив не заводился
				delete [] z[w]; // удалили одномерный массив на ndw
			}
		}
		delete [] z;
	}
	if (lastOne) {
		delete [] lastOne;
	}
	if (Tscores) {
		delete [] Tscores;
	}
	if (Nscores) {
		delete [] Nscores;
	}
}

void document::str2arr(std::string line, int* arr) {
	std::string word;
	int words_count = 0;
	for (size_t i = 0; i < line.length(); i++) {
		char a = line[i];
		if (a == ' ') {
			arr[words_count] = atoi(word.c_str());
			words_count++;
			word.clear();
		} else {
			word.push_back(a);
		}
	}
	if (word.length() != 0) {
		arr[words_count] = atoi(word.c_str());
	}
}

void document::read(std::ifstream & f) {
	f >> Nd;
	words = new int[Nd];
	numbs = new int[Nd];
		
	std::string line;
	getline(f, line); // какой-то энтер дочитать
	getline(f, line);
	str2arr(line, words);
		
	getline(f, line);
	str2arr(line, numbs);
}

collection::collection() {
	N = 0; DOC = 0; WOR = 0; CAT = 0; NW = 0;
	nu_sum = 0; mu_sum = 0;
	corpus = NULL;
	nwt = NULL; ntd = NULL; nt = NULL; nd = NULL;
	phi = NULL; phiNW = NULL; theta = NULL;
	pi = NULL; 	nu = NULL; nude = NULL;
	p = NULL; 
	fo = NULL; mu = NULL; muwe = NULL;
    phiSum = NULL;
}

collection::~collection() {
	if (corpus) 
		delete [] corpus;

	if (nwt) {
		for (int w = 0; w < WOR; ++w) {
			if (nwt[w])
				delete [] nwt[w];
		}
		delete [] nwt;
	}
	if (ntd) {
		for (int t = 0; t < CAT; ++t) {
			if (ntd[t])
				delete [] ntd[t];
		}
		delete [] ntd;
	}
	if (nt) 
		delete [] nt;
	if (nd) 
		delete [] nd;
  	if (phi) {
		for (int t = 0; t < CAT; ++t) {
			if (phi[t])
				delete [] phi[t];
		}
		delete [] phi;
	}
    if (phiSum) {
		delete [] phiSum;
	}
	if (phiNW) {
		delete[] phiNW;
	}
	if (theta) {
		for (int d = 0; d < DOC; ++d) {
			if (theta[d])
				delete [] theta[d];
		}
		delete [] theta;
	}
	if(backDistribution) {
        delete[] backDistribution;
    }
    if(ones) {
        delete [] ones;
    }
    if (pi) {
		for (int d = 0; d < DOC; ++d) {
			if (pi[d])
				delete [] pi[d];
		}
		delete [] pi;
	}
	if (fo)
		delete [] fo;
	if (nu) {
		for (int d = 0; d < DOC; ++d) {
			if (nu[d])
				delete [] nu[d];
		}
		delete [] nu;
	}
	if (mu) {
		for (int d = 0; d < DOC; ++d) {
			if (mu[d])
				delete [] mu[d];
		}
		delete [] mu;
	}
	if (nude) 
		delete [] nude;
	if (muwe)
		delete [] muwe;
	if (p)
		delete [] p;
	if (pHdwt)
		delete [] pHdwt;
}

model::model(t_fun _fun, t_sDistr _smoothDistr, double _alpha, double _beta, double _gamma, double _eps, t_init _init, double _HdwtSparsity, t_piStep _piStep, 
		     t_sFun _sFun, t_sDistr _sparseDistr, int _sStart, int _sStep, double _sRatioTheta, double _sRatioPhi, double _delta, int _CAT, int _swCAT, int _ITER,
             double _tHeadSumRatio, double _tHeadItemsRatio, int _anticorrStart, double _anticorrCoef, int _anticorrInc) {
	fun = _fun;
	alpha = _alpha;
	beta = _beta;
	gamma = _gamma;
	eps = _eps;
	init = _init;
	HdwtSparsity = _HdwtSparsity;
	piStep = _piStep;
	sFun = _sFun;
    sparseDistr = _sparseDistr;
    smoothDistr = _smoothDistr;
	sStart = _sStart;
	sStep = _sStep;
	sRatioTheta = _sRatioTheta;
	sRatioPhi = _sRatioPhi;
    anticorrCoef = _anticorrCoef;
    anticorrStart = _anticorrStart;
    anticorrInc = _anticorrInc;
	CAT = _CAT;
    swCAT = _swCAT;
	ITER = _ITER;
	robust = (gamma != 0 || eps != 0);
    sparse = (sRatioTheta != 0 || sRatioPhi != 0);
    anticorrelated = anticorrCoef != 0;
	delta = _delta;
    tHeadSumRatio = _tHeadSumRatio;
    tHeadItemsRatio = _tHeadItemsRatio;
    wordCooccurences = NULL;
    wordOccurences = NULL;
}

model::~model () {
    if (wordCooccurences) {
        for (int w = 0; w < control.WOR; ++w) {
            delete[] wordCooccurences[w];
        }
        delete[] wordCooccurences;
    }
    if (wordOccurences) {
        delete[] wordOccurences;
    }
}

int collection::read(const char* filename) {
	using namespace std;
	ifstream f(filename);
	if (!f)
		return 1; //ошибка при открытии файла
	f >> DOC;
	corpus = new document[DOC];
	f >> WOR;
	for (int i = 0; i < DOC; ++i) {
		corpus[i].read(f);
	}
	return 0;
}

//*****************************************ДЕЛЕНИЕ КОНТРОЛЯ*******************************************

void collection::random_half_resting(double portion) { // portion = 1/2 - адекватное
	for (int d = 0; d < DOC; ++d) { // в каждом документе
		corpus[d].rest = new int [corpus[d].Nd];
		for (int i = 0; i < corpus[d].Nd; ++i) {
			corpus[d].rest[i] = 0;
		}
		int* p_local = new int [corpus[d].Nd];
		p_local[0] = corpus[d].numbs[0];
		for (int w = 1; w < corpus[d].Nd; ++w) {
			p_local[w] = p_local[w - 1] + corpus[d].numbs[w];
		}
		for (int c = 0; c < nd[d] * portion; ++c) { // пока не наберем portion
			int word = multigen(p_local, corpus[d].Nd);
			++corpus[d].rest[word]; // половина для контроля
			--corpus[d].numbs[word]; // половина для настройки
			--N; // уменьшаем контрольное N
			assert(corpus[d].numbs[word] >= 0);
			for (int w = word; w < corpus[d].Nd; ++w) {
				--p_local[w];
			}
		}
		delete [] p_local;
	}
}

void collection::exact_half_resting() { // дает странные результаты =(
	for (int d = 0; d < DOC; ++d) { // в каждом документе
		nd[d] = 0;
		corpus[d].rest = new int [corpus[d].Nd];
		for (int i = 0; i < corpus[d].Nd; ++i) {
			corpus[d].rest[i] = 0;
		}
		for (int w = 0; w < corpus[d].Nd; ++w) {
			corpus[d].rest[w] = corpus[d].numbs[w] / 2;
			corpus[d].numbs[w] = corpus[d].numbs[w] - corpus[d].rest[w];
			nd[d] += corpus[d].numbs[w];
		}
		// у нас теперь как будто бы в 2 раза меньше слов
	}
}

void collection::not_half_resting() {
		for (int d = 0; d < DOC; ++d) { // в каждом документе
		corpus[d].rest = new int [corpus[d].Nd];
		for (int w = 0; w < corpus[d].Nd; ++w) {
			corpus[d].rest[w] = corpus[d].numbs[w];
		}
		//nd остался прежним
	}
}

//************************************ИНИЦИАЛИЗАЦИИ**************************************

void collection::uniSam_dist(double alpha, double beta, double gamma, double eps, bool robust, int mode) {
	// распределение слов по темам
	for (int d = 0; d < DOC; ++d) { // документы
		for (int w = 0; w < corpus[d].Nd; ++w) { // по словам
			int ndw = corpus[d].numbs[w];
			int extra = (gamma != 0) + (eps != 0);
			for (int n = 0; n < ndw; ++n) { // по слововхождениям. Для каждого слововхождения выбираем рандомно одну сущность.
				/*
				int topic;
				do
					topic = rand() / (RAND_MAX / (CAT + extra));
				while (topic >= CAT + extra); //выбрали topic для нашего слововхождения

				if (topic < CAT) { // тема
					nwt[corpus[d].words[w]][topic]++;
					ntd[topic][d]++;
					nt[topic]++;
					nd[d]++;
				} else if (gamma != 0 && topic == CAT) {
					nu[d][w]++;
					nude[d] ++;
				} else if (mode == 0) {
					muwe[corpus[d].words[w]] ++; 
					mu_sum ++;
				}*/

				int topic, role;
				double probRoles[3] = {1 - gamma - eps, gamma, eps};
				for (int r = 1; r < 3; ++r) { //cumsum
					probRoles[r] += probRoles[r - 1];
				}
				role = multigen(probRoles, 3);
				if (role == 0) {
					do
						topic = rand() / (RAND_MAX / (CAT));
					while (topic >= CAT); //выбрали topic для нашего слововхождения

					nwt[corpus[d].words[w]][topic]++;
					ntd[topic][d]++;
					nt[topic]++;
					nd[d]++;
				} else if (role == 1) {
					nu[d][w]++;
					nude[d] ++;
				} else if (mode == 0) {
					muwe[corpus[d].words[w]] ++; 
					mu_sum ++;
				}
			} // по слововхождениям
		} // по словам
		if (robust) {
			for (int w = 0; w < corpus[d].Nd; ++w) {
				pi[d][w] = (nu[d][w] + 0.01) / (nude[d] + corpus[d].Nd * 0.01);
			}
		}
	}	
	for (int t = 0; t < CAT; ++t) { 
		for (int d = 0; d < DOC; ++d) {
			assert(nd[d] > 0);
			theta[d][t] = (ntd[t][d] + 0.5) / (nd[d] + CAT * 0.5); // ! попробуем сглаживать !ВСЕГДА!. а то напихаем сразу нулей в распределение...
		}
		if (mode == 0) {
			for (int w = 0; w < WOR; ++w) {
				assert(nt[t] > 0);
				phi[t][w] = (nwt[w][t] + 0.01) / (nt[t] + WOR * 0.01);
				assert(phi[t][w] >= 0);
			}
		}
	}
	if (robust && mode == 0) {
		for (int w = 0; w < WOR; ++w) {
			fo[w] = (muwe[w] + 0.01) / (mu_sum + WOR * 0.01);
		}
	}
	// Получив распределения, обнуляем все счетчики
	for (int t = 0; t < CAT; ++t) {
		if (mode == 1) {
			nt[t] = 0; // коряво, да. но обучающий nt нам ещё пригодится для оценки фи новых слов.
		}
		for (int d = 0; d < DOC; ++d) {
			ntd[t][d] = 0;
			nd[d] = 0;
		}
		for (int w = 0; w < WOR; ++w) {
			nwt[w][t] = 0;
		}
	}

	if (robust) {
		for (int d = 0; d < DOC; ++d) {
			for (int w = 0; w < corpus[d].Nd; ++w) {
				nu[d][w] = 0;
			}
			nude[d] = 0;
		}
		if (mode == 0) {
			mu_sum = 0;
			for (int w = 0; w < WOR; ++w) {
				muwe[w] = 0;
			}
		}
	}
}

void fillRandomly(double* param, int LEN) {
	long double sum = 0;
	for (int i = 0; i < LEN; ++i) {
		param[i] = rand();
		sum += param[i];
	}
	for (int i = 0; i < LEN; ++i) {
		param[i] /= sum;
	}
}

void collection::rnd_dist(bool robust, int mode) {
    if (mode == 0) {
		for (int t = 0; t < CAT; ++t) {
			fillRandomly(phi[t], WOR);
		}
	}
	for (int d = 0; d < DOC; ++d) {
		fillRandomly(theta[d], CAT);
	}
	if (robust) {
		for (int d = 0; d < DOC; ++d) {
			fillRandomly(pi[d], corpus[d].Nd);
		}
		if (mode == 0) {
			fillRandomly(fo, WOR);
		}
	}
}

// mode == 0 - инициализация коллекции для обучения, 1 - для контроля
void collection::init(t_fun fun, double alpha, double beta, double gamma, double eps, t_init init, double HdwtSparsity, bool robust, int _CAT, int mode, double portionOfTest) {
	CAT = _CAT;
	nwt = new long double* [WOR];
    backDistribution = new double [WOR];
    ones = new double [WOR];
	for (int w = 0; w < WOR; ++w) {
		nwt[w] = new long double [CAT];
		for (int t = 0; t < CAT; ++t) {
			nwt[w][t] = 0;
		}
        backDistribution[w] = 0;
        ones[w] = 1;
	}
	nt = new long double [CAT];
	p = new double [CAT];
	pHdwt = new double [CAT];
	ntd = new long double* [CAT];
	for (int t = 0; t < CAT; ++t) {
		nt[t] = 0;
		p[t] = 0;
		ntd[t] = new long double [DOC];
		for (int d = 0; d < DOC; ++d) {
			ntd[t][d] = 0;
		}
	}
	nd = new long double [DOC];
		
	for (int d = 0; d < DOC; ++d) {
		nd[d] = 0;
		if (fun == S) { // структуры для запоминания тем
			corpus[d].z = new int* [corpus[d].Nd];
			corpus[d].lastOne = new double [corpus[d].Nd];
		}
		for (int w = 0; w < corpus[d].Nd; ++w) {
			int ndw = corpus[d].numbs[w];
			nd[d] += ndw;
            backDistribution[corpus[d].words[w]] += ndw;
			if (fun == S) { // структуры продолжаются
				int sam_num = (HdwtSparsity == 0 ? ndw : HdwtSparsity > 0 ? min(ndw, HdwtSparsity) : -HdwtSparsity);
				corpus[d].z[w] = new int [sam_num]; // значения -- мусор.
				corpus[d].lastOne[w] = 0;
			}
		} // слова
		N += nd[d];
	} // посчитали N, для контроля это пока длина всех половинок вместе, в делении уменьшим.

    for (int w = 0; w < WOR; ++w) {
        backDistribution[w] /= N;
    }
	
	if (mode == 1) {
		random_half_resting(portionOfTest); // ДЕЛЕНИЕ КОНТРОЛЯ ПОПОЛАМ
	}
	if (mode == 0) {
        pt = new double [CAT];
		phi = new double* [CAT];
        phiSum = new double [WOR];
		for (int t = 0; t < CAT; ++t) {
			phi[t] = new double [WOR];
            pt[t] = 0;
		}
	} 
	theta = new double* [DOC];
	for (int d = 0; d < DOC; ++d) {
		theta[d] = new double [CAT];
		nd[d] = 0; // обнулили nd
	}	
	if (robust) {
		if (mode == 0) {
			muwe = new double [WOR];
			fo = new double [WOR];
			for (int w = 0; w < WOR; ++w) {
				muwe[w] = 0;
				fo[w] = 0;
			}
			mu = new double* [DOC];
		}
		
		pi = new double* [DOC];
		nu = new double* [DOC];
		nude = new double [DOC];
		for (int d = 0; d < DOC; ++d) {
			pi[d] = new double [corpus[d].Nd];
			nu[d] = new double [corpus[d].Nd];
			if (mode == 0) {
				mu[d] = new double [corpus[d].Nd];
			}
			nude[d] = 0;
			for (int w = 0; w < corpus[d].Nd; ++w) {
				nu[d][w] = 0;
				pi[d][w] = 0;
				if (mode == 0) {
					mu[d][w] = 0;
				}
			}
		}
	}	
	// копит тематические счетчики и оценивает по ним распределения, обнуляет счетчики
	if (init == uniSam) {
		uniSam_dist(alpha, beta, gamma, eps, robust, mode);
	} else {
		rnd_dist(robust, mode);
	}

	// выделяем память для оценок тематичности слов
	for (int d = 0; d < DOC; ++d) {
		corpus[d].Tscores = new double [corpus[d].Nd];
		corpus[d].Nscores = new double [corpus[d].Nd];
	}

}

void model::initialization(double portionOfTest) {
	training.init(fun, alpha, beta, gamma, eps, init, HdwtSparsity, robust, CAT, 0, portionOfTest);
	control.init(fun, alpha, beta, gamma, eps, init, HdwtSparsity, robust, CAT, 1, portionOfTest);

	control.phiNW = new double [CAT];
	for (int i = 0; i < CAT; ++i) {
		control.phiNW[i] = 0;
	}

	control.NW = control.WOR - training.WOR;
	if (control.NW != 0  && !robust) { 
		if (beta == 0 && delta == 0) { // только в этом случае нужно вычислить оптимальную дельту
			int newWords = 0, anyWords = 0;
			for (int d = 0; d < control.DOC; ++d) { // пробегаемся по контролю
				for (int w = 0; w < control.corpus[d].Nd; ++w) {
					int ndw = control.corpus[d].numbs[w] + control.corpus[d].rest[w];
					if (control.corpus[d].words[w] >= training.WOR) {
						newWords += ndw;
					}
					anyWords += ndw;
				}
			}
			delta = 1.0 * newWords / anyWords / control.NW;
		}

		if (beta != 0) {
			for (int t = 0; t < CAT; ++t) {
				control.phiNW[t] = beta / (training.nt[t] + control.WOR * beta);
			}
		} else {
			for (int t = 0; t < CAT; ++t) {
				control.phiNW[t] = delta;
			}
		}
	}
	for (int t = 0; t < CAT; ++t) { // ну и кто будет за меня это делать?!
		training.nt[t] = 0;
	}
}


void model::CalculateWordOccurencesCooccurences(collection* col) {
    for (int d = 0; d < col->DOC; ++d) {
        // printf("processing document %d...", d);
        for (int u = 0; u < col->corpus[d].Nd; ++u) {
            int left = col->corpus[d].words[u];
            for (int v = u; v < col->corpus[d].Nd; ++v) {
                int right = col->corpus[d].words[v];
                wordCooccurences[min(left,right)][max(left, right)] += 1;
            }
            wordOccurences[left] += 1;
        }
    }
}

void model::PrintWordCooccurences(const char* filename) {
    std::fstream out_file(filename, std::fstream::out | std::fstream::app);
    for (int wSmall = 0; wSmall < control.WOR; ++wSmall) {
        for (int wBig = wSmall + 1; wBig < control.WOR; ++wBig) {
            out_file << wordCooccurences[wSmall][wBig] << '\t';
        }
        out_file << '\n';
    }
    out_file.close();
}

void model::ReadWordCooccurences(const char* filename) { 

}

//**************************СТО ЛЕТ НЕ ЗАПУСКАЛА************************************
/*
// Начальное приближение с помощью быстрой кластеризации
void collection::clever_dist() { // проверь что с N. раньше заполнялся в самом начале вместе с nd
	int R = DOC - CAT; // параметр
	
	for (int d = 0; d < DOC; ++d) {  // нулевая инициализация всех списков!
		corpus[d].z = new node* [corpus[d].Nd];
		for (int w = 0; w < corpus[d].Nd; ++w) {
			corpus[d].z[w] = NULL;
		}
	}

	double *nw = new double [WOR];
	for (int w = 0; w < WOR; ++w) {
		nw[w] = 0;
	}
	for (int t = 0; t < CAT; ++t) {
		int d = t; // выбираем первые 100 документов
		N += nd[d];
		nt[t] += nd[d];
		ntd[t][d] += nd[d];
		for (int w = 0; w < corpus[d].Nd; ++w) {
			corpus[d].z[w] = new node(t, corpus[d].numbs[w], NULL);
			nwt[corpus[d].words[w]][t] = corpus[d].numbs[w];
			nw[corpus[d].words[w]] += corpus[d].numbs[w];
		}
		for (int gw = 0; gw < WOR; ++gw) {
			phi[t][gw] = nwt[gw][t] / nt[t];
		}
	}
	for (int r = CAT; r < CAT + R; ++r) {
		int d = r;
		N += nd[d];
		double romin = WOR, ro;
		int tmin;
		for (int t = 0; t < CAT; ++t) {
			ro = 0;
			for (int gw = 0; gw < WOR; ++gw) {
				ro += 0.5 * phi[t][gw];
			}
			for (int w = 0; w < corpus[d].Nd; ++w) {
				double pwd = corpus[d].numbs[w] / nd[d];
				ro += 0.5 * pwd - sqrt(phi[t][corpus[d].words[w]]) * sqrt(pwd);
			} // посчитали расстояние до темы t
			if (ro < romin) {
				romin = ro;
				tmin = t;
			}
		} // выбрали ближайшую тему tmin
		nt[tmin] += nd[d];
		ntd[tmin][d] += nd[d];
		for (int w = 0; w < corpus[d].Nd; ++w) {
			corpus[d].z[w] = new node(tmin, corpus[d].numbs[w], NULL);
			nwt[corpus[d].words[w]][tmin] += corpus[d].numbs[w];
			nw[corpus[d].words[w]] += corpus[d].numbs[w];
		}
		for (int gw = 0; gw < WOR; ++gw) {
			phi[tmin][gw] = nwt[gw][tmin] / nt[tmin];
		}
	}
	for (int d = 0; d < DOC; ++d) {
		for (int t = 0; t < CAT; ++t) {
			theta[d][t] = 0;
			for (int w = 0; w < corpus[d].Nd; ++w) {
				theta[d][t] += corpus[d].numbs[w] / nd[d] * nwt[corpus[d].words[w]][t] / nw[corpus[d].words[w]];
			}
		}
	}
	delete [] nw;

	double a = 0.001, b = 0.000001;
	for (int t = 0; t < CAT; ++t) { 
		for (int d = 0; d < DOC; ++d) {
			theta[d][t] = (theta[d][t] + a) / (1 + CAT*a);
			assert (theta[d][t] > 0);
		}
		for (int w = 0; w < WOR; ++w) {
			phi[t][w] = (phi[t][w] + b) / (1 + WOR*b);
			assert (phi[t][w] > 0);
		}
	}
}
*/