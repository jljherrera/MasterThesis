//============================================================================
// Name        : 3SAT.cpp
// Author      : Jose Luis Herrera
// Version     :
// Copyright   : Copy freely
// Description : Polynomials generation
//============================================================================

#include <iostream>
#include <sstream>		/* ostringstream */
#include <vector>
#include <array>
#include <string>
#include <stdio.h>      /* NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cmath>
#include <ginac/ginac.h>
#include <map>

using namespace std;
using namespace GiNaC;

void characteristic3CF(vector<int> &, string &, int **);
void findTrueValues(size_t, int **);
void incBinary(vector<int> &, size_t);
void inverseChar3CF(size_t, int **);
int testAssignment(size_t, vector<int> &, int **);
string get_name(const string & s, int k);
const symbol & x(const int);
int getIdx(ex);
ex genPolynomials(size_t, int **);
int findNegated(int, int **, vector<int> &, vector<int> &);
ex checkAssigInPoly(vector<int> &, ex);
void findSatPtsPoly(size_t, ex);

int main() {

	//vector<int> aPoint {1,0,0,1};	// Point at the 4-dimensional hypercube in Q^n (n>=3)
	//vector<int> aPoint {1,1,0,0,1,1,0};	// Point at the 7-dimensional hypercube in Q^n (n>=3)
	//vector<int> aPoint {1,1,0,0,1,1,0,1};	// Point at the 8-dimensional hypercube in Q^n (n>=3)
	//vector<int> aPoint {1,1,0,0,1,1,0,1,1};	// Point at the 9-dimensional hypercube in Q^n (n>=3)
	vector<int> aPoint {1,1,0,0,1,1,0,1,1,1,0,0,0,1};// Point at the 14-dimensional hypercube in Q^n (n>=3)
	//vector<int> aPoint {1,0,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,0,1,0};// Point at the 20-dimensional hypercube
	//vector<int> aPoint {1,1,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,1,0,1,0,1};// Point at the 30-dimensional hypercube in Q^n (n>=3)
//	vector<int> aPoint {1,1,0,0,1,1,0,1,1,1,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,1,0,1,0,1,0,1,0,
//		1,1,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,1,0,
//		1,1,1,1,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,1,0};

	vector<int> guessPoint(aPoint.size());

	size_t i;
	int ii;
	string fa;	// 3CF function
	ex compPoly, termPoly;

	const int num3CF = 4*(aPoint.size() - 1) - 1;
	int** ptrf;
	ptrf = new int *[num3CF];

	for (ii=0; ii<num3CF; ii++)
	{
		ptrf[ii] = new int [6];
	}

	for (ii=0; ii<int(aPoint.size()); ii++)	// Generate "x" variables
	{
		//cout << x(ii) << " ";
		x(ii);
	}

	characteristic3CF(aPoint, fa, ptrf);

	cout << "\n\nPoint in " << aPoint.size() << "-dimensional hypercube: { ";
	for (i=0; i<aPoint.size(); i++)
	{
		cout << aPoint[i] << " ";
	}
	cout << "}\n" << endl;
	cout << "Computed 3CF: " << fa << "\n" << endl;

	findTrueValues(aPoint.size(), ptrf);
	cout << "\n";
	inverseChar3CF(aPoint.size(), ptrf);
	cout << "\n";
	compPoly = genPolynomials(aPoint.size(), ptrf);
	cout << "Computed polynomial: " << compPoly << endl;
	cout << "\n";
	cout << "Polynomial evaluated in original assignment: " << checkAssigInPoly(aPoint, compPoly);
	cout << "\n" << endl;

	findSatPtsPoly(aPoint.size(), compPoly);

	cout << "\nFinished\n" << endl;

	for (i=0; i<guessPoint.size(); i++)
	{
		guessPoint[i] = 9;
	}
	ii = 0;
	cout << "Computed polynomial has " << compPoly.nops() << " terms:\n" << endl;
	for (i=0; i!=compPoly.nops(); i++)
	{
		termPoly = compPoly.op(i);
		cout << termPoly << " ";
		if ( is_a<symbol>(termPoly) )
		{
			cout << "1  ";
			cout << getIdx(termPoly) << ", ";
			guessPoint[getIdx(termPoly)] = 1;
		}
		else if ( is_a<symbol>(-1*termPoly) )
		{
			termPoly = -1*termPoly;
			cout << "-1  ";
			cout << getIdx(termPoly) << ", ";
			guessPoint[getIdx(termPoly)] = 0;
		}
		else
		{
			cout << "0,  ";
			ii++;
		}
	}
	cout << "\nMonomials with more than one variable or a constant: " << ii << endl;
	cout << "Guessed polynomial satisfying assignment: { ";
	for (i=0; i<guessPoint.size(); i++)
	{
		cout << guessPoint[i] << " ";
	}
	cout << "}\n" << endl;

//	cout << "GiNaC tutorial" << endl; // prints GiNaC tutorial
//
//	ex aux, sum1, sum2, sum3;
//
//	for (ii=0; ii<int(aPoint.size()); ii++)
//	{
//		cout << x(ii) << " ";
//	}
//	cout << endl;
//
//	ex CF = -1 + x(0) + x(1) + x(2) -x(0)*x(1) - x(0)*x(2) - x(1)*x(2) + x(0)*x(1)*x(2);
//	cout << "CF = " << CF << endl;
//	cout << "CF = " << CF.subs(x(0)==0).subs(x(1)==0).subs(x(2)==1) << endl;
//
//	sum1 = x(0)*x(1) + x(0)*x(2);
//	sum2 = -3*x(0)*x(1);
//	ii = 3;
//	sum3 = x(ii);
//	cout << sum3 << endl;
//	aux = sum1 + sum2 + sum3;
//
//	cout << aux << endl;
//	cout << aux.subs(x(0)==1).subs(x(1)==1) << endl;

	return 0;
}

void characteristic3CF(vector<int> &a, string &fa, int **ptrFunc)
{
	const int tuple = 3;
	const int partitions = 9;
	const int cols = 4;
	const char ch = 'x';	// Character used for padding
	int ac_idx, ii, jj, kk, ll, mm, nn, n, nJ;
	size_t i;

	n  = a.size();	// "n" with dimension of hypercube

	vector<int> J(n);
	vector<int> auxJ(n-1);
	vector<int> ac(n);
	vector<int> j(tuple + 1);
	char buff[21];	// To store index of 'X' variables, up to an integer of 64 bits
	string epsilon[cols];
	string fc, aux;
	string epsilon3[partitions][cols] = {
				{ "00_", "10_", "01_", "11_" },
				{ "_00", "_01", "01_", "11_" },
				{ "00_", "10_", "_11", "_10" },
				{ "_00", "_01", "_10", "_11" },
				{ "_00", "_10", "0_1", "1_1" },
				{ "0_0", "1_0", "0_1", "1_1" },
				{ "0_0", "1_0", "_11", "_01" },
				{ "0_0", "0_1", "10_", "11_" },
				{ "00_", "01_", "1_0", "1_1" }
		};

	// Initialization:
	srand (time(NULL));
	nJ = n;
	fa = "";
	for (ii=0; ii<n; ii++)
	{
		J[ii]    = ii;
		ac[ii]   = a[ii];
	}

	// Vector to generate exponents of "f3,a". Last part of algorithm. LSbit first.
	vector<string> ai;
	ai.push_back("000");
	ai.push_back("100");
	ai.push_back("010");
	ai.push_back("110");
	ai.push_back("001");
	ai.push_back("101");
	ai.push_back("011");
	ai.push_back("111");

	ll = 0;	// 3CF clauses generated
	while (nJ > 3)
	{
		ii  = rand() % (int)J.size();	// 0 <= ii < J.size()
		j[tuple]  = J[ii];			// Choose j3 in J
		J.erase(J.begin() + ii);	// J = J - {j3}

		for (i=0; i<J.size(); i++)	// Copy J in auxJ to generate j0, j1, j2
		{
			auxJ[i] = J[i];
		}

		for (ii=0; ii<tuple; ii++)	// Generate j0, j1, j2
		{
			kk = rand() % (int)auxJ.size();
			j[ii] = auxJ[kk];
			auxJ.erase(auxJ.begin() + kk);
		}
		auxJ.resize(J.size()-1);	// Leave auxJ with the correct size for next iteration

		ii = rand() % partitions;	// Randomly, choose one of the "partitions"
		for (jj=0; jj<cols; jj++)
		{
			epsilon[jj] = "";		// Generate template with selected partition
			for (kk=0; kk<n; kk++)	// First fill it with common character
			{
				epsilon[jj] += ch;
			}
			mm = 0;

			for (kk=0; kk<tuple; kk++)	// Now put partition bits in position determined by "j"
			{
				epsilon[jj].at(j[kk]) = epsilon3[ii][jj].at(mm);
				mm++;
			}
		}

		fc = "(";	// Generate "fc": four 3-clauses
		for (ii=0; ii<cols; ii++)
		{
			mm = 0;	// ptrFunc column counter
			for (jj=0; jj<tuple; jj++)
			{
				if ( epsilon[ii].at( j[jj] ) == '1' )
				{
					sprintf(buff, "%d", j[jj] );
					fc += string("X") + buff + "\'" + "v";
					ptrFunc[ll][mm]   = j[jj];
					ptrFunc[ll][mm+3] = 1; // Invert
					mm++;
					continue;
				}
				if ( epsilon[ii].at( j[jj] ) == '0' )
				{
					sprintf(buff, "%d", j[jj] );
					fc += string("X") + buff + "v";
					ptrFunc[ll][mm]   = j[jj];
					ptrFunc[ll][mm+3] = 0; // Do not invert
					mm++;
					continue;
				}
			}
			nn  = ac[j[tuple]] == 1 ? 0 : 1;
			aux = nn == 0 ? "" : "\'";
			sprintf(buff, "%d", j[tuple]);
			fc += string("X") + buff + aux + ")^(";
			ptrFunc[ll][mm]   = j[tuple];

			ptrFunc[ll][mm+3] = nn; // delta_3
			ll++;	// Pointing to next 3CF clause
		}
		fc.resize(fc.size() - 2);
		fa += fc + string("^");

		ac.at(j[tuple]) = 9;	// Suppressing j3-th entry in ac (with a 9)

		nJ--;
	}
	ii = 0;
	ac_idx = 0;
	for (i=0; i<ac.size(); i++)
	{
		if (ac[i] != 9)
		{
			ac_idx += ac[i] * std::pow(2,ii);
			ii++;
		}
	}
	ai.erase(ai.begin() + 7 - ac_idx);	// "ai" with exponents of variables in "J"

	fc = "(";
	for (i=0; i<ai.size(); i++)
	{
		mm = 0;	// ptrFunc column counter
		for (jj=0; jj<tuple; jj++)
		{
			if (ai[i].at(jj) == '0')
			{
				sprintf(buff, "%d", J[jj] );
				fc += string("X") + buff + "\'" + "v";
				ptrFunc[ll][mm]   = J[jj];
				ptrFunc[ll][mm+3] = 1;	// Invert
				mm++;
				continue;
			}
			if (ai[i].at(jj) == '1')
			{
				sprintf(buff, "%d", J[jj] );
				fc += string("X") + buff + "v";
				ptrFunc[ll][mm]   = J[jj];
				ptrFunc[ll][mm+3] = 0;	// Do not invert
				mm++;
				continue;
			}
		}
		fc.at(fc.size() - 1) = ')';
		fc += "^(";
		ll++;	// Pointing to next 3CF clause
	}
	//fc.at(fc.size() - 2) = ')';
	fc.resize(fc.size() - 2);

	fa += fc;	// 'fa' has a string with the 3CF generated
}

void findTrueValues(size_t n, int **ptrFunc)
{
	const int tuple = 3;
	const int num3CF = 4*(n - 1) - 1;
	int ii, jj, kk;
	int edo3CF, edoAll3CF;
	vector<int> inputValues(n);
	size_t i;

	if ( n>20 )
	{
		cout << "Computed satisfying assignment by testing all possibilities:\n";
		cout << "NOT computed. Taking too long: Number of bits " << n << endl;
		return;
	}

	for (i=0; i<n; i++)	// Start testing with zero vector
	{
		inputValues[i] = 0;
	}

	for (ii=0; ii<std::pow(2,n); ii++)
	{
		edoAll3CF = 1;	// Let's suppose this vector will satisfy
		for (jj=0; jj<num3CF; jj++)
		{
			edo3CF = 0;
			for (kk=0; kk<tuple; kk++)
			{
				if ((inputValues[ptrFunc[jj][kk]] ^ ptrFunc[jj][kk+3]) == 1)
				{
					edo3CF = 1;
					break;	// One 3CF clause found equal to one
				}
			}
			if (edo3CF == 0)
			{
				edoAll3CF = 0;
				break;	// One 3CF clause found equal to zero, so test next vector
			}
		}
		if ( edoAll3CF == 1)
		{
			// All 3CF clauses found equal to one, so this vector satisfies equation:
			cout << "Computed satisfying assignment by testing all possibilities:\n";
			for (i=0; i<n; i++)
			{
				cout << inputValues[i] << " ";
			}
			cout << endl;
		}
		incBinary(inputValues, n);
	}
}

void incBinary(vector<int> &binaryNumber, size_t n)
{
	size_t i;
	int    increment = 1;

	for (i=0; i<n; i++)
	{
		if ((binaryNumber[i] == 1) && (increment == 1))
		{
			binaryNumber[i] ^= increment;
			increment        = 1;
		}
		else
		{
			binaryNumber[i] ^= increment;
			increment        = 0;
		}
	}
}

void inverseChar3CF(size_t n, int **ptrFunc)
{
	const int num3CF = 4*(n - 1) - 1;
	int ii, numZeroes, found, carry, temp;
	vector<int> inputValues(n);
	size_t i;
	numZeroes = found = 0;
	int nonDef = 0;

	for (i=0; i<n; i++)	// Initialize assignment vector
	{
		inputValues[i] = 9;
	}

	/* Obtain trivially assigned values */
	for (ii=0; ii<(num3CF-7); ii +=4)
	{
		inputValues[ptrFunc[ii][2]] = ptrFunc[ii][5] == 0 ? 1 : 0;
	}
	for (ii=num3CF-7; ii<num3CF; ii++)
	{
		numZeroes = ptrFunc[ii][5] == 0 ? numZeroes+1 : numZeroes;
	}
	inputValues[ptrFunc[num3CF - 1][2]] = numZeroes == 4 ? 1 : 0;

	/* Determine number of bits undefined */
	for (i=0; i<n; i++)
	{
		if (inputValues[i] == 9)
		{
			nonDef++;
		}
	}

	/* Define array to test bits undefined */
	vector<int> a(nonDef);

	/* Find index in assignment vector of undefined bits */
	for (i=0; i<n; i++)
	{
		if (inputValues[i] == 9)
		{
			a[found] = i;
			found++;
			if (found == nonDef)
			{
				break;
			}
		}
	}

	/* Test possible values of undefined bits */
	for (ii=0; ii<nonDef; ii++)	// Start with zeroes combination
	{
		inputValues[a[ii]] = 0;
	}
	while ( testAssignment(inputValues.size(), inputValues, ptrFunc) == 0 )
	{
		carry = 1;	// Increment first bit always
		for (ii=0; ii<nonDef; ii++)	// Increment test bits
		{
			temp = inputValues[a[ii]];
			inputValues[a[ii]] = temp ^ carry;
			carry = temp & carry;
		}
	}

	cout << "Computed satisfying assignment by scanning 3-CF:\n";
	for (i=0; i<n; i++)
	{
		cout << inputValues[i] << " ";
	}
	cout << endl;
}

int testAssignment(size_t n, vector<int> &inputValues, int **ptrFunc)
{
	const int tuple = 3;
	const int num3CF = 4*(n - 1) - 1;
	int jj, kk;
	int edo3CF, edoAll3CF;
	//vector<int> inputValues(n);
	//size_t i;

	edoAll3CF = 1;	// Let's suppose this vector will satisfy
	for (jj=0; jj<num3CF; jj++)
	{
		edo3CF = 0;
		for (kk=0; kk<tuple; kk++)
		{
			if ((inputValues[ptrFunc[jj][kk]] ^ ptrFunc[jj][kk+3]) == 1)
			{
				edo3CF = 1;
				break;	// One 3 clause found equal to one
			}
		}
		if (edo3CF == 0)
		{
			edoAll3CF = 0;
			break;	// One 3 clause found equal to zero, so this vector fails
		}
	}
	// If edoAll3CF == 1, this vector satisfies equation
	return edoAll3CF;
}

string get_name(const string & s, int k)
{
	char buff[10];
	sprintf(buff, "%d", k);
	return (string(s) + buff);
}

const symbol & x(const int k)	// get_symbol modified: always
                                // returns a symbol starting with "x"
                                // concatenated with the number given
{
	const string s = get_name("x", k);
	static map<string, symbol> directory;

	map<string, symbol>::iterator i = directory.find(s);
	if (i != directory.end())
		return i->second;
	else
		return directory.insert(make_pair(s, symbol(s))).first->second;
}

int getIdx(ex monomial)	// It must receive a monomial with ONE variable only starting with 'x' and then a number!!!
{
	ostringstream auxStr;

	auxStr << monomial;
	return stoi(auxStr.str().erase(0,1));
}

ex genPolynomials(size_t n, int **ptrFunc)
{
	const int num3CF = 4*(n - 1) - 1;
	int ii, varsNeg;
	vector<int> idxNegated(3);
	vector<int> idxNonNegd(3);
	ex Polynomial, X0, X1, X2;

	Polynomial = 0;

	for (ii=0; ii<num3CF; ii++)
	{
		varsNeg = findNegated(ii, ptrFunc, idxNegated, idxNonNegd);
		switch(varsNeg)
		{
		case 0:	// No negated variables:
			X0 = x(idxNonNegd[0]);
			X1 = x(idxNonNegd[1]);
			X2 = x(idxNonNegd[2]);
			Polynomial += -1 + X0 + X1 + X2 - X0*X1 - X0*X2 - X1*X2 + X0*X1*X2;
			break;
		case 1:	// One negated variable:
			X0 = x(idxNegated[0]);
			X1 = x(idxNonNegd[0]);
			X2 = x(idxNonNegd[1]);
			Polynomial += -X0 + X0*X1 + X0*X2 - X0*X1*X2;
			break;
		case 2:	// Two negated variable:
			X0 = x(idxNegated[0]);
			X1 = x(idxNegated[1]);
			X2 = x(idxNonNegd[0]);
			Polynomial += -X0*X1 + X0*X1*X2;
			break;
		case 3:	// Three negated variable:
			X0 = x(idxNegated[0]);
			X1 = x(idxNegated[1]);
			X2 = x(idxNegated[2]);
			Polynomial += -X0*X1*X2;
			break;
		default:
			cout << "Critical error in polynomial generation!!!" << endl;
		}
	}
	//cout << "Computed polynomial: " << Polynomial << endl;
	return Polynomial;
}

int findNegated(int idx, int **ptrFunc, vector<int> &idxNegated, vector<int> &idxNonNegd)
{
	int ii, jj, count;

	count = 0;	// Initialize counter of negated variables found
	jj    = 0;  // Counter for non-negated variables
	for (ii=0; ii<3; ii++)
	{
		idxNegated[ii] = -1;	// No index found
		idxNonNegd[ii] = -1;
	}

	for (ii=3; ii<6; ii++)
	{
		if ( ptrFunc[idx][ii] == 1 )
		{
			idxNegated[count] = ptrFunc[idx][ii-3];
			count++;
		}
		else
		{
			idxNonNegd[jj] = ptrFunc[idx][ii-3];
			jj++;
		}
	}
	return count;
}

ex checkAssigInPoly(vector<int> & Point, ex compPoly)
{
	int bitPto;
	size_t i;
	ex tempPoly;

	tempPoly = compPoly;

	for (i=0; i<Point.size(); i++)
	{
		bitPto = Point[i];
//		if ( i == 0) bitPto = 0;
//		if ( i == 3) bitPto = 1;
		tempPoly = tempPoly.subs( x(i) == bitPto );
	}
	return tempPoly;
}

void findSatPtsPoly(size_t n, ex compPoly)
{
	int ii, counter=0;
	vector<int> inputValues(n);
	size_t i;
	ex tempPoly;

	cout << "Computed satisfying assignments in Polynomial, by testing all possibilities:\n";

	if ( n>=20 )
	{
		cout << "NOT computed. Taking too long: Number of bits " << n << endl;
		return;
	}

	for (i=0; i<n; i++)	// Start testing with zero vector
	{
		inputValues[i] = 0;
	}

	for (ii=0; ii<std::pow(2,n); ii++)
	{
		tempPoly = compPoly;
		for (i=0; i<n; i++)
		{
			tempPoly = tempPoly.subs( x(i) == inputValues[i] );
		}

		if ( tempPoly.is_zero() )	// Satisfying assignment found:
		{
			for (i=0; i<n; i++)
			{
				cout << inputValues[i] << " ";
			}
			cout << "Poly = " << tempPoly << endl;
			counter++;
		}
		incBinary(inputValues, n);
	}
	cout << "Found " << counter << " assignments satisfying Poly = 0" << endl;
}
