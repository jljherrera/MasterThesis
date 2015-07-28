//============================================================================
// Name        : functions.cpp
// Author      : Jose Luis Juan Herrera Garcia
// Version     : July 28, 2015
// Copyright   : Copy freely
// Description : Modular arithmetic with polynomials using GiNaC
//============================================================================

#include <iostream>
#include <stdio.h>
#include <ginac/ginac.h>
//#include <time.h>
using namespace std;
using namespace GiNaC;

/*********************************************
 *                                           *
 *              F U N C T I O N S            *
 *                                           *
 *********************************************/
int lineparams(int argc, char *argv[], unsigned int * n, unsigned int * ord){
	if ( argc == 1 ){
		*n   = 16;	// Default number of variables 16
		*ord = 7;	// Default field F_7
	}
	else if ( argc == 2 ){
		*n   = atoi(argv[1]);
		*ord = 7;	// Default field F_7
	}
	else if ( argc == 3 ){
		*n   = atoi(argv[1]);
		*ord = atoi(argv[2]);
	}
	else{
		cout << "\nUsage: " << argv[0] << " NumVars BaseField\n" << endl;
		return 1;
	}

	if ( *n<1 ){
		cout << "\nError in number of variables\n" << endl;
		return 2;
	}
	else if ( *ord<1 ){
		cout << "\nError in order of field\n" << endl;
		return 3;
	}

	cout << "\nPolynomials in " << *n << \
			" variables will be generated, in a field F_" << \
			*ord << "\n" << endl;
	return 0;
}

string get_name(const string & s, int k)
{
	char buff[10];
	sprintf(buff, "%d", k);
	return (string(s) + buff);
}

const symbol & x(const int k)
/* get_symbol modified: always returns a symbol starting with "x"
   concatenated with the number given */
{
	const string s = get_name("x", k);

	static map<string, symbol> directoryX;

	map<string, symbol>::iterator i = directoryX.find(s);
	if (i != directoryX.end())
		return i->second;
	else
		return directoryX.insert(make_pair(s, symbol(s))).first->second;
}

const symbol & a(const int k)
/* get_symbol modified: always returns a symbol starting with "a"
   concatenated with the number given */
{
	const string s = get_name("a", k);

	static map<string, symbol> directoryA;

	map<string, symbol>::iterator i = directoryA.find(s);
	if (i != directoryA.end())
		return i->second;
	else
		return directoryA.insert(make_pair(s, symbol(s))).first->second;
}

numeric ret2order(ex value, unsigned int ord)
/* Return a given "value" to the field of order "ord".
 * REMARK: "value" MUST evaluate to a "numeric" type */
{
	return mod( ex_to<numeric>(value), ord );
}

ex term2order(ex term, unsigned int ord)
/* Return a "term" of a polynomial to a one with the correct
 * order "ord" */
{
	size_t	i;
	ex		newterm;	// To acumulate new term

	if ( is_a<mul>(term) ){	// Several monomials multiplying themselves
		newterm = 1;	// Initialize accumulator for new term
		for ( i=0; i!=term.nops(); i++){
			if ( is_a<numeric>( term.op(i) ) ){
				newterm *= ret2order(term.op(i), ord);
			}
			else{
				newterm *= term.op(i);
			}
		}
		return newterm;
	}
	else if ( is_a<numeric>(term) ){ // A constant term
		return ret2order(term, ord);
	}
	else if ( is_a<symbol>(term) || is_a<power>(term) ){
		return term;
	}
	else{
		cout << "Term not identified: " << term << endl;
		return 9999;
	}
}

ex poly2order(ex poly, unsigned int ord)
/* Returns a polynomial "poly" as a polynomial with coefficients
 * in field F_ord */
{
	size_t	i;
	ex		newpoly;	// To accumulate new polynomial

	newpoly = 0;
	for ( i=0; i!=poly.nops(); i++){
		newpoly += term2order(poly.op(i), ord);
	}
	return newpoly;
}

ex genquadterm(symbol var1, symbol var2, unsigned int ord)
/* Generates a quadratic term with "var1", "var2" and a
 * coefficient in F_ord */
{
	return (rand() % ord) * var1 * var2;
}

ex genlinterm(symbol var, unsigned int ord)
/* Generates a linear term with "var" and a coefficient in F_ord */
{
	return (rand() % ord) * var;
}

ex genconsterm(unsigned int ord)
/* Generates a constant term in F_ord */
{
	return ( rand() % ord );
}

ex genpolyUOV(const symbol & (* var)(int), unsigned int o,\
		unsigned int v, unsigned int ord)
/* Generates a polynomial with the construction of UOV.
 * The first parameter is the variable to use, observe that
 * it is a function (first of all return value "const symbol &", then
 * pointer to the function "var" and finally the parameter to the
 * function "int"). Then comes the number of oil and vinegar variables
 * and finally the order of the field F_ord.
 * REMARK: from var(0) to var(v-1) we have the vinegar variables and
 *         from var(v) to var(n-1) the oil variables */
{
	ex				poly  = 0;	// Initialize polynomial
	ex				polyq = 0;	// Initialize polynomial: quad terms
	ex				polyl = 0;	// Initialize polynomial: linear terms
	ex				aux   = 0;
	unsigned int	n = o + v;	// Total number of variables
	size_t			i, j;

	if ( ord == 2 ){
		for ( i=0; i<v; i++ ){		// Products vars. vinegar times vinegar+oil
			for ( j=0; j<n; j++ ){
				if ( i == j ) continue;	// Since x_i^2 = x_i
				aux += genquadterm( var(i), var(j), ord );
			}
			polyq += aux;
			aux    = 0;
		}
	}
	else{
		//clock_t tStart = clock();
		for ( i=0; i<v; i++ ){		// Products vars. vinegar times vinegar+oil
			for ( j=0; j<n; j++ ){
				aux += genquadterm( var(i), var(j), ord );
			}
			polyq += aux;
			aux    = 0;
		}
		//cout << "Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
	}
	for ( i=0; i<n; i++ ){		// Linear terms
		polyl += genlinterm( var(i), ord );
	}
	poly  = polyq + polyl + genconsterm(ord);
	return poly2order(poly, ord);
}

ex * genpolyvectUOV(const symbol & (* var)(int), unsigned int o,\
		unsigned int v, unsigned int ord, ex * polyvect)
/* Generates a polynomial vector, i.e. "o" polynomials with
 * the construction of UOV.
 * The first parameter is the variable to use, observe that
 * it is a function (first of all return value "const symbol &", then
 * pointer to the function "var" and finally the parameter to the
 * function "int"). Then comes the number of oil and vinegar variables
 * and finally the order of the field F_ord.
 * REMARK 1: from var(0) to var(v-1) we have the vinegar variables and
 *         from var(v) to var(n-1) the oil variables
 * REMARK 2: Since a pointer to "polyvect" is passed, this array is
 *           modified and a pointer to it is returned, although it
 *           is of no interest, since in 'main' we can use directly
 *           the original "polyvect" given.
 * REMARK 3: Since these polynomial are being generated for the first
 *           time, it is not necessary to call "ret2order" since they
 *           are generated correctly in F_ord */
{
	for ( size_t i=0; i<o; i++ ){
		polyvect[i] = genpolyUOV(var, o, v, ord);
	}
	return polyvect;
}


numeric xgcd(numeric a, numeric b, numeric & x, numeric & y)
/* Extended Euclid algorithm: returns gcd(a,b) and in the passed
 * variables "x" and "y" the numbers so x*a + y*b = gcd(a,b) */
{
	numeric		d, xp, yp;
	if ( b == 0 ){
		x = 1;
		y = 0;
		return a;
	}
	d = xgcd( b, mod(a, b), xp, yp );
	x = yp;
	y = xp - iquo(a,b)*yp;
	return d;
}

numeric invnumber(numeric a, unsigned int ord)
/* Computes the inverse of "a" in a field F_ord) */
{
	numeric	d, xp, yp;	// gcd(a,ord) = d = xp*a + yp*ord

	d = xgcd(a, ord, xp, yp);
	if ( d != 1 ){
		cout << a << " does not have an inverse in F_" << ord << endl;
		return 0;
	}
	return ret2order(xp, ord);
}

void printmat(string text, matrix mat, unsigned int rows,\
		unsigned int cols)
/* Prints in console a matrix "rows x cols" */
{
	size_t	i,j;

	cout << text << endl;
	for ( i=0; i<rows; i++ ){
		cout << i+1 << ".\t";
		for ( j=0; j<cols; j++ ){
			cout << mat(i,j) << " ";
		}
		cout << endl;
	}
	cout << endl;
}

void printvec(string text, ex * vec, unsigned int size)
/* Prints in console a vector of "size" rows */
{
	size_t	i;

	cout << text << endl;
	for ( i=0; i<size; i++ ){
		cout << i+1 << ".\t" << vec[i] << endl;
	}
	cout << endl;
}

void genrndmatrix(matrix & Ms, matrix & MsInv,\
		unsigned int n, unsigned int ord)
/* A pointer to an already created matrix must be passed to this function
 * and it is filled with random values in F_ord. This function, also
 * checks, that the generated matrix is invertible and returns in "MsInv"
 * that matrix. "MsInv" must also have been created. Note that "Ms, MsInv"
 * are classes!!! */
{
	ex		denom = 0;
	ex		invdenom;
	size_t	i,j;

	while ( 1 ){
		for ( i=0; i<n; i++ ){
			for ( j=0; j<n; j++ ){
				Ms(i,j) = rand() % ord;
			}
		}
		if ( ret2order(Ms.determinant(), ord) != 0 ) break;	// Ms is invertible
	}

	MsInv = Ms.inverse();

	for ( i=0; i<n; i++ ){		// Finding biggest denominator
		for ( j=0; j<n; j++ ){
			if ( MsInv(i,j).denom() > denom ){
				denom = MsInv(i,j).denom();
			}
		}
	}

	invdenom = invnumber(ex_to<numeric>(denom), ord);	// Inverse of denom.

	for ( i=0; i<n; i++ ){		// Find inverse matrix
		for ( j=0; j<n; j++ ){
			MsInv(i,j) = denom*MsInv(i,j)*invdenom;
			if ( is_a<numeric>( MsInv(i,j) ) ){
				MsInv(i,j) = ret2order(MsInv(i,j), ord);
			}
			else{
				cout << "Error, MsInv with non-numeric values!!!: MxInv(" <<\
						i << "," << j << ") = " << MsInv(i,j) << endl;
			}
		}
	}
}

void genrndvector(matrix & vs, unsigned int n, unsigned int ord)
/* A pointer to an already created column vector must be passed
 * to this function and it is filled with random values in F_ord.
 * Note that "vs" is a class!!! */
{
	for ( size_t i=0; i<n; i++ ){
		vs(i,0) = rand() % ord;
	}
}

void S(matrix & Ms, matrix & vs, const symbol & (* var)(int),\
		unsigned int n, unsigned int ord, matrix & ss)
/* Computes the affine transformation: ss = Ms.var + vs */
{
	matrix	auxvect(n, 1);
	size_t	i;

	for ( i=0; i<n; i++ ){
		auxvect(i, 0) = var(i);	// Define vector in variables "var"
	}

	ss = Ms.mul(auxvect).add(vs);
	for ( i=0; i<n; i++ ){		// Back to order: F_ord
		ss(i,0) = poly2order(ss(i,0), ord);
	}
}

void Sinv(matrix MsInv, matrix vs, matrix ss, unsigned int n,\
		unsigned int ord, matrix & xx)
/* Computes the inverse affine transformation: xx = MsInv(ss - vs) */
{
	size_t		i;

	xx = MsInv.mul(ss.sub(vs));

	for ( i=0; i<n; i++ ){
		if ( xx(i,0).nops() == 0 ){
			xx(i,0) = ret2order(xx(i,0), ord);	// Just a constant
		}
		else{
			xx(i,0) = poly2order(xx(i,0), ord);
		}
	}
}

void evalpoly(ex poly, const symbol & (* var)(int), matrix & assig,\
		ex & y, unsigned int n, unsigned int ord)
/* Computes the image "y" of "poly" in "assig". "poly" has "n" variables
 * of type "var" and this function makes sure the result is in F_ord */
{
	size_t	i, j;
	lst ls1, ls2;
	unsigned int step = 2;

	if ( step > n ) step = n;

	for ( i=0; i<n; i+=step ){
		for ( j=i; ( (j<(i + step)) && (j<n) ); j++ ){
		      ls1.append(var(j));
		      ls2.append(assig(j,0));
		}
		poly = poly.subs(ls1, ls2, subs_options::no_pattern);
		poly = expand(poly);
		ls1.remove_all();
		ls2.remove_all();
	}
	y = expand(poly);
	if ( y.nops() == 0 ){
		y = ret2order(y, ord);	// Just a constant
	}
	else{
		y = poly2order(y, ord);
	}
}

void evalpolyvect(ex * polyvect, const symbol & (* var)(int),\
		matrix & assig, ex * y, unsigned int n, unsigned int o,\
		unsigned int ord)
/* Computes the image "y" of "polyvect" in "assig". "polyvect" has "n"
 * variables of type "var" and this function makes sure the result is in F_ord */
{
	for ( size_t i=0; i<o; i++ ){
		evalpoly(polyvect[i], var, assig, y[i], n, ord);
		}
}

unsigned int solvesyseq(ex * polyvect, ex * img, const symbol & (* var)(int),\
		matrix & preimg, unsigned int n, unsigned int o, unsigned int ord)
/* Solve a system of equations of the form "polyvect = img" in variables
 * "var". The value found of the "n" variables will be returned in
 * "preimg" matrix. Remember that the first "n-o" variables are de vinegar
 * variables and they take a random value. This produces "o" equations
 * in "o" oil variables and this function solves this system of equations.
 * This function returns the number of tries made to find a solution. */
{
	lst 			eqns, vars;
	ex				pv[o], result, aux;	// To store polys without vinegar vars.
	matrix			vinvars( (n-o), 1 );// Vinegar variables
	size_t			i;
	unsigned int	k = 1;				// NUmber of tries to find solution
	char			cont = 1;			// Flag to look for a solution
	char			taut = 0;			// Tautology found
	char			denzero = 0;		// A zero denominator;
	numeric			denom;				// Denominator of a number
	numeric			invdenom;

	while( cont ){
		for ( i=0; i<(n-o); i++ ){		// Generate randomly, vinegar variables
			vinvars[i]   = rand() % ord;
			preimg(i,0)  = vinvars[i];
		}

		evalpolyvect(polyvect, var, vinvars, pv, n-o, o, ord);

		for ( i=0; i<o; i++ ){			// Generate list of equations
			eqns.append( pv[i] == img[i] );
		}

		for ( i=n-o; i<n; i++ ){			// Generate list of oil variables
			vars.append( var(i) );
		}

		result = lsolve(eqns, vars);

		if ( result.nops() == 0 ){
			//cout << "Result empty!!!" << endl;
			eqns.remove_all();
			vars.remove_all();
			k++;
			continue;
		}

		for ( i=n-o; i<n; i++ ){
			aux = var(i);
			aux = aux.subs(result[i - (n-o)]);	// 'result' starts in 0
			if ( is_a<numeric>(aux) ){
				preimg(i,0) = aux;
			}
			else{	// A tautology found: No solutions for system of eqns.
				//cout << "Tautology found!!!" << endl;
				eqns.remove_all();
				vars.remove_all();
				k++;
				taut = 1;
				break;
			}
		}
		if ( taut == 1 ){
			taut = 0;
			continue;
		}

		for ( i=0; i<n; i++ ){
			denom = (ex_to<numeric>(preimg(i,0))).denom();
			if ( denom != 1 ){
				if ( ret2order(denom, ord) == 0 ){
					denzero = 1;
					break;
				}
				invdenom = invnumber(denom, ord);
				if ( invdenom == 0 ){
					denzero = 1;
					break;
				}
				preimg(i,0) = preimg(i,0).numer() * invdenom;
			}
			preimg(i,0) = ret2order(preimg(i,0), ord);
		}

		if ( denzero == 1 ){
			eqns.remove_all();
			vars.remove_all();
			k++;
			denzero = 0;
			continue;
		}
		cont = 0;	// Result found
	}
	return k;
}
