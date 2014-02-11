#include "stdafx.h"
#include "types.h"


void CalculateWordCooccurences(t_col col) {
	try { 
		// создаем модель
		model m(P, sUniform, 0, 0, 0, 0, uniSam, 0, add, sRaws, sOld, 0, 0, 0, 0, 0, 100, 0, 40, 0.5, 0.05, 0, 0, 0);

		// считываем коллекию
		std::string colName;
		switch (col) {
		case RuDisOld: 
			colName = "RuDisOld";
			break;
		case NIPSOld:
			colName = "NIPSOld";
			break;
		case RuDis:
			colName = "RuDis";
			break;
		case RuDisNWS:
			colName = "RuDisNWS";
			break;
		case NIPS:
			colName = "NIPS";
			break;
		case NIPSNWS:
			colName = "NIPSNWS";
			break;
		}
		m.training.read((colName + std::string("_t.txt")).data()); 
		m.control.read((colName + std::string("_c.txt")).data()); 
           
        //******for coherence************
        m.wordCooccurences = new double* [m.control.WOR];
        m.wordOccurences = new double [m.control.WOR];
        for (int u = 0; u < m.control.WOR; ++u) {
            m.wordCooccurences[u] = new double [m.control.WOR];
            for (int v = 0; v < m.control.WOR; ++v) {
                m.wordCooccurences[u][v] = 0;
            }
            m.wordOccurences[u] = 0;
        }
        m.CalculateWordOccurencesCooccurences(&(m.training));
        m.CalculateWordOccurencesCooccurences(&(m.control));
        //*******************************
        printf("printing...");
        m.PrintWordCooccurences((colName + std::string("_cooccurences.txt")).data());
	}
	catch (...) {
		printf("The cooccurences calculating failed.");
	}
    printf("finished");
}


void launch(t_col col, t_fun fun, double alpha, double beta, double gamma, double eps, t_init init,
	        double HdwtSparsity, t_piStep piStep, t_sFun sFun, int sStart, int sStep, double sRatioTheta, double sRatioPhi, 
            int CAT, int ITER, const char* comment, double delta,
            double tHeadSumRatio, double tHeadItemsRatio, t_sDistr sparseDistr, int swCAT, t_sDistr smoothDistr,
            int anticorrStart, double anticorrCoef, int anticorrInc) {

	char exName[1024];
	try { 
		// создаем модель
		model m(fun, smoothDistr, alpha, beta, gamma, eps, init, HdwtSparsity, piStep, 
                sFun, sparseDistr, sStart, sStep, sRatioTheta, sRatioPhi, delta, CAT, swCAT, ITER,
                tHeadSumRatio, tHeadItemsRatio, anticorrStart, anticorrCoef, anticorrInc);

		// считываем коллекию
		std::string colName;
		switch (col) {
		case RuDisOld: 
			colName = "RuDisOld";
			break;
		case NIPSOld:
			colName = "NIPSOld";
			break;
		case RuDis:
			colName = "RuDis";
			break;
		case RuDisNWS:
			colName = "RuDisNWS";
			break;
		case NIPS:
			colName = "NIPS";
			break;
		case NIPSNWS:
			colName = "NIPSNWS";
			break;
		}
		m.training.read((colName + std::string("_t.txt")).data()); 
		m.control.read((colName + std::string("_c.txt")).data()); 
		
        //******for coherence************
        m.wordCooccurences = new double* [m.control.WOR];
        m.wordOccurences = new double [m.control.WOR];
        for (int u = 0; u < m.control.WOR; ++u) {
            m.wordCooccurences[u] = new double [m.control.WOR];
            for (int v = 0; v < m.control.WOR; ++v) {
                m.wordCooccurences[u][v] = 0;
            }
            m.wordOccurences[u] = 0;
        }
        m.CalculateWordOccurencesCooccurences(&(m.training));
        m.CalculateWordOccurencesCooccurences(&(m.control));
        //*******************************

		// генерируем название файла выхода
		sprintf(exName, "%s-c%d-%d-i%d%s-a%g-b%g-%s-%s-%d-%d-%g-%g-ac-%d-%g%s-terms-%g-%g%s", 
						  colName.data(), CAT, swCAT, ITER,
                          smoothDistr == sBackTrain? "-BackTrain" : "", alpha, beta,
						  sFun == sRaws? "sRaws": sFun == sTails? "sTails": "sMatrix", 
                          sparseDistr == sOld? "sOld" : sparseDistr == sUniform? "sUniform": sparseDistr == sBackTrain? "sBackTrain" : "sBackLit",
                          sStart, sStep, sRatioTheta, sRatioPhi,
                          anticorrStart, anticorrCoef, anticorrInc == 1? "" : ("-" + std::to_string((long double)anticorrInc)).data(),
                          tHeadSumRatio, tHeadItemsRatio,
						  comment);

		// запускаем алгоритм построения модели
		m.main_proc(exName);
	}
	catch (...) {
		printf("The task failed: %s", exName);
	}
}


void model::main_proc(const char* name) { 
	double gammaFrom = 0, gammaTo = 0;
	int iterTo = -1;

	if (gamma < 0) { // сигнал о том, что хотим плавно наращивать гаммы
		gammaFrom = 0.001;
		gammaTo = -gamma;
	    iterTo = 20; 
		//gamma = min(gamma0 / (iter0 * iter0) * 0 * 0 + 0.001, gamma0);
		gamma = gammaFrom;
	}

	initialization(0.5);

	// открытие файла вывода
	std::fstream out_file((std::string("outputs\\") + std::string(name) + std::string(".txt")).data(), std::fstream::out | std::fstream::app);
	out_file << std::fixed << std::setprecision (7);
	if (sparse) {
        out_file << "#NewData t_perpNoise, t_perpBack, t_pwd0Counter, sPhiLevel, t_sThetaLevel, c_perpNoise, c_perpBack, c_pwd0Counter, c_sThetaLevel, phi_sp, t_theta_sp, c_theta_sp";
    } else if (anticorrelated) {
        out_file << "#NewData t_perpNoise, t_perpBack, t_pwd0Counter, c_perpNoise, c_perpBack, c_pwd0Counter, phi_sp, t_theta_sp, c_theta_sp";    
    }
    else {
        out_file << "#NewData t_perp, c_perp, phi_sp, t_theta_sp, c_theta_sp";
    }
    if (robust) {
		out_file << ", piw_sp, piw_sum, t_pidw_sp, t_pidw_sum, c_pidw_sp, c_pidw_sum, backword, t_noise, c_noise";
	}
    out_file << ", termsRatio, topicContrast, topicPurity, kernelSize, coherenceKernel, coherence10, coherence100\n";

    /*
	pl_estimation(&training,out_file, 0);
    if (sparse) {
        out_file << '\t' << 0.0 << '\t' << 0.0;
    }
	pl_estimation(&control, out_file, 1);
    if (sparse) {
        out_file << '\t' << 0.0;
    }
	sparse_and_sum_cntl(out_file);
	out_file << '\n' << std::flush;
    */

//	std::fstream Tscores_file((std::string("outputs\\") + std::string(name) + std::string("-Tscores.txt")).data(), std::fstream::out | std::fstream::app);
//	Tscores_file << std::fixed << std::setprecision (5);

//	std::fstream tops4tInit_file((std::string("outputs\\") + std::string(name) + std::string("-tops4tInit.txt")).data(), std::fstream::out | std::fstream::app);
//	training.topWordsOut(tops4tInit_file, 100);
//	tops4tInit_file << '\n' << '\n';
//	training.topDocsOut(tops4tInit_file, 100);

	std::fstream tops4tFinal_file((std::string("outputs\\") + std::string(name) + std::string("-tops4t.txt")).data(), std::fstream::out | std::fstream::app);
	
//	std::fstream BackgroundInit_file((std::string("outputs\\") + std::string(name) + std::string("-BackgroundInit.txt")).data(), std::fstream::out | std::fstream::app);
//	training.topBackgroundOut(BackgroundInit_file, training.WOR);
//	std::fstream BackgroundFinal_file((std::string("outputs\\") + std::string(name) + std::string("-BackgroundFinal.txt")).data(), std::fstream::out | std::fstream::app);

//	std::fstream NoiseFinal_file((std::string("outputs\\") + std::string(name) + std::string("-NoiseFinal.txt")).data(), std::fstream::out | std::fstream::app);

	//std::fstream gamma_file("gammaChanges.txt", std::fstream::out | std::fstream::app);
	printf("Performing iterations for\n %s\n", name);
	for (int i = 0; i < ITER; ++i) { // итерации
		printf("iteration %d ... \n", i);
		if (i <= iterTo) {
			gamma = gammaFrom + (gammaTo - gammaFrom) * i * i / (i * i + (iterTo - i) * (iterTo - i));
			//gamma_file << gamma << "\n";
		}
		switch (fun) {
			case S:
				training.one_gs_iteration(robust, i > 0, training.phi, training.fo, alpha, beta, gamma, eps, HdwtSparsity, piStep);
				break;
			case P:
				training.one_plsa_iteration(robust, i > 0, training.phi, training.fo, alpha, beta, gamma, eps, abs(HdwtSparsity) * (HdwtSparsity > 0 || i > 10), piStep);
				break;
		}
        training.pt_update(); // for -terms
        // sparsing
        double sPhiLevel = 0, sThetaLevel = 0;
		if (sparse && i >= sStart && i % sStep == 0) {
			switch (sFun) {
			case sRaws: 
                if (sparseDistr == sOld) {
                    sPhiLevel = sparse_raws_old(sRatioPhi, training.nwt, training.nt, training.WOR, CAT - swCAT);
			        sThetaLevel = sparse_raws_old(sRatioTheta, training.ntd, training.nd, CAT - swCAT, training.DOC);
                } else {
				    sPhiLevel = sparse_raws(sRatioPhi, 
                                            sparseDistr == sUniform? training.ones : training.backDistribution,
                                            training.nwt, training.nt, training.WOR, CAT - swCAT);
				    sThetaLevel = sparse_raws(sRatioTheta, training.ones, training.ntd, training.nd, CAT- swCAT, training.DOC);
                }
				break;
			case sTails:
				sPhiLevel = sparse_tail(sRatioPhi, 0.15, training.nwt, training.nt, training.WOR, CAT - swCAT);
				sThetaLevel = sparse_tail(sRatioTheta, 0.15, training.ntd, training.nd, CAT - swCAT, training.DOC); // очень странная
				break;
			case sMatrix:
				sPhiLevel = sparse_matrix(sRatioPhi, training.nwt, training.nt, training.WOR, CAT - swCAT);
				sThetaLevel = sparse_matrix(sRatioTheta, training.ntd, training.nd, CAT - swCAT, training.DOC);
				break;
			}
		}   
        phi_update(i);
		theta_update(&training);

        // quality estimation
		perplexity_estimation(&training, out_file, 0);
        if (sparse) {
            out_file << '\t' << sPhiLevel << '\t' << sThetaLevel;
        }
		perplexity_control_learning(out_file, i); // разрежаем контрольные тета по ходу дела
      	sparsity_estimation(out_file);
        if (i != ITER - 1) {
            contrast_purity_estimation(out_file, false);
        } else {
            vector< vector<IdProb> > kernelWords = contrast_purity_estimation(out_file, true);
            KernelWordsOut(tops4tFinal_file, kernelWords);
        }
        coherence_estimation(out_file, 10);
        coherence_estimation(out_file, 100);
		out_file << '\n' << std::flush;

//		if (i % 5 == 0) {
//			training.Tscores_cntl(Tscores_file, i, 0);
//			training.Tscores_cntl(Tscores_file, i, 200);
//			training.Tscores_cntl(Tscores_file, i, 400);
//			training.Tscores_cntl(Tscores_file, i, 800);
//			training.Tscores_cntl(Tscores_file, i, 1400);
//		}
	} // итерации

	// Закончили обучать модель. 

    // std::fstream hist_file((std::string("outputs\\") + std::string(name) + std::string("_histograms.txt")).data(), std::fstream::out | std::fstream::app);
	// hist_file << std::fixed << std::setprecision (7);
	out_file << "\n\n\n\n" << "#NewData topics, wordsWithNonZeros, WordsWithHeadSize\n" << std::flush;
    terms_sparsity_histogram(out_file); 
    // hist_file.close();

	
	training.topWordsOut(tops4tFinal_file, 100);
	tops4tFinal_file << '\n' << '\n';
	training.topDocsOut(tops4tFinal_file, 100);
//	training.topBackgroundOut(BackgroundFinal_file, training.WOR);
//	training.topNoiseOut(NoiseFinal_file, 500);
	// закрытие файлов
	out_file.close();
    tops4tFinal_file.close();
}



void model:: phi_update(int iteration) {
    double* distribution;  
    if (smoothDistr == sBackTrain) {
        distribution = training.backDistribution;
    } else { //  (smoothDistr == sUniform)
        distribution = training.ones;
    }
    // regularization
    for (int t = 0; t < CAT; ++t) {
        for (int w = 0; w < training.WOR; ++w) {
            // smoothing   
            if (beta != 0 && (swCAT == 0 || t >= CAT - swCAT)) {     
                training.nwt[w][t] += beta * distribution[w];
                training.nt[t] += beta * distribution[w];
            }
            // anticorr
            if (iteration > 0 && anticorrCoef != 0 && iteration > anticorrStart) {
                double coef = anticorrCoef * min(iteration - anticorrStart, anticorrInc) / anticorrInc;
                double toBeSubstracted = coef * training.phi[t][w] * (training.phiSum[w] - training.phi[t][w]);
                if (training.nwt[w][t] < toBeSubstracted) {
                    training.nt[t] -= training.nwt[w][t];
                    training.nwt[w][t] = 0;
                } else {
                    training.nwt[w][t] -= toBeSubstracted;
                    training.nt[t] -= toBeSubstracted;
                }
            }
          
        }
    }
    // normalization
    for (int w = 0; w < training.WOR; ++w) {
        training.phiSum[w] = 0;
    }
    for (int t = 0; t < CAT; ++t) {
        for (int w = 0; w < training.WOR; ++w) {
			training.phi[t][w] = training.nwt[w][t] / training.nt[t];
            training.phiSum[w] += training.phi[t][w];
        }
    }
}

void model::theta_update(collection* col) {
	for (int d = 0; d < col->DOC; ++d) {
		for (int t = 0; t < CAT; ++t) {
			col->theta[d][t] = (col->ntd[t][d] + alpha * (t >= CAT - swCAT)) / (col->nd[d] + swCAT * alpha);
		}
	}
}




void collection:: pt_update() {
	for (int t = 0; t < CAT; ++t) {
		pt[t] = nt[t];
    }
}



//*****************************************************************************


// копим шум
void collection::noise_count(long double Z, double gamma, int d, int w) {
	int ndw = corpus[d].numbs[w];
	
	nude[d] -= nu[d][w]; nu_sum -= nu[d][w];
	assert(nude[d] > -MP); assert(nu_sum >= 0);
	if (nude[d] < 0) {
		nude[d] = 0;
	}
		
	nu[d][w] = ndw * gamma * pi[d][w] / Z;
	assert(nu[d][w] >= 0);

	nude[d] += nu[d][w]; nu_sum += nu[d][w];
	assert(nude[d] >= 0);
}

// копим фон
void collection::back_count(long double Z, double eps, int d, int w) { // функция вызывается только для обучения
	int w_glob = corpus[d].words[w];
	int ndw = corpus[d].numbs[w];
	
	muwe[w_glob] -= mu[d][w]; mu_sum -= mu[d][w];
	assert(muwe[w_glob] > -MP); assert(mu_sum >= 0);
	if (muwe[w_glob] < 0) {
		muwe[w_glob] = 0;
	}
		
	mu[d][w] = ndw * eps * fo[w_glob] / Z;
	assert(mu[d][w] >= 0);

	muwe[w_glob] += mu[d][w]; mu_sum += mu[d][w];
	assert(muwe[w_glob] >= 0);
}

//**************************************************************************
	// ДЛЯ ВЛАДЫ ЦЕЛЫХ
	/*
	//std::fstream sizes_file("sizes.txt", std::fstream::out | std::fstream::app);
	//std::fstream nd_file("nd.txt", std::fstream::out | std::fstream::app);
	std::fstream nt_file("plsa-nt.txt", std::fstream::out | std::fstream::app);
	std::fstream nwt_file("plsa-nwt.txt", std::fstream::out | std::fstream::app);
	std::fstream ntd_file("plsa-ntd.txt", std::fstream::out | std::fstream::app);
	std::fstream ndwt_file("plsa-ndwt.txt", std::fstream::out | std::fstream::app);

	//sizes_file << "Число документов DOC = " << training.DOC << std::endl <<
	//			   "Число тем CAT = " << training.CAT << std::endl <<
	//			   "Число слов в словаре WOR = " << training.WOR << std::endl <<
	//			   "Число различных слов в каждом документе Nd (одномерный массив на DOC):" << std::endl;
	//nd_file << "Счетчики n_d (одномерный массив на DOC)" << std::endl;
	nt_file << "Счетчики n_t (одномерный массив на CAT)" << std::endl;
	nwt_file << "Счетчики n_wt (двумерный массив WOR на CAT)" << std::endl;
	ntd_file << "Счетчики n_td (двумерный массив CAT на DOC)" << std::endl;
	ndwt_file << "Счетчики n_dwt (для каждого документа d указаны " << '\n' << "айдишник каждого встречающегося слова w и с новой строки соответсвующий ему список пар (тема t:счетчик n_dwt))" << std::endl;
 	for (int d = 0; d < training.DOC; ++d) {
	//	sizes_file << training.corpus[d].Nd << ' ';
	//	nd_file << training.nd[d] << ' ';

		for (int w = 0; w < training.corpus[d].Nd; ++w) {
			ndwt_file << training.corpus[d].words[w] << std::endl;
			node* prev = NULL;
			node* curr = training.corpus[d].z[w];
			while (curr) { 
				ndwt_file << curr->topic << ":" << curr->count << ' ';
				prev = curr;
				curr = curr->next;
			}		
			ndwt_file << std::endl;
		}
		ndwt_file << std::endl;
	}

	for (int t = 0; t < CAT; ++t) {
		nt_file << training.nt[t] << ' ';
		for (int d = 0; d < training.DOC; ++d) {
			ntd_file << training.ntd[t][d] << ' ';
		}
		ntd_file << std::endl;
	}

	for (int w = 0; w < training.WOR; ++w) {
		for (int t = 0; t < training.CAT; ++t) {
			nwt_file << training.nwt[w][t] << ' ';
		}
		nwt_file << std::endl;
	}


//	sizes_file.close();
//	nd_file.close();
	nt_file.close();
	nwt_file.close();
	ntd_file.close();
	ndwt_file.close();
	*/
	//*********************************************