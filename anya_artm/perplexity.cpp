#include "stdafx.h"
#include "types.h"


//**********************ѕ≈–ѕЋ≈ —»я*******************************
/*
// простой подсчет перплексии, использую параметры из переданной коллекции, phi, fo из обучени€
void model::pl_estimation(collection* col, std::fstream &f, int mode) {
	double sum = 0, Zdw = 0, loc_N = 0, pwd = 0;
	int badWordsCount = 0;
    for (int d = 0; d < col->DOC; ++d) {
		for (int w = 0; w < col->corpus[d].Nd; ++w) {
			int ndw = (mode == 0? col->corpus[d].numbs[w] : col->corpus[d].rest[w]);
			if (ndw != 0) {
				int w_glob = col->corpus[d].words[w];
				Zdw = 0;
				for (int t = 0; t < CAT; ++t) {
					if (mode == 0) { // обучение
						Zdw += training.phi[t][w_glob] * col->theta[d][t];
					} else if (w_glob < training.WOR) { // контроль, слово встречалось в обучении
						Zdw += training.phi[t][w_glob] * col->theta[d][t] * (1 - control.NW * control.phiNW[t]);
					} else { // контроль, слово новое
						Zdw += control.phiNW[t] * col->theta[d][t];
					}
				}
				pwd = Zdw;
				
				if (robust) {
					pwd = Zdw / (1 + gamma + eps) + gamma / (1 + gamma + eps) * col->pi[d][w] + eps / (1 + gamma + eps) * (w_glob < training.WOR? training.fo[w_glob] : 0); 
				}
				if (pwd == 0) {
					badWordsCount++;
					pwd = 1.0 / col->corpus[d].Nd;
				}
				sum += ndw * log (pwd);
				loc_N += ndw;
				assert(loc_N > 0);
			}
		}
    }

	if (mode == 1)
		f << '\t';
	f << exp(- sum / loc_N) << '\t' << badWordsCount;;
}
*/


double model::CalculateZdw(collection* col, int mode, int d, int w_glob) {
    double Zdw = 0;
    for (int t = 0; t < CAT; ++t) {
		if (mode == 0) { // обучение
			Zdw += training.phi[t][w_glob] * col->theta[d][t];
		} else if (w_glob < training.WOR) { // контроль, слово встречалось в обучении
			Zdw += training.phi[t][w_glob] * col->theta[d][t] * (1 - control.NW * control.phiNW[t]);
		} else { // контроль, слово новое
			Zdw += control.phiNW[t] * col->theta[d][t];
		}
	}
    return Zdw;
}


void model::perplexity_estimation(collection* col, std::fstream &f, int mode) {
	double topical_sum = 0, noise_sum = 0, back_sum = 0, noise_norm = 0, back_norm = 0, loc_N = 0, loc_TN = 0;
    for (int d = 0; d < col->DOC; ++d) {
		double loc_nd = 0, topical_nd = 0, nw_sum = 0;
		for (int w = 0; w < col->corpus[d].Nd; ++w) {
			int ndw = (mode == 0? col->corpus[d].numbs[w] : col->corpus[d].rest[w]);
			if (ndw != 0) {
				int w_glob = col->corpus[d].words[w];
				double Zdw = CalculateZdw(col, mode, d, w_glob);
				if (Zdw != 0) {
					topical_nd += ndw;
                    topical_sum += ndw * log(Zdw);
				} else {
                    assert(sparse || anticorrelated);
                    noise_sum += ndw * log(1.0*ndw);
                    back_sum += ndw * log(training.backDistribution[w_glob]);
                    nw_sum += training.backDistribution[w_glob];
                }
				loc_nd += ndw;
			}
		}
        // at the end of document
        if (sparse || anticorrelated) {
            noise_sum += (loc_nd - topical_nd) * log(1.0 / loc_nd);
            noise_norm += topical_nd * log(topical_nd / loc_nd);
            back_norm += topical_nd * log(1 - nw_sum);
            loc_TN += topical_nd;
        }
		loc_N += loc_nd;
    }

	if (mode == 1) {
		f << '\t';
    }
    if (sparse || anticorrelated) {
        f << exp(- (topical_sum + noise_sum + noise_norm) / loc_N) << '\t' << exp(- (topical_sum + back_sum + back_norm) / loc_N) << '\t' << (loc_N - loc_TN) / loc_N;
    } else {
	    f << exp(- topical_sum / loc_N);
    }
}


void model::perplexity_control_learning (std::fstream &f, int iTrain) {
	// нужно определить phi дл€ новых слов
	if (control.NW != 0 && !robust && beta != 0) {
		for (int t = 0; t < CAT; ++t) {
			control.phiNW[t] = beta / (training.nt[t] + control.WOR * beta);
		}
	} // else сделан один раз в инициализации (про дельту)

	switch (fun) {
		case S:
			control.one_gs_iteration(robust, iTrain > 0, training.phi, training.fo, alpha, beta, gamma, eps, HdwtSparsity, piStep);
			break;
		case P:
			control.one_plsa_iteration(robust, iTrain > 0, training.phi, training.fo, alpha, beta, gamma, eps, abs(HdwtSparsity) * (HdwtSparsity > 0 || iTrain > 10), piStep);
			break;
	}
    double sThetaLevel = 0;
	if (sparse && iTrain >= sStart && iTrain % sStep == 0) {
		switch (sFun) {
			case sRaws: 
                if (sparseDistr == sOld) {
                    sThetaLevel = sparse_raws_old(sRatioTheta, control.ntd, control.nd, CAT - swCAT, control.DOC);
                } else {
				    sThetaLevel = sparse_raws(sRatioTheta, training.ones, control.ntd, control.nd, CAT - swCAT, control.DOC);
                }
            	break;
			case sTails:
				sThetaLevel = sparse_tail(sRatioTheta, 0.15, control.ntd, control.nd, CAT - swCAT, control.DOC);
				break;
			case sMatrix:
				sThetaLevel = sparse_matrix(sRatioTheta, control.ntd, control.nd, CAT - swCAT, control.DOC);
				break;
        }
	} // настроили тета, ню, пи
    theta_update(&control);

	if (iTrain > 0 && gamma != 0) { // на нулевой итерации основные пи не трогались, не надо трогать и эти, все хорошо.
		for (int d = 0; d < control.DOC; ++d) {
			long double S = 0;
			for (int w = 0; w < control.corpus[d].Nd; ++w) {
				double ndw = control.corpus[d].rest[w];
				double ndwValid = control.corpus[d].numbs[w];
				if (ndw != 0 && ndwValid == 0) { // перерасчет только дл€ новых слов!
					long double Z = 0;
					int w_glob = control.corpus[d].words[w];
					for (int t = 0; t < CAT; ++t) { // нашли распределение дл€ (d, w)
						if (w_glob < control.WOR - control.NW) {
							control.p[t] = training.phi[t][w_glob] * control.theta[d][t] * (1 - control.NW * control.phiNW[t]);
						} else {
							control.p[t] = control.phiNW[t] * control.theta[d][t];
						}
						assert(control.p[t] >= 0);
						Z += control.p[t];
					}
					assert (Z >= 0);
					control.pi[d][w] = max(0, (ndw / control.nude[d] - (Z + eps * (w_glob < training.WOR? training.fo[w_glob] : 0)) / gamma));
				}
				S += control.pi[d][w];
			}
			for (int w = 0; w < control.corpus[d].Nd; ++w) { // нормировка контрольных pi_dw!
				control.pi[d][w] /= S; 
			}
		}
	}

	perplexity_estimation(&control, f, 1);
    if (sparse) {
        f << '\t' << sThetaLevel;
    }
}
