#include "stdafx.h"
#include "types.h"

// �������� ��������� ������������ � ����������, ��� ������� ������ �����. 
// � ��������� ������ ���������� ��������� �� ��������� phi � fo
// �� ������ �������� ���������� �������������� �� � ����.

void collection::one_gs_sp_iteration(bool robust, bool accumulated, double** t_phi, double* t_fo, double alpha, double beta, double gamma, double eps, double HdwtSparsity, t_piStep piStep) {
	long double Z = 0; // ����� �������������. p - ���������� ������ ��� �������������
	long double Zdw = 0;
	for (int d = 0; d < DOC; ++d) { // ���������
		long double S = 0; // ����� ��
		for (int w = 0; w < corpus[d].Nd; ++w) { // �����
			int ndw = corpus[d].numbs[w];
			int w_glob = corpus[d].words[w];
			if (ndw != 0) {
				Z = 0; Zdw = 0;
				for (int t = 0; t < CAT; ++t) { // ����� ������������� ��� (d, w)
					if (accumulated) {
						theta[d][t] = (ntd[t][d] + alpha) / (nd[d] + CAT * alpha);
						if (nd[d] == 0) { // ������������ ���������� ����� ��������� ����� 0
							theta[d][t] = 1.0 / CAT; // ����������� �� �����
							if (d != 49) {
								int someoneElse = 0;
							}
						}
						if (phi) { // ��������
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
					if (phi) { // ��������
						p[t] = t_phi[t][w_glob] * theta[d][t];
					} else if (w_glob < WOR - NW) { // ��������, ����� ����������� � ��������
						p[t] = t_phi[t][w_glob] * theta[d][t] * (1 - NW * phiNW[t]);
					} else { // ��������, ����� �����
						p[t] = phiNW[t] * theta[d][t];
					}
					assert(p[t] >= 0);
					Zdw += p[t];
				}

				if (!robust && Zdw == 0) { // ��������1
					for (int t = 0; t < CAT; ++t) {
						p[t] = theta[d][t];
						Zdw += p[t];
					}
				}

				for (int t = 1; t < CAT; ++t) { //cumsum
					p[t] += p[t - 1];
				}

				if (accumulated && gamma != 0) { // ���������� pi_dw
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
				for (int g = 0; g < sam_num; ++g) { // ���������� �������� sam_num ���. sam_num <= ndw
					int topic, role;
					// �������� ������ ��������
					if (lastOne != 0) { // � ������������� �������� ��� lastOne = 0 -> �������� ������������
						role = corpus[d].z[w][g];
						if (role < CAT) { // ����
							topic = role;
							if (nwt[w_glob][topic] != 0) { // �� ��������� � ���� ������������
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
							if (ntd[topic][d] != 0) { // ���� ����-�������� �� ���������� � ������������
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
						} else if (role == CAT) { // ���
							nude[d] -= lastOne; nu_sum -= lastOne;
							assert(nude[d] > -MP); assert(nu_sum >= 0);
							if (nude[d] < 0) {
								nude[d] = 0;
							}
						} else if (phi) { // ���
							muwe[w_glob] -= lastOne; mu_sum -= lastOne;
							assert(muwe[w_glob] > -MP); assert(mu_sum >= 0);
							if (muwe[w_glob] < 0) {
								muwe[w_glob] = 0;
							}
						}
					}
					// ������ �������: ���������� ����
					double probRoles[3] = {Zdw, gamma * pi[d][w], eps * (w_glob < WOR - NW ? t_fo[w_glob] : 0)};
					for (int r = 1; r < 3; ++r) { //cumsum
						probRoles[r] += probRoles[r - 1];
					}
					role = multigen(probRoles, 3);
					if (d == 49) {
						int hello4 = 0;
					}
					if (role == 0) { // ����
						topic = multigen(p, CAT); // ��������� ����
						corpus[d].z[w][g] = topic;	// ��������� ����
						// ������� ��������
						nwt[w_glob][topic] += one;
						assert(nwt[w_glob][topic] >= 0);
						ntd[topic][d] += one;
						assert(ntd[topic][d] >= 0);
						nt[topic] += one;
						assert(nt[topic] >= 0);
						nd[d] += one;
						assert(nd[d] >= 0);
					} else if (role == 1) { // ���
						corpus[d].z[w][g] = CAT;
						nude[d] += one; nu_sum += one;
						assert(nude[d] >= 0);
					} else { // ���
						corpus[d].z[w][g] = CAT + 1;
						if (phi) {
							muwe[w_glob] += one; mu_sum += one;
							assert(muwe[w_glob] >= 0);
						}
					}
				}
				corpus[d].lastOne[w] = one;
			} // if ��������
		} //�����
		
		// ����������� pi[d][w]
		if (accumulated && gamma != 0) {
			for (int w = 0; w < corpus[d].Nd; ++w) { // �����
				pi[d][w] = pi[d][w] / S;
				assert(pi[d][w] >= 0);
			}
		}
	} // ���������
	
}
