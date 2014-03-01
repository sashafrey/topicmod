// BasicVersionEx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "types.h"

// Разбиение по файлам:
// initialization.cpp - конструкторы и деструкторы классов, чтение их входных файлов, разбиение контроля на половинки, инициализайия коллекций и модели
// gs.cpp - one_gs_iteration
// plsa.cpp - one_pls_iteration
// perplexity.cpp - подсчет перплексии по формуле, умный подсчет на контрое (настройка на одной половине + подсчет на другой)
// outputs.cpp - все выводы в файлы (теты, фи, разреженности профилей, ...)
// types.cpp - main_proc (руководство всем итерационным процессом), noise_count, back_count (изменения робастных компонент - вызывается из one_***_iteration).
// sparsers.cpp - разреживание

// Параметры функции launch:
// коллекция (RuDisOld, RuDisOldNW, NIPSOld, RuDis, RuDisNWS, NIPS, NIPSNWS), alpha, beta, gamma, epsilon, инициализация(uniSam, rnd), 
// разреженность p(t|dw) (GS: 0->ndw сэмплов, иначе min(x, ndw). PLSA: 0->обычный, иначе обнуление доли x ненулевых значений p[t]), 
// шаг обновления pidw (add, mult), 
// параметры дополнительно разреженности start, step, ratio, число тем, число итераций, 
// комментарий, который войдет в конец названия файла с выводом,
// параметр для обработки новых слов в нерегуляризованных алгоритмах delta
// (при delta = 0 используется некоторое оптимальное значение delta0,
// параметр имеет такое же значение по умолчанию. в робастных алгоритмах phi_wt для новых слов равны 0)

//*********************************************


void algs_8(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01, t_init ini=uniSam) {
// Эксперимент с S/P, D/нет D, R/нет R
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
// Эксперимент с S/P, D/нет D, R/нет R
	launch(col, S, 0,   0,    0,	0,   ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  0,	0,   ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, 0,   0,    0,	0,	 ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  0,	0,	 ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void algs_8_robust(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01, t_init ini=uniSam) {
// Эксперимент с S/P, D/нет D, R/нет R
	launch(col, S, 0,   0,    gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, 0,   0,    gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam,	eps, ini, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


void P_vs_SD(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) {
// Эксперимент с ролью новых слов и регуляризации
	launch(col, P, 0,   0,    gam, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void mult_step(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { //2h
// Эксперимент с мультипликативным шагом
	launch(col, P, 0,   0,    gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, 0,   0,    gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 0, mult, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void sampling_min(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// Эксперимент с числом сэмплирований в SD, в SDR
	launch(col, S, alp, bet,  gam, eps, uniSam, 1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 4,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, 10, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void sampling_fix(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// Эксперимент с числом сэмплирований в SD, в SDR
	launch(col, S, alp, bet,  gam, eps, uniSam, -1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -4,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet,  gam, eps, uniSam, -10, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


// Значение параметра s = 100 для алгоритмов типа P зарезервировано на смысл "оставляй в Hdwt ndw максимальных элементов".
// Это крайне коряво, надо быть осторожными!

void PLSA_fixtails_start0(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// Новый эксперимент с хвостами
	launch(col, P, alp, bet,  gam, eps, uniSam, 100,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix"); // в коде plsa 100
	launch(col, P, alp, bet,  gam, eps, uniSam, 1,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 2,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 3,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 4,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 5,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, 10, add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
}

void PLSA_fixtails_start10(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // 3h
// Новый эксперимент с хвостами
	launch(col, P, alp, bet,  gam, eps, uniSam, -100,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, -1,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
	launch(col, P, alp, bet,  gam, eps, uniSam, -2,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -3,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -4,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -5,  add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
//	launch(col, P, alp, bet,  gam, eps, uniSam, -10, add, sRaws, 0, 0, 0, 0, 100, 40, "fix");
}


void PLSA_tails_start0(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // many
// Эксперимент с разреживанием Hdwt в PLSA
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.05, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, 0.5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void PLSA_tails_start10(t_col col, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) { // many
// Эксперимент с разреживанием Hdwt в PLSA
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.05, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.1,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.2,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.3,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.5,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.6,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, P, alp, bet,  gam, eps, uniSam, -0.7,  add, sRaws, 0, 0, 0, 0, 100, 40, "");
}

void SDR_gamma_opt(t_col col, double alp=0.5, double bet=0.01, double eps=0.01) { // 6h
// Эксперимент с подбором gamma
	launch(col, S, alp, bet, 0,     eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.001, eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.01,  eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.05,  eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.1,   eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.3,   eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, 0.5,   eps, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


void SDR_eps_opt(t_col col, double alp=0.5, double bet=0.01, double gam=0.01) { // 2h
// Эксперимент с подбором eps
	launch(col, S, alp, bet, gam, 0,    uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.01, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.05, uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.1,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.2,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.3,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
	launch(col, S, alp, bet, gam, 0.5,  uniSam, 0, add, sRaws, 0, 0, 0, 0, 100, 40, "");
}


void ExtraSparsing(t_col col, double gam=0.01, double eps=0.01) { // 3h
// Дополнительное разреживание P(R) и S(R)
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
// Эксперимент с подбором beta в PD или SD
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
// Эксперимент с подбором delta для новых слов в P
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
// Cписок экспериментов для большой статьи, январь 2013:
	// 8 запусков - умная инициализация, все сочетания S/P, D/нет D, R/нет R
	algs_8(col, alp, bet, gam, eps, uniSam);

	// 8 запусков - рандомная инициализация, все сочетаний S/P, D/нет D, R/нет R 
	algs_8(col, alp, bet, gam, eps, rnd);

	// 4 запуска - робастные с мультипликативными шагами, все сочетания S/P, D/нет D
	mult_step(col, alp, bet, gam, eps);

	// 6 запусков - подбор числа самплирований для SD
	sampling_min(col, alp, bet, 0, 0);

	// 6 запусков - подбор числа самплирований для SDR
	sampling_min(col, alp, bet, gam, eps);

	// 5 запусков - подбор параметра разреживания Hdwt в P
	PLSA_tails_start0(col, 0, 0, 0, 0);

	// 5 запусков - подбор параметра разреживания Hdwt в PR
	PLSA_tails_start0(col, 0, 0, gam, eps);

	// 8 запусков - оптимизация gamma в SDR
	SDR_gamma_opt(col, alp, bet, eps);

	// 6 запусков - оптимизация eps в SDR
	SDR_eps_opt(col, alp, bet, gam);

	// дополнительное разреживание PR и SR
	ExtraSparsing(col, gam, eps);

	// дополнительное разреживание P и S
	ExtraSparsing(col, 0, 0);
}


void TestJanuary2013NewWords(t_col col, t_col colNW, double alp=0.5, double bet=0.01, double gam=0.01, double eps=0.01) {
	// сравниваем работу всех основных алгоритмов на двух коллекциях
	algs_8(col, alp, bet, gam, eps, uniSam);
	algs_8(colNW, alp, bet, gam, eps, uniSam);

	// оптимизация параметра delta для работы с новыми словами в P
	deltaOpt(colNW);

	// оптимизация параметра beta, участвующего в обработке новых слов в PD и SD на двух коллекциях
	betaOpt(col, P);	
	betaOpt(col, S);
	betaOpt(colNW, P);
	betaOpt(colNW, S);	
}

// А потом были эксперименты с Tscores. И были эксперименты с Top-глазками.


void TestApril2013(t_col col, double alp=0.5, double bet=0.01, double gam=-0.01, double eps=0.01) { // теперь по умолчанию гамма возрастает
// Переделывание экспериментов.
	// 8 запусков - умная инициализация, все сочетания S/P, D/нет D, R/нет R
	algs_8(col, alp, bet, gam, eps, uniSam);

	// 4 запусков - рандомная инициализация, сочетания S/P, D/нет D
	algs_4_nonRobust(col, alp, bet, rnd);

	// 4 запуска - робастные с мультипликативными шагами, все сочетания S/P, D/нет D
	mult_step(col, alp, bet, gam, eps);

	// 12 запусков - подбор стратегии и числа самплирований для SD
	sampling_fix(col, alp, bet, 0, 0);
	sampling_min(col, alp, bet, 0, 0);

	// 12 запусков - подбор стратегии и числа самплирований для SDR
	sampling_fix(col, alp, bet, gam, eps);
	sampling_min(col, alp, bet, gam, eps);

	// 12 запусков - подбор параметра разреживания Hdwt в P
	PLSA_tails_start0(col, 0, 0, 0, 0);
	PLSA_tails_start10(col, 0, 0, 0, 0);

	// 12 запусков - подбор параметра разреживания Hdwt в PR
	PLSA_tails_start0(col, 0, 0, gam, eps);
	PLSA_tails_start10(col, 0, 0, gam, eps);

	// 9 запусков - оптимизация gamma в SDR
	SDR_gamma_opt(col, alp, bet, eps);

	// 7 запусков - оптимизация eps в SDR
	SDR_eps_opt(col, alp, bet, gam);

	// дополнительное разреживание PR и SR
	ExtraSparsing(col, gam, eps);

	// дополнительное разреживание P и S
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
    // 3 вида разреживания
    LaunchSparsing(sOld);
    LaunchSparsing(sUniform);
    LaunchSparsing(sBackTrain);

    // модель с разреживанием и сглаживанием

    // оптимизируем разреживание
    LaunchSparsingAndSmoothing(10, sUniform, 0.5, 0.01, sOld);
    LaunchSparsingAndSmoothing(10, sUniform, 0.5, 0.01, sUniform);
    LaunchSparsingAndSmoothing(10, sUniform, 0.5, 0.01, sBackTrain);

    // другое число шумовых тем 
    LaunchSparsingAndSmoothing(1, sUniform, 0.5, 0.01, sOld);
    LaunchSparsingAndSmoothing(1, sUniform, 0.5, 0.01, sBackTrain);

     // другое сглаживание
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


// gs не работает, NW тоже вряд ли, робастность тем более.
int main(int argc, char* argv[])
{
    LaunchDecember2013(NIPSOld, 100, 10, 40, sUniform, 0.8, 0.1, sRaws, sUniform, 10, 1, 0.08, 0.1, 10, 100000, 10, 0.5, 0.02, "");

    // ToArticle7();

   	printf("Everything is completed");
	for(;;);
	return 0;
}

