// BasicVersionEx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "types.h"

// ��������� �� ������:
// initialization.cpp - ������������ � ����������� �������, ������ �� ������� ������, ��������� �������� �� ���������, ������������� ��������� � ������
// gs.cpp - one_gs_iteration
// plsa.cpp - one_pls_iteration
// perplexity.cpp - ������� ���������� �� �������, ����� ������� �� ������� (��������� �� ����� �������� + ������� �� ������)
// outputs.cpp - ��� ������ � ����� (����, ��, ������������� ��������, ...)
// types.cpp - main_proc (����������� ���� ������������ ���������), noise_count, back_count (��������� ��������� ��������� - ���������� �� one_***_iteration).
// sparsers.cpp - ������������

// ��������� ������� launch:
// ��������� (RuDisOld, RuDisOldNW, NIPSOld, RuDis, RuDisNWS, NIPS, NIPSNWS), alpha, beta, gamma, epsilon, �������������(uniSam, rnd), 
// ������������� p(t|dw) (GS: 0->ndw �������, ����� min(x, ndw). PLSA: 0->�������, ����� ��������� ���� x ��������� �������� p[t]), 
// ��� ���������� pidw (add, mult), 
// ��������� ������������� ������������� start, step, ratio, ����� ���, ����� ��������, 
// �����������, ������� ������ � ����� �������� ����� � �������,
// �������� ��� ��������� ����� ���� � ������������������ ���������� delta
// (��� delta = 0 ������������ ��������� ����������� �������� delta0,
// �������� ����� ����� �� �������� �� ���������. � ��������� ���������� phi_wt ��� ����� ���� ����� 0)

//*********************************************


void algs_8(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01, t_init ini=uniSam) {
// ����������� � S/P, D/��� D, R/��� R
	launch(col, S, 0,   0,    0,	0,   ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  0,	0,   ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, 0,   0,    gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, 0,   0,    0,	0,	 ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  0,	0,	 ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, 0,   0,    gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void algs_4_nonRobust(t_col col, double alp=0.5, double bet=0.01, t_init ini=uniSam) {
// ����������� � S/P, D/��� D, R/��� R
	launch(col, S, 0,   0,    0,	0,   ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  0,	0,   ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, 0,   0,    0,	0,	 ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  0,	0,	 ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void algs_8_robust(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01, t_init ini=uniSam) {
// ����������� � S/P, D/��� D, R/��� R
	launch(col, S, 0,   0,    gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, 0,   0,    gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


void P_vs_SD(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) {
// ����������� � ����� ����� ���� � �������������
	launch(col, P, 0,   0,    gam, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void mult_step(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { //2h
// ����������� � ����������������� �����
	launch(col, P, 0,   0,    gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, 0,   0,    gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void sampling_min(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// ����������� � ������ ������������� � SD, � SDR
	launch(col, S, alp, bet,  gam, eps, uniSam, 1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 4,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 10, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void sampling_fix(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// ����������� � ������ ������������� � SD, � SDR
	launch(col, S, alp, bet,  gam, eps, uniSam, -1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -4,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -10, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


// �������� ��������� s = 100 ��� ���������� ���� P ��������������� �� ����� "�������� � Hdwt ndw ������������ ���������".
// ��� ������ ������, ���� ���� �����������!

void PLSA_fixtails_start0(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// ����� ����������� � ��������
	launch(col, P, alp, bet,  gam, eps, uniSam, 100,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix"); // � ���� plsa 100
	launch(col, P, alp, bet,  gam, eps, uniSam, 1,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 2,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 3,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 4,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 5,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 10, add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
}

void PLSA_fixtails_start10(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// ����� ����������� � ��������
	launch(col, P, alp, bet,  gam, eps, uniSam, -100,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, -1,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, -2,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -3,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -4,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -5,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -10, add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
}


void PLSA_tails_start0(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // many
// ����������� � ������������� Hdwt � PLSA
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.05, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void PLSA_tails_start10(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // many
// ����������� � ������������� Hdwt � PLSA
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.05, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.6,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.7,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void SDR_gamma_opt(t_col col, double alp=0.5, double bet=0.01, double eps=0.01) { // 6h
// ����������� � �������� gamma
	launch(col, S, alp, bet, 0,     eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.001, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.01,  eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.05,  eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.1,   eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.3,   eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.5,   eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


void SDR_eps_opt(t_col col, double alp=0.5, double bet=0.01, double gam=0.01) { // 2h
// ����������� � �������� eps
	launch(col, S, alp, bet, gam, 0,    uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.01, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.05, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.1,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.2,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.3,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.5,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


void ExtraSparsing(t_col col, double gam=0.01, double eps=0.01) { // 3h
// �������������� ������������ P(R) � S(R)
	launch(col, P, 0, 0, gam, eps, uniSam, 0, add, sRaws, 15, 2, 0.1,  0.1,  100, 40, "");
	launch(col, P, 0, 0, gam, eps, uniSam, 0, add, sRaws, 10, 2, 0.15, 0.15, 100, 40, "");
	launch(col, P, 0, 0, gam, eps, uniSam, 0, add, sRaws, 10, 2, 0.2,  0.2,  100, 40, "");

	launch(col, P, 0, 0, gam, eps, uniSam, 0, add, sRaws, 15, 1, 0.15,  0.15,  100, 40, "");
	launch(col, P, 0, 0, gam, eps, uniSam, 0, add, sRaws, 1, 2, 0.1, 0.1, 100, 40, "");
	launch(col, P, 0, 0, gam, eps, uniSam, 0, add, sTails, 15, 2, 0.1,  0.001,  100, 40, "");

	launch(col, S, 0, 0, gam, eps, uniSam, 0, add, sRaws, 15, 2, 0.05, 0.05, 100, 40, "");
	launch(col, S, 0, 0, gam, eps, uniSam, 0, add, sRaws, 15, 2, 0.1,  0.1,  100, 40, "");
	launch(col, S, 0, 0, gam, eps, uniSam, 0, add, sRaws, 10, 2, 0.15, 0.15, 100, 40, "");
}

void betaOpt(t_col col, t_fun f) {
// ����������� � �������� beta � PD ��� SD
	launch(col, f, 0.5, 0,       0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.00001, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.0001,  0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.001,   0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.01,    0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.1,     0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.2,     0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 0.3,     0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
	launch(col, f, 0.5, 1,       0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "");
}

void deltaOpt(t_col col) {
// ����������� � �������� delta ��� ����� ���� � P
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.001); 
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.0001); 
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.00001); 
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.000001);
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.0000001); 
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.00000001); 
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.000000001);
	launch(col, P, 0, 0, 0, 0, uniSam, 0, add, sRaws, 0, 0, 0, 0,  100, 40, "",  0.0000000001); 
}

//***************************************************************************************


void TestJanuary2013(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) {
// C����� ������������� ��� ������� ������, ������ 2013:
	// 8 �������� - ����� �������������, ��� ��������� S/P, D/��� D, R/��� R
	algs_8(col, alp, bet, gam, eps, uniSam);

	// 8 �������� - ��������� �������������, ��� ��������� S/P, D/��� D, R/��� R 
	algs_8(col, alp, bet, gam, eps, rnd);

	// 4 ������� - ��������� � ������������������ ������, ��� ��������� S/P, D/��� D
	mult_step(col, alp, bet, gam, eps);

	// 6 �������� - ������ ����� ������������� ��� SD
	sampling_min(col, alp, bet, 0, 0);

	// 6 �������� - ������ ����� ������������� ��� SDR
	sampling_min(col, alp, bet, gam, eps);

	// 5 �������� - ������ ��������� ������������ Hdwt � P
	PLSA_tails_start0(col, 0, 0, 0, 0);

	// 5 �������� - ������ ��������� ������������ Hdwt � PR
	PLSA_tails_start0(col, 0, 0, gam, eps);

	// 8 �������� - ����������� gamma � SDR
	SDR_gamma_opt(col, alp, bet, eps);

	// 6 �������� - ����������� eps � SDR
	SDR_eps_opt(col, alp, bet, gam);

	// �������������� ������������ PR � SR
	ExtraSparsing(col, gam, eps);

	// �������������� ������������ P � S
	ExtraSparsing(col, 0, 0);
}


void TestJanuary2013NewWords(t_col col, t_col colNW, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) {
	// ���������� ������ ���� �������� ���������� �� ���� ����������
	algs_8(col, alp, bet, gam, eps, uniSam);
	algs_8(colNW, alp, bet, gam, eps, uniSam);

	// ����������� ��������� delta ��� ������ � ������ ������� � P
	deltaOpt(colNW);

	// ����������� ��������� beta, ������������ � ��������� ����� ���� � PD � SD �� ���� ����������
	betaOpt(col, P);	
	betaOpt(col, S);
	betaOpt(colNW, P);
	betaOpt(colNW, S);	
}

// � ����� ���� ������������ � Tscores. � ���� ������������ � Top-��������.


void TestApril2013(t_col col, double alp=0.5, double bet=0.01, double gam=-0.01, double eps=0.01) { // ������ �� ��������� ����� ����������
// ������������� �������������.
	// 8 �������� - ����� �������������, ��� ��������� S/P, D/��� D, R/��� R
	algs_8(col, alp, bet, gam, eps, uniSam);

	// 4 �������� - ��������� �������������, ��������� S/P, D/��� D
	algs_4_nonRobust(col, alp, bet, rnd);

	// 4 ������� - ��������� � ������������������ ������, ��� ��������� S/P, D/��� D
	mult_step(col, alp, bet, gam, eps);

	// 12 �������� - ������ ��������� � ����� ������������� ��� SD
	sampling_fix(col, alp, bet, 0, 0);
	sampling_min(col, alp, bet, 0, 0);

	// 12 �������� - ������ ��������� � ����� ������������� ��� SDR
	sampling_fix(col, alp, bet, gam, eps);
	sampling_min(col, alp, bet, gam, eps);

	// 12 �������� - ������ ��������� ������������ Hdwt � P
	PLSA_tails_start0(col, 0, 0, 0, 0);
	PLSA_tails_start10(col, 0, 0, 0, 0);

	// 12 �������� - ������ ��������� ������������ Hdwt � PR
	PLSA_tails_start0(col, 0, 0, gam, eps);
	PLSA_tails_start10(col, 0, 0, gam, eps);

	// 9 �������� - ����������� gamma � SDR
	SDR_gamma_opt(col, alp, bet, eps);

	// 7 �������� - ����������� eps � SDR
	SDR_eps_opt(col, alp, bet, gam);

	// �������������� ������������ PR � SR
	ExtraSparsing(col, gam, eps);

	// �������������� ������������ P � S
	ExtraSparsing(col, 0, 0);
	
}


//***************************************************


void LaunchDecember2013(t_col col, int CAT, int swCAT, int ITER,
                         t_sDistr smoothDistr, double alpha, double beta, 
                         t_sFun sFun, t_sDistr sparseDistr, int sStart, int sStep, double sRatioTheta, double sRatioPhi, 
                         int anticorrStart, double anticorrCoef, int anticorrInc,
                         double tHeadSumRatio, double tHeadItemsRatio,
                         const char* comment) {


    launch(col, P, alpha, beta, 0, 0, uniSam, 0, add, sFun, sStart, sStep, sRatioTheta, sRatioPhi, CAT, ITER, comment, 0, 
        tHeadSumRatio, tHeadItemsRatio, sparseDistr, swCAT, smoothDistr, anticorrStart, anticorrCoef, anticorrInc);
}

void LaunchSparsingAndSmoothing(int swCAT, t_sDistr smoothDistr, double alpha, double beta, t_sDistr sparseDistr) {
//    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 0,  0,    0,    0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.05, 0.05, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1,  0.1,  0.1, 0, 0, 1, 0.5, 0.02, "");

    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0.1,  0.15,  0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1,  0.05,  0.1,  0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 1,  1,  0.05,  0.05, 0, 0, 1, 0.5, 0.02, "");

    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0.1,  0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.15, 0.15, 0, 0, 1, 0.5, 0.02, "");
}

void LaunchSparsingAndSmoothingNoTheta(int swCAT, t_sDistr smoothDistr, double alpha, double beta, t_sDistr sparseDistr) {
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 0,  0,    0,    0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0, 0.05, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1,  0,  0.1, 0, 0, 1, 0.5, 0.02, "");

    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0,  0.15,  0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1,  0,  0.1,  0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 1,  1,  0,  0.05, 0, 0, 1, 0.5, 0.02, "");

    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0,  0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1,  0, 0.15, 0, 0, 1, 0.5, 0.02, "");
}

void LaunchSparsingAndSmoothingNoPhi(int swCAT, t_sDistr smoothDistr, double alpha, double beta, t_sDistr sparseDistr) {
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.05,  0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.06,  0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.07,  0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.08,  0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1, 0.09,  0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10, 1,  0.1,  0, 0, 0, 1, 0.5, 0.02, "");

    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0.1,  0,  0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0.12,  0,  0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15, 1,  0.15,  0,  0, 0, 1, 0.5, 0.02, "");

   // LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 1,  1,  0.05,  0, 0, 0, 1, 0.5, 0.02, "");
}


void LaunchSparsingAndSmoothingExtra(int swCAT, t_sDistr smoothDistr, double alpha, double beta, t_sDistr sparseDistr) {
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10,  1,  0.05,  0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10,  1,  0.1,  0.05, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10,  1,  0.08,  0.08, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10,  1,  0.08,  0.1, 0, 0, 1, 0.5, 0.02, "");
 
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15,  1,  0.1,  0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15,  1,  0.9,  0.9, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15,  1,  0.1,  0.08, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15,  1,  0.08, 0.1, 0, 0, 1, 0.5, 0.02, "");
}
void LaunchSparsingAndSmoothingExtraSmall(int swCAT, t_sDistr smoothDistr, double alpha, double beta, t_sDistr sparseDistr) {
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10,  1,  0.08,  0.08, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 10,  1,  0.08,  0.1, 0, 0, 1, 0.5, 0.02, "");
 
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15,  1,  0.1,  0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, swCAT, 40, smoothDistr, alpha, beta, sRaws, sparseDistr, 15,  1,  0.9,  0.9, 0, 0, 1, 0.5, 0.02, "");
}

void LaunchSparsing(t_sDistr sparseDistr) {
    LaunchSparsingAndSmoothing(0, sUniform, 0, 0, sparseDistr);
}


void SparsingSmoothingCombinations() {
    // 3 ���� ������������
    LaunchSparsing(sOld);
    LaunchSparsing(sUniform);
    LaunchSparsing(sBackTrain);

    // ������ � ������������� � ������������

    // ������������ ������������
    LaunchSparsingAndSmoothing(10, sUniform, 0.5, 0.01, sOld);
    LaunchSparsingAndSmoothing(10, sUniform, 0.5, 0.01, sUniform);
    LaunchSparsingAndSmoothing(10, sUniform, 0.5, 0.01, sBackTrain);

    // ������ ����� ������� ��� 
    LaunchSparsingAndSmoothing(1, sUniform, 0.5, 0.01, sOld);
    LaunchSparsingAndSmoothing(1, sUniform, 0.5, 0.01, sBackTrain);

     // ������ �����������
    LaunchSparsingAndSmoothing(10, sBackTrain, 0.5, 100, sOld);
    LaunchSparsingAndSmoothing(10, sBackTrain, 0.5, 100, sBackTrain);

    LaunchSparsingAndSmoothing(1, sBackTrain, 0.5, 100, sOld);
    LaunchSparsingAndSmoothing(1, sBackTrain, 0.5, 100, sBackTrain);

    LaunchSparsingAndSmoothing(1, sUniform, 0.5, 0.01, sUniform);
    LaunchSparsingAndSmoothing(10, sBackTrain, 0.5, 100, sUniform);
    LaunchSparsingAndSmoothing(1, sBackTrain, 0.5, 100, sUniform);

}


void ToArticle7() {
    LaunchDecember2013(NIPSOld, 100, 0, 40, sUniform, 0, 0, sRaws, sUniform, 0, 0, 0, 0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, 0, 40, sUniform, 0.5, 0.01, sRaws, sUniform, 0, 0, 0, 0, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0, 0, sRaws, sUniform, 10, 1, 0.08, 0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0.8, 0.1, sRaws, sUniform, 0, 0, 0, 0, 10, 100000, 10, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0.8, 0.1, sRaws, sBackTrain, 10, 1, 0.08, 0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0.8, 0.1, sRaws, sUniform, 10, 1, 0.08, 0.1, 0, 0, 1, 0.5, 0.02, "");
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0.8, 0.1, sRaws, sUniform, 10, 1, 0.08, 0.1, 10, 100000, 10, 0.5, 0.02, "");
}


// gs �� ��������, NW ���� ���� ��, ����������� ��� �����.
int main(int argc, char* argv[])
{
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0.8, 0.1, sRaws, sUniform, 10, 1, 0.08, 0.1, 10, 100000, 10, 0.5, 0.02, "");

    // ToArticle7();

   	printf("Everything is completed");
	for(;;);
	return 0;
}

