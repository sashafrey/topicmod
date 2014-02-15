#include "stdafx.h"
#include "types.h"

// итерация алгоритма производится с коллекцией, для которой вызван метод. 
// в парамтрах всегда передается указатель на обучающую phi и fo
// на первой итерации пользуемся фиксированными фи и тета.

void collection::one_gs_sp_iteration(bool robust, bool accumulated, double** t_phi, double* t_fo, double alpha, double beta, double gamma, double eps, double HdwtSparsity, t_piStep piStep) {
	long double Z = 0; // сумма распределения. p - глобальный массив для распределения
	long double Zdw = 0;
	for (int d = 0; d < DOC; ++d) { // документы
		long double S = 0; // сумма пи
		for (int w = 0; w < corpus[d].Nd; ++w) { // слова
			int ndw = corpus[d].numbs[w];
			int w_glob = corpus[d].words[w];
			if (ndw != 0) {
				Z = 0; Zdw = 0;
				for (int t = 0; t < CAT; ++t) { // нашли распределение для (d, w)
					if (accumulated) {
						theta[d][t] = (ntd[t][d] + alpha) / (nd[d] + CAT * alpha);
						if (nd[d] == 0) { // тематическая компонента ВСЕГО документа равна 0
							theta[d][t] = 1.0 / CAT; // размазываем по темам
							if (d != 49) {
								int someoneElse = 0;
							}
						}
						if (phi) { // обучение
							phi[t][corpus[d].words[w]] = (nwt[corpus[d].words[w]][t] + beta) / (nt[t] + WOR * beta);
							if (!(phi[t][corpus[d].words[w]] >= 0))
								int hello2 = 0;
							if (eps != 0) {
								fo[w_glob] = muwe[w_glob] / mu_sum;
								if (!(fo[w_glob] >= 0)) 
									int hello3 = 0;
							}
						}
					}
					if (phi) { // обучение
						p[t] = t_phi[t][w_glob] * theta[d][t];
					} else if (w_glob < WOR - NW) { // контроль, слово встречалось в обучении
						p[t] = t_phi[t][w_glob] * theta[d][t] * (1 - NW * phiNW[t]);
					} else { // контроль, слово новое
						p[t] = phiNW[t] * theta[d][t];
					}
					assert(p[t] >= 0);
					Zdw += p[t];
				}

				if (!robust && Zdw == 0) { // таблетка1
					for (int t = 0; t < CAT; ++t) {
						p[t] = theta[d][t];
						Zdw += p[t];
					}
				}

				for (int t = 1; t < CAT; ++t) { //cumsum
					p[t] += p[t - 1];
				}

				if (accumulated && gamma != 0) { // обнолвение pi_dw
					if (piStep == add) {
						pi[d][w] = max(0, (ndw / nude[d] - (Zdw + eps * (w_glob < WOR - NW ? t_fo[w_glob] : 0)) / gamma));
					} else {
						pi[d][w] = nu[d][w] / nude[d];
					}
					assert(pi[d][w] >= 0);	
					S += pi[d][w];
				}

				if (robust) {
					Z = Zdw + gamma * pi[d][w] + eps * (w_glob < WOR - NW ? t_fo[w_glob] : 0);
				} else {
					Z = Zdw;
				}
				assert (Z > 0);

				//******
				if (robust) {
					corpus[d].Tscores[w] = Zdw / Z; 
				}
				//******

				int sam_num = (HdwtSparsity == 0 ? ndw : HdwtSparsity > 0 ? min(ndw, HdwtSparsity) : -HdwtSparsity);				
				double one = ndw / double(sam_num);
				assert(one >= 0);

				double lastOne = corpus[d].lastOne[w];
				for (int g = 0; g < sam_num; ++g) { // сэмплируем сущности sam_num раз. sam_num <= ndw
					int topic, role;
					// вычитаем старые счетчики
					if (lastOne != 0) { // в инициализации положить все lastOne = 0 -> гарантия безопасности
						role = corpus[d].z[w][g];
						if (role < CAT) { // тема
							topic = role;
							if (nwt[w_glob][topic] != 0) { // не пострадал в боях разреживания
								nwt[w_glob][topic] -= lastOne;
								assert(nwt[w_glob][topic] > -MP);
								if (nwt[w_glob][topic] < 0) {
									nwt[w_glob][topic] = 0;
								}
								nt[topic] -= lastOne;
								assert(nt[topic] > -MP);
								if (nt[topic] < 0) {
									nt[topic] = 0;
								}
							}
							if (ntd[topic][d] != 0) { // пара тема-документ не пострадала в разреживании
								ntd[topic][d] -= lastOne;
								assert(ntd[topic][d] > -MP);
								if (ntd[topic][d] < 0) {
									ntd[topic][d] = 0;
								}
								nd[d] -= lastOne;
								assert(nd[d] > -MP);
								if (nd[d] < 0) {
									nd[d] = 0;
								}
							}
						} else if (role == CAT) { // шум
							nude[d] -= lastOne; nu_sum -= lastOne;
							assert(nude[d] > -MP); assert(nu_sum >= 0);
							if (nude[d] < 0) {
								nude[d] = 0;
							}
						} else if (phi) { // фон
							muwe[w_glob] -= lastOne; mu_sum -= lastOne;
							assert(muwe[w_glob] > -MP); assert(mu_sum >= 0);
							if (muwe[w_glob] < 0) {
								muwe[w_glob] = 0;
							}
						}
					}
					// первый уровень: сэмплируем роль
					double probRoles[3] = {Zdw, gamma * pi[d][w], eps * (w_glob < WOR - NW ? t_fo[w_glob] : 0)};
					for (int r = 1; r < 3; ++r) { //cumsum
						probRoles[r] += probRoles[r - 1];
					}
					role = multigen(probRoles, 3);
					if (d == 49) {
						int hello4 = 0;
					}
					if (role == 0) { // тема
						topic = multigen(p, CAT); // сгенерили тему
						corpus[d].z[w][g] = topic;	// запомнили тему
						// плюсуем счетчики
						nwt[w_glob][topic] += one;
						assert(nwt[w_glob][topic] >= 0);
						ntd[topic][d] += one;
						assert(ntd[topic][d] >= 0);
						nt[topic] += one;
						assert(nt[topic] >= 0);
						nd[d] += one;
						assert(nd[d] >= 0);
					} else if (role == 1) { // шум
						corpus[d].z[w][g] = CAT;
						nude[d] += one; nu_sum += one;
						assert(nude[d] >= 0);
					} else { // фон
						corpus[d].z[w][g] = CAT + 1;
						if (phi) {
							muwe[w_glob] += one; mu_sum += one;
							assert(muwe[w_glob] >= 0);
						}
					}
				}
				corpus[d].lastOne[w] = one;
			} // if закрылся
		} //слова
		
		// нормировать pi[d][w]
		if (accumulated && gamma != 0) {
			for (int w = 0; w < corpus[d].Nd; ++w) { // слова
				pi[d][w] = pi[d][w] / S;
				assert(pi[d][w] >= 0);
			}
		}
	} // документы
	
}
