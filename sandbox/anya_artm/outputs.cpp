#include "stdafx.h"
#include "types.h"


// ****** вывод разреженностей плюс assert-ы на равенство всех сумм 1 *******

void count_matrix_sparsity(std::fstream &f, double** param, int H, int W) {
	long double count = 0;
	long double sum = 0;
	for (int d = 0; d < H; ++d) {
		for (int t = 0; t < W; ++t) {
			sum += param[d][t];
			if (param[d][t] == 0)
				++count;
		}
	}
 //   assert(sum / H > 1.0 - MP && sum / H < 1.0 + MP); // мимоходом проверяем, что распределения сумируются в 1.
	f << '\t' << count / (H * W);
}

void ckeck_sum_matrix(std::fstream &f, double** param, int H, int W) {
	long double sum = 0;
	for (int d = 0; d < H; ++d) {
        sum = 0;
		for (int t = 0; t < W; ++t) {
			sum += param[d][t];	
		}
        assert(sum > 1.0 - MP && sum < 1.0 + MP); // проверяем, что распределения сумируются в 1.
	}
}

void count_matrix_sparsity_long(std::fstream &f, long double** param, int H, int W) {
	long double count = 0;
	for (int d = 0; d < H; ++d) {
		for (int t = 0; t < W; ++t) {
			if (param[d][t] == 0)
				++count;
		}
	}
	f << '\t' << count / (H * W);
}

void count_pi_sparsity_and_sum(std::fstream &f, collection* collection) {
	long double zero = 0;
	long double any = 0;
	long double sum = 0;
	for (int d = 0; d < collection->DOC; ++d) {
		for (int w = 0; w < collection->corpus[d].Nd; ++w) {
			++any;
			sum += collection->pi[d][w];
			if (collection->pi[d][w] == 0)
				++zero;
		}
	}
	f << '\t' << zero / any << '\t' << sum / collection->DOC;
}

void model::sparsity_estimation(std::fstream &f) {
	count_matrix_sparsity(f, training.phi, CAT - swCAT, training.WOR);
    ckeck_sum_matrix(f, training.phi, CAT, training.WOR);
//	count_matrix_sparsity_long(f, training.nwt, training.WOR, CAT);

	count_matrix_sparsity(f, training.theta, training.DOC, CAT - swCAT);
    ckeck_sum_matrix(f, training.theta, training.DOC, CAT);
//	count_matrix_sparsity_long(f, training.ntd, CAT, training.DOC);
	
	count_matrix_sparsity(f, control.theta, control.DOC, CAT - swCAT);
    ckeck_sum_matrix(f, control.theta, control.DOC, CAT);
//	count_matrix_sparsity_long(f, control.ntd, CAT, control.DOC);
	
	if (robust) {
		count_matrix_sparsity(f, &(training.fo), 1, training.WOR);
		count_pi_sparsity_and_sum(f, &training);
		count_pi_sparsity_and_sum(f, &control);

		f << '\t' << training.mu_sum / training.N;
		f << '\t' << training.nu_sum / training.N;
		f << '\t' << control.nu_sum / control.N;
	}
}


// ******* Тематичности слов, контрастность и чистота тем *********


int cmpDownIdProb (const void * elem1, const void * elem2) {
	return ((IdProb *)elem1)->prob - ((IdProb *)elem2)->prob < 0? +1 :
		   ((IdProb *)elem1)->prob - ((IdProb *)elem2)->prob > 0? -1 : 0; 
}


vector< vector<IdProb> > model::contrast_purity_estimation(std::fstream &f, bool getKernelWords) {
    int tCAT = CAT - swCAT;
    double termsNumber = 0;
    double* topicContrast = new double[tCAT];
    double* topicPurity = new double[tCAT];
    double* kernelSize = new double[tCAT];
    double* kernelCoherence = new double[tCAT];
    for (int t = 0; t < tCAT; ++t) {
        topicContrast[t] = 0;
        topicPurity[t] = 0;
        kernelSize[t] = 0;
        kernelCoherence[t] = 0;
    }
    double averageTopicContrast = 0;
    double averageTopicPurity = 0;
    double averageKernelSize = 0;
    double averageKernelCoherence = 0;

    vector< vector<IdProb> > kernelWords(CAT); 

	IdProb* topics4w = new IdProb [tCAT];
	for (int w = 0; w < training.WOR; ++w) { // проход по словарю
		double Z = 0;
        int t = 0;
		for (; t < tCAT; ++t) { // формируем массив тем по тематическим темам
			topics4w[t].id = t;
			topics4w[t].prob = training.phi[t][w] * training.pt[t]; // от нормировки отноения больше-меньше не изменятся. успеем.
			Z += topics4w[t].prob;
		}
        for (; t < CAT; ++t) {
            Z += training.phi[t][w] * training.pt[t]; // сумму досчитываем по всем темам
        }
		qsort(topics4w, tCAT, sizeof(IdProb), cmpDownIdProb); // соритруем по вероятностям

        double head_sum = 0;
        int head_size = 0;
        for (int t = 0; t < tCAT && t < tHeadItemsRatio * CAT  && head_sum < tHeadSumRatio * Z; ++t) { // идем по голове тематических тем и насуммировываем доля от полной суммы
            head_sum += topics4w[t].prob;
            head_size += 1;
        }
        if (Z > 0 && head_sum >= tHeadSumRatio * Z) { // клево, слово тематичное
            termsNumber += 1;
            for (int t = 0; t < head_size; ++t) {
                int t_glob = topics4w[t].id;
                kernelSize[t_glob] += 1;
                topicContrast[t_glob] += topics4w[t].prob / Z;
                topicPurity[t_glob] += training.phi[t_glob][w];
              //  if (getKernelWords) {
                    kernelWords[t_glob].push_back(IdProb(w, topics4w[t].prob / Z));
              //  }
            }
        }
	}

    for (int t = 0; t < tCAT; ++t) {
        for (int i = 0; i < kernelWords[t].size(); ++i) {
            int left = kernelWords[t][i].id;
            for (int j = i + 1; j < kernelWords[t].size(); ++j) {
                int right = kernelWords[t][j].id;
                if (wordCooccurences[left][right] * (training.DOC + control.DOC) > wordOccurences[left] * wordOccurences[right]) {
                    kernelCoherence[t] += log(wordCooccurences[left][right] * (training.DOC + control.DOC) / wordOccurences[left] / wordOccurences[right]);
                }
            }
        }
    }

    for (int t = 0; t < tCAT; ++t) {
        if (kernelSize[t] != 0) {
            topicContrast[t] /= kernelSize[t];
        }
        if (kernelSize[t] > 2) {
                kernelCoherence[t] /= (kernelSize[t] * (kernelSize[t] - 1) / 2);
        }
        averageTopicContrast += topicContrast[t];
        averageTopicPurity += topicPurity[t];
        averageKernelSize += kernelSize[t];
        averageKernelCoherence += kernelCoherence[t];
    }
    
    f << '\t' << termsNumber / training.WOR << '\t' << averageTopicContrast / tCAT << '\t' << averageTopicPurity / tCAT 
      << '\t' << averageKernelSize / tCAT << '\t' << averageKernelCoherence / tCAT;

	delete[] topics4w;
    delete[] topicContrast;;
    delete[] topicPurity;
    delete[] kernelSize;
    delete[] kernelCoherence;
    return kernelWords;
}

void model::KernelWordsOut(std::fstream &f, vector< vector<IdProb> > kernelWords) {
    // sort?
    for (int t = 0; t < CAT; ++t) {
        for (int i = 0; i < kernelWords[t].size(); ++i) {
            int w = kernelWords[t][i].id;
            f << '\t' << w << ':' << training.phi[t][w] << ':' << kernelWords[t][i].prob;
        }
        f << '\n';
    }
    f << '\n' << '\n' << '\n';
}

void model::terms_sparsity_histogram(std::fstream &f) {
    int tCAT = CAT - swCAT;
    double* WordsWithNonZeros = new double[tCAT + 1];
    double* WordsWithHeadSize = new double[tCAT + 1];
    for (int t = 0; t < tCAT + 1; ++t) {
        WordsWithNonZeros[t] = 0;
        WordsWithHeadSize[t] = 0;
    }
  
	IdProb* topics4w = new IdProb [tCAT];
	for (int w = 0; w < training.WOR; ++w) { // для каждого слова по очереди
		double Z = 0;
        int t = 0;
		for (; t < tCAT; ++t) { // формируем массив тем по тематическим темам
			topics4w[t].id = t;
			topics4w[t].prob = training.phi[t][w] * training.pt[t]; // от нормировки отноения больше-меньше не изменятся. успеем.
			Z += topics4w[t].prob;
		}
        for (; t < CAT; ++t) {
            Z += training.phi[t][w] * training.pt[t]; // сумму досчитываем по всем темам
        }
		qsort(topics4w, tCAT, sizeof(IdProb), cmpDownIdProb); // соритруем по вероятностям

        double headSum = 0;
        int headSize = 0, nonZeros = 0;
        for (int t = 0; t < tCAT && headSum < tHeadSumRatio * Z; ++t) {
            headSum += topics4w[t].prob;
            headSize += 1;
        }
        for (int t = 0; t < tCAT && topics4w[t].prob != 0; ++t) {
            nonZeros += 1;
        }

        WordsWithNonZeros[nonZeros] += 1;
        if (headSize != 0) { // работаем только с терминами, имеющими ненули
            WordsWithHeadSize[headSize] += 1;
        }
	}

    for (int t = 0; t < tCAT + 1; ++t) { 
        f << t << '\t' << WordsWithNonZeros[t] << '\t' << WordsWithHeadSize[t] << '\n' << std::flush;
    }

    delete[] topics4w;
	delete[] WordsWithNonZeros;
    delete[] WordsWithHeadSize;;
}






// ******** далее старые тематичности каждого слова в коллекции + выводы всяких разных топов ********



void collection::Tscores_cntl(std::fstream &f, int i, int d) {
	f << "#NewData Tscores-i" << i << "-d" << d << '\n';
	qsort(corpus[d].Tscores, corpus[d].Nd, sizeof(double), cmpDown);
	for (int w = 0; w < corpus[d].Nd; ++w) {
		f << corpus[d].Tscores[w] << '\n'; // нет соответвия индексам
	}
	f << std::flush;
}



node* collection::BuildTopWordsList(int topic, int num) {
    int t = topic;
    node *list, *prev, *cur, *toadd;
    list = NULL;
	double minp = 0;
	int curNum = 0;
	// проход по словарю
	for (int w = 0; w < WOR; ++w) {
		if (curNum < num || phi[t][w] > minp) {
			if (curNum < num) {
				toadd = new node(w, phi[t][w], NULL);
				curNum++;
			} else {
				toadd = list;
				toadd->phiValue = phi[t][w];
				toadd->wordIdx = w;
				list = list->next;
			}

			prev = list; cur = list;
			while (cur && phi[t][w] > cur->phiValue) {
				prev = cur;
				cur = cur->next;
			}

			if (prev == cur && prev == list) { // добавление в начало списка
				toadd->next = prev? prev : NULL;
				list = toadd;
			} else {
				toadd->next = prev->next;
				prev->next = toadd;
			}

			minp = list->phiValue;
		}
	}
    return list;
}


void collection::topWordsOut(std::fstream &f, int num) { // выведем матрицу CAT на 100
	node *list, *prev, *cur;
	for (int t = 0; t < CAT; ++t) {
		list = BuildTopWordsList(t, num);
		
		// проход по списку и вывод слов, и удаление списка
		prev = list; cur = list;
		while (cur) {
			f << cur->wordIdx << ':' << cur->phiValue << ' ';
			prev = cur;
			cur = cur->next;
			delete prev;
		}
		f << '\n';
	}
	f << std::flush;
}


void model::coherence_estimation(std::fstream &f, int num) {
	node *list, *ptrLeft, *ptrRight, *ptrDeletion;
    int left, right;
    double coherence = 0, averageCoherence = 0;
	for (int t = 0; t < CAT - swCAT; ++t) {
        coherence = 0;
		list = training.BuildTopWordsList(t, num);
		
		// проход по списку, суммирование pmi, и удаление списка
		ptrLeft = list; ptrRight = list;
		while (ptrLeft) { // иду по списку
            left = ptrLeft->wordIdx;
            ptrRight = ptrLeft->next;
            while(ptrRight) {
                right = ptrRight->wordIdx;
                if (wordCooccurences[left][right] * (training.DOC + control.DOC) > wordOccurences[left] * wordOccurences[right]) {
                    coherence += log(wordCooccurences[left][right] * (training.DOC + control.DOC) / wordOccurences[left] / wordOccurences[right]);
                }
                ptrRight = ptrRight->next;
            }
			ptrDeletion = ptrLeft;
			ptrLeft = ptrLeft->next;
			delete ptrDeletion;
		}
		
        averageCoherence += coherence / (num*(num-1)/2);
	} // t
    averageCoherence /= (CAT - swCAT);
	f << '\t' << averageCoherence;
}


void collection::topDocsOut(std::fstream &f, int num) {
	// посчитаем длины документов
	int* docLens = new int [DOC];
	for (int d = 0; d < DOC; ++d) {
		docLens[d] = 0;
		for (int w = 0; w < corpus[d].Nd; ++w) {
			docLens[d] += corpus[d].numbs[w];
	    }
	}	

	IdProb* docs4t = new IdProb [DOC];
	for (int t = 0; t < CAT; ++t) { // для каждой темы по очереди
		double denom = 0;
		for (int d = 0; d < DOC; ++d) { // формируем массив документов
			docs4t[d].id = d;
			docs4t[d].prob = theta[d][t] * docLens[d]; // от нормировки отноения больше-меньше не изменятся. успеем.
			denom += docs4t[d].prob;
		}
		qsort(docs4t, DOC, sizeof(IdProb), cmpDownIdProb); // соритруем по вероятностям
		for (int i = 0; i < num; ++i) {
			f << docs4t[i].id << ':' << docs4t[i].prob / denom << ' ';
		}
		f << '\n';
	}

	delete[] docLens;
	delete[] docs4t;
}

void collection::topBackgroundOut(std::fstream &f, int num) {
	IdProb* dict = new IdProb [WOR];
	for (int w = 0; w < WOR; ++w) {
		dict[w].id = w;
		dict[w].prob = fo[w];
	}
	qsort(dict, WOR, sizeof(IdProb), cmpDownIdProb);
	for (int i = 0; i < num; ++i) {
		f << dict[i].id << ':' << dict[i].prob << ' ';
	}
	f << '\n';
	delete[] dict;
}

void collection::topNoiseOut(std::fstream &f, int num) {
	for (int d = 0; d < DOC; ++d) {
		IdProb* Twords = new IdProb[corpus[d].Nd];
		IdProb* Nwords = new IdProb[corpus[d].Nd];
		for (int w = 0; w < corpus[d].Nd; ++w) {
			Twords[w].id = corpus[d].words[w];
			Twords[w].prob = corpus[d].Tscores[w];
			Nwords[w].id = corpus[d].words[w];
			Nwords[w].prob = corpus[d].Nscores[w];
		}
		qsort(Twords, corpus[d].Nd, sizeof(IdProb), cmpDownIdProb);
		qsort(Nwords, corpus[d].Nd, sizeof(IdProb), cmpDownIdProb);
		for (int i = 0; i < num; ++i) {
			f << Twords[i].id << ':' << Twords[i].prob << ' ';
		}
		f << '\n' << '\n';
		for (int i = 0; i < num; ++i) {
			f << Nwords[i].id << ':' << Nwords[i].prob << ' ';
		}
		f << '\n' << '\n' << '\n';

		delete[] Twords;
		delete[] Nwords;
	}
}



//**********************старое*******************************
void collection::print_theta(const char* filename) {
	using namespace std;
	ofstream f(filename);
	for (int d = 0; d < DOC; ++d) {
		for (int t = 0; t < CAT; ++t) {
			f << theta[d][t];
			f << ' ';
		}
		f << '\n';
	}
	f.close();
}

void collection::print_phi(const char* filename) {
	if (!phi) // какой-то идиот вызвал для контрольной коллекции
		return; 
	using namespace std;
	ofstream f(filename);
	for (int t = 0; t < CAT; ++t) {
		for (int w = 0; w < WOR; ++w) {
			f << phi[t][w];
			f << ' ';
		}
		f << '\n';
	}
	f.close();
}

void collection::sumdt_cntl(const char* filename) {
	using namespace std;
	fstream f(filename, fstream::out | fstream::app);
	for (int t = 0; t < CAT; ++t) {
		double sum = 0;
		for (int d = 0; d < DOC; ++d) {
			sum += theta[d][t] * nd[d] / nt[t];
		}
		f << sum << ' ';
	}
	f << '\n';
	f.close();
}

void collection::sumtw_cntl(const char* filename) {
	using namespace std;
	fstream f(filename, fstream::out | fstream::app);
	for (int w = 0; w < WOR; ++w) {
		double nw = 0;
		double sum = 0;
		for (int t = 0; t < CAT; ++t) {
			nw += nwt[w][t];
			sum += phi[t][w] * nt[t];
		}
		f << (sum / nw) << ' ';
	}
	f << '\n';
	f.close();
}

