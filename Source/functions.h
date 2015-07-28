/*
 * functions.h
 *
 *  Created on: Jul 28, 2015
 *      Author: jherrera
 */

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <ginac/ginac.h>

#define RATIO 1.0/3.0	// Ratio o/n

#ifndef SOURCE_FUNCTIONS_H_
#define SOURCE_FUNCTIONS_H_

int lineparams(int, char *argv[], unsigned int *, unsigned int *);
string get_name(const string &, int);
const symbol & x(const int);
const symbol & a(const int);
numeric ret2order(ex, unsigned int);
ex term2order(ex, unsigned int);
ex poly2order(ex, unsigned int);
ex genquadterm(symbol, symbol, unsigned int);
ex genlinterm(symbol, unsigned int);
ex genconsterm(unsigned int);
ex genpolyUOV(const symbol & (*)(int), unsigned int,\
		unsigned int, unsigned int);
ex * genpolyvectUOV(const symbol & (*)(int), unsigned int,\
		unsigned int, unsigned int, ex *);
numeric xgcd(numeric, numeric, numeric &, numeric &);
numeric invnumber(numeric, unsigned int);
void printmat(string, matrix, unsigned int, unsigned int);
void printvec(string, ex *, unsigned int);
void genrndmatrix(matrix &, matrix &, unsigned int, unsigned int);
void genrndvector(matrix &, unsigned int, unsigned int);
void S(matrix &, matrix &, const symbol & (*)(int),\
		unsigned int, unsigned int, matrix &);
void Sinv(matrix, matrix, matrix, unsigned int, unsigned int, matrix &);
void evalpoly(ex, const symbol & (*)(int), matrix &,\
		ex &, unsigned int, unsigned int);
void evalpolyvect(ex *, const symbol & (*)(int), matrix &,\
		ex *, unsigned int, unsigned int, unsigned int);
unsigned int solvesyseq(ex *, ex *, const symbol & (*)(int),\
		matrix &, unsigned int, unsigned int, unsigned int);

#endif /* SOURCE_FUNCTIONS_H_ */
