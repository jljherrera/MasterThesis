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
			". Number of vinegar variables: " << v << endl;

	/* Generate UOV Polynomials */
	genpolyvectUOV(a, o, v, ord, PiUOV);	// Not using returned pointer
	printvec("UOV polynomials:", PiUOV, o);

	/* Generate matrix and vector for affine transformation */
	genrndmatrix(Ms, MsInv, n, ord);
	genrndvector(vs, n, ord);

	/* Generate Public Polynomials */
	S(Ms, vs, x, n, ord, aux);
	//printmat("S:", aux, n, 1);

	evalpolyvect(PiUOV, a, aux, Pi, n, o, ord);
	printvec("Public polynomials:", Pi, o);

	/* Compute pre-image for a desired image */
	for ( i=0; i<o; i++ ){
		y[i] = rand() % ord;
	}
	printvec("Desired image for Pi:", y, o);

	rc = solvesyseq(PiUOV, y, a, preimg, n, o, ord);
	cout << "Result found in " << rc << " tries." << endl;
	printmat("Pre-image in 'a' variables:", preimg, n, 1);
	evalpolyvect(PiUOV, a, preimg, y, n, o, ord);
	printvec("PiUOV evaluated in 'a'  :", y, o);

	Sinv(MsInv, vs, preimg, n, ord, aux);
	printmat("Pre-image in 'x' variables:", aux, n, 1);
	evalpolyvect(Pi, x, aux, y, n, o, ord);
	printvec("Pi evaluated in 'x'  :", y, o);

	return 0;
}

