//============================================================================
// Name        : modarith.cpp
// Author      : Jose Luis Juan Herrera Garcia
// Version     : July 28, 2015
// Copyright   : Copy freely
// Description : Modular arithmetic with polynomials using GiNaC
//============================================================================

#include <iostream>
#include <stdio.h>
#include <ginac/ginac.h>


using namespace std;
using namespace GiNaC;

#include "functions.h"

int main(int argc, char *argv[])
{
	unsigned int	n;		// Total number of variables
	unsigned int	o,v;	// Number of oil, vinegar variables
	unsigned int	ord;	// Field to work in. Order of field.
	int				rc;
	size_t  		i;
	ex				poly, subpoly;
	numeric			xp, yp;	// xp*a + yp*b = gcd(a,b)
	const char * PKFile   = "PK.txt";	// Name of public key file
	const char * SKFile   = "SK.txt";	// Name of secret key file
	const char * MVFile   = "MsVs.txt";	// Name of matrix and vector file (A.T.)
	const char * MInvFile = "MInv.txt";	// Name of inverse matrix file

	rc = lineparams(argc, argv, &n, &ord);
	if ( rc != 0 )
		return rc;

	for (i=0; i<n; i++){	// Generate "x" & "a" variables
		x(i);
		a(i);
	}

	/* Some initializations */
	o = int(n * RATIO);			// Number of oil variables
	v = n - o;					// Number of vinegar variables
	srand((unsigned)time(0));	// Initialize random number generator
	ex		PiUOV[o];			// Polynomial vector with UOV structure
	ex		Pi[o];				// Public polynomial vector
	ex		y[o];				// Image of a polynomial vector
	matrix	Ms(n, n);			// Matrix for affine transformation
	matrix	MsInv(n, n);		// Inverse of "Ms"
	matrix	vs(n, 1);			// Column vector for affine transformation
	matrix  aux(n, 1);			// Auxiliary vector
	matrix  preimg(n, 1);		// To store pre-image of a desired image

	cout << "Number of oil variables: " << o <<\
			". Number of vinegar variables: " << v << "\n" << endl;

	/* Generate UOV Polynomials */
	clock_t tStart = clock();
	genpolyvectUOV(a, o, v, ord, PiUOV);	// Not using returned pointer
	cout << "Time to generate PiUOV: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	printvec("Secret polynomials:", PiUOV, o);

	/* Generate matrix and vector for affine transformation */
	genrndmatrix(Ms, MsInv, n, ord);
	printmat("Ms matrix:", Ms, n, n);
	printmat("Ms inverse matrix:", MsInv, n, n);
	genrndvector(vs, n, ord);
	printmat("vs vector:", vs, n, 1);

	/* Generate Public Polynomials */
	tStart = clock();
	S(Ms, vs, x, n, ord, aux);
	cout << "Time to generate A.T.: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	printmat("A.T.:", aux, n, 1);

	tStart = clock();
	evalpolyvect(PiUOV, a, aux, Pi, n, o, ord);
	cout << "Time to generate Pi: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	printvec("Public polynomials:", Pi, o);

	/* Compute pre-image for a desired image */
	for ( i=0; i<o; i++ ){
		y[i] = rand() % ord;
	}
	printvec("Desired image for Pi:", y, o);

	tStart = clock();
	rc = solvesyseq(PiUOV, y, a, preimg, n, o, ord);
	cout << "Time to solve system of eqs.: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	cout << "Result found in " << rc << " tries." << endl;
	printmat("Pre-image in 'a' variables:", preimg, n, 1);
	tStart = clock();
	evpolyvectnum(PiUOV, a, preimg, y, n, o, ord);
	cout << "Time to compute PiUOV(a): " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	printvec("PiUOV evaluated in 'a'  :", y, o);

	tStart = clock();
	Sinv(MsInv, vs, preimg, n, ord, aux);
	cout << "Time to generate inverse A.T.: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	printmat("Pre-image in 'x' variables:", aux, n, 1);
	tStart = clock();
	evpolyvectnum(Pi, x, aux, y, n, o, ord);
	cout << "Time to compute Pi(x): " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	printvec("Pi evaluated in 'x'  :", y, o);

	/* Write info generated in files */
	rc = writekeys(PKFile, SKFile, MVFile, MInvFile, Pi, PiUOV,\
			Ms, vs, MsInv, n, o, ord);

	return 0;
}

