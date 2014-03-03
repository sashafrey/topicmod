#include "stdafx.h"
#include "types.h"

// �������� ��������� ������������ � ����������, ��� ������� ������ �����. 
// � ��������� ������ ���������� ��������� �� ��������� phi � fo

void collection::one_plsa_iteration(bool robust, bool accumulated, double** t_phi, double* t_fo, double alpha, double beta, double gamma, double eps, double HdwtSparsity, t_piStep piStep) {
	for (int t = 0; t < CAT; ++t) { //��� �������� ��������
		nt[t] = 0;
		for (int d = 0; d < DOC; ++d) {
			ntd[t][d] = 0;
			nd[d] = 0;
		}
		for (int w = 0; w < WOR; ++w) {
			nwt[w][t] = 0;
		}
	}
	for (int d = 0; d < DOC; ++d) { // ���������
		long double S = 0; // ��� ��
		for (int w = 0; w < corpus[d].Nd; ++w) { // �����
			int ndw = corpus[d].numbs[w];
			if (ndw != 0) {
				long double Z = 0;
				long double Zdw = 0;
				int w_glob = corpus[d].words[w];
				for (int t = 0; t < CAT; ++t) {
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
										
				if (robust) {
					Z = Zdw + gamma * pi[d][w] + (w_glob < WOR - NW ? eps * t_fo[w_glob] : 0);
					assert(Z > 0);
				} else { // if (AssignmentToNoise) {
					Z = Zdw;
			//	} else {
			//		if (Zdw == 0 ) { // ��������1
			//			for (int t = 0; t < CAT; ++t) {
			//				p[t] = theta[d][t];
			//				Zdw += p[t];
			//			}
			//		}
			//		Z = Zdw;
			//		assert(Z > 0);
				}
			
				if (Zdw != 0 && HdwtSparsity != 0) {
					sparseHdwt(HdwtSparsity == 100? ndw: HdwtSparsity, Zdw); // ����� ��������� ������ p[t].
				}

				//******
				if (robust) {
					corpus[d].Tscores[w] = Zdw / Z;
					corpus[d].Nscores[w] = gamma * pi[d][w] / Z;
				}
				//******
				
				if (Z > 0) {
					for (int t = 0; t < CAT; ++t) {
						p[t] *= ndw / Z; // ����������

						nwt[w_glob][t] += p[t];
						nt[t] += p[t];
						ntd[t][d] += p[t];
						nd[d] += p[t];
					}
				}
								
				if (robust) {
					noise_count(Z, gamma, d, w);
					if (phi) { // ��������
						back_count(Z, eps, d, w);
					}
					if (accumulated && gamma != 0) { // pi[d][w] update
						if (piStep == add) {
							pi[d][w] = max(0, (ndw / nude[d] - (Zdw + eps * (w_glob < WOR - NW ? t_fo[w_glob] : 0)) / gamma));
						} else {
							pi[d][w] = nu[d][w] / nude[d];
						}
					
						assert(pi[d][w] >= 0);	
						S += pi[d][w];
					}
				}
			} // if ����������
		} //�����
		// ����������� pi[d][w]
		if (accumulated && gamma != 0) {
			for (int w = 0; w < corpus[d].Nd; ++w) { // �����
				pi[d][w] = pi[d][w] / S;
				assert(pi[d][w] >= 0);
			}
		}
	} // ���������
/*
	for (int d = 0; d < DOC; ++d) { //theta
		for (int t = 0; t < CAT; ++t) {
			theta[d][t] = (ntd[t][d] + alpha) / (nd[d] + CAT * alpha);
		}
	}
*/	
    if (phi) { // ��������
    /*
		for (int t = 0; t < CAT; ++t) { //phi � ���		
			for (int w = 0; w < WOR; ++w) {
				phi[t][w] = (nwt[w][t] + beta) / (nt[t] + WOR * beta);
			}	
        }
    */
		if (eps != 0) {
			for (int w = 0; w < WOR; ++w) {
				fo[w] = muwe[w] / mu_sum;
			}
		}
	}
}







//********************************** � ����� ��������� �� ��������� *************************************************
/*
//PLSA � ���������� ������� �������������
void collection::one_gem_iteration(double** t_phi,  double* alpha, double alpha0, double beta, double dolya, int shtuki) { // ��� �������������, �������� ������ ���������� ����������
	// it's adequate to choose dolya = 0.9, shtuki = 12
	node *prev, *curr;
	for (int d = 0; d < DOC; ++d) { // ���������
		if (d % 200 == 0)
			printf("%d\n", d);
		for (int w = 0; w < corpus[d].Nd; ++w) { // �����
			int ndw = corpus[d].numbs[w];
			double S = 0;
			for (int t = 0; t < CAT; ++t) {
					S += t_phi[t][corpus[d].words[w]] * theta[d][t];
			}
			// �������� �������� ������� ������, ������� ���
			curr = corpus[d].z[w];
			corpus[d].z[w] = NULL;
			while (curr) { //����� �������� ������� ������, ������� ���
				nwt[corpus[d].words[w]][curr->topic] -= curr->count;
				ntd[curr->topic][d] -= curr->count;
				nt[curr->topic] -= curr->count;
				prev = curr;
				curr = curr->next;
				delete prev;
			}			
			double sum = 0, count = 0;
			int len = 0;
			// ������� ����� �������� � ��������� ���������
			for (int topic = 0; topic < CAT; ++topic) {
				curr = corpus[d].z[w]; // � ������
				count = t_phi[topic][corpus[d].words[w]] * theta[d][topic];
				while (curr && count > curr->count) { // ������������ �� ������ ������
					prev = curr;
					curr = curr->next;
				}
				if (curr != corpus[d].z[w]) { // ���� ������������
					prev->next = new node(topic, count, curr); // ���������
					sum += count;
					len++;
					if (len > shtuki) { // ���� ������ ������� �������
						prev = corpus[d].z[w]; // ����������� �������
						corpus[d].z[w] = prev->next;
						sum -= prev->count;
						len--;
						delete prev;
					}
				} else if (topic < shtuki) {
					corpus[d].z[w] = new node(topic, count, corpus[d].z[w]);
					sum += count;
					len++;
				}
				curr = corpus[d].z[w]; // � ������
				while (curr && sum - curr->count > dolya * S) { // ������ ������ ����������
					sum -= curr->count;
					len--;
					prev = curr;
					curr = curr->next;
					delete prev;
				}
				corpus[d].z[w] = curr;
				if (corpus[d].z[w]->count + sum > S) {
					break;
				}
			}
			//if (i % 4 == 0 && d % 400 == 0 && (w == 5 || w == 48 || w == 200)) {
			//	printf("len = %d for i %d for d %d for w %d \n", len, i, d, w);
			//}

			while (curr) { // ���������
				curr->count *= ndw / sum;
				nwt[corpus[d].words[w]][curr->topic] += curr->count;
				ntd[curr->topic][d] += curr->count;
				nt[curr->topic] += curr->count;
				curr = curr->next;
			}
			
			// ���������� ����� (d,w). ����� �������� �� ���������� ����.
			for (int t = 0; t < CAT; ++t) {
				if (phi) {
					phi[t][corpus[d].words[w]] = (nwt[corpus[d].words[w]][t] + beta) / (nt[t] + WORinTrain * beta);
				}
				theta[d][t] = (ntd[t][d] + alpha[t]) / (nd[d] + alpha0);
			}
			
		} // �����
	} // ���������
}
*/
