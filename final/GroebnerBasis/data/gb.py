###################################################
#                                                 #
#   Test program to solve a MQequations using     #
#          Groebner Basis with Polybori           #
#                                                 #
#   Jose Luis Juan Herrera Garcia       May, 2015 #
#   Thesis work                                   #
#   Computer science graduate studies             #
###################################################

###################################################
#                      Add-on's                   #
###################################################
import sys
import os
import time
import random
import copy
from polybori        import Block, declare_ring
from polybori        import Polynomial
from polybori        import Variable
from polybori.gbrefs import load_file
from polybori.gbcore import groebner_basis

###################################################
#                    Functions                    #
###################################################
def getFileHandler(msg, mode):
    ''' Prints "msg", asking for the name of a file to open
        in mode "mode". Returns error code and file handler '''
    fName = raw_input(msg)
    if ( fName != fName.replace(' ', '') ):
        print "\nFile name error: '{0}'\n".format(fName)
        fh = 0     # No file handler
        RC = 1
    else:
        RC = 0
        try:
            fh = open(fName, mode)
        except:
            print("\nCould not open file {0}\n".format(fName))
            RC = 2
            fh = 0    # No file handler
    return RC, fh

def getInteger(msg):
    ''' Prints "msg", asking for an integer greater than zero
        Returns error code and integer read '''
    intNumber = raw_input(msg)
    RC     = 0
    error  = False
    try:			# Is a number?
        intN = int(intNumber)
    except:
        print("\nNot a number {0}\n".format(intNumber))
        error =  True
        intN  = -1
        RC    =  1
    if ( ( not error ) and ( intNumber == 0 ) ):
        print "\nNumber error: '{0}'\n".format(intNumber)
        intN = -1
        RC   =  2
    return RC, intN

def readPK(fi):
    ''' Reads a file with fileHandler "fi", that contains in each line,
        a multivariate quadratic equation. The file
        has the Public Key. Returns a list with the correct
        names of variables for PolyBoRi (x1 as x(1)) and the index
        of the greater variable '''
    polySet   = []
    nums      = '0123456789'
    maxNumVar = ''
    maxNVInt  = 0
    for line in fi:         # Read file
        aux = ''
        line = line.replace( '\n', '' ).replace('^','**')
        fixing = False
        for i in range( len(line) ):
            ch = line[i]
            if ( fixing and ( ch in nums ) ):
                maxNumVar += ch
            if ( ch == 'x' ):                               # x(
                fixing = True
                aux += 'x('
                continue
            if ( fixing and ( ch not in nums ) ):           # x(nn) not EOL
                fixing = False
                aux += ')' + ch
                if ( maxNVInt < int( maxNumVar ) ): maxNVInt = int( maxNumVar )
                maxNumVar = ''
                continue
            elif ( fixing and ( i == ( len(line) - 1 ) ) ): # x(nn) in EOL
                fixing = False
                aux += ch + ')'
                if ( maxNVInt < int( maxNumVar ) ): maxNVInt = int( maxNumVar )
                maxNumVar = ''
                continue
            else:
                aux += ch
        polySet.append(aux)
    return polySet, maxNVInt

def writePolyForPolyBoRi(Poly, numVars, filename):
    ''' "Poly" must be a list of polynomials in PolyBoRi syntax.
        "numVars" is the maximum index number of an 'x' variable.
        This function, creates a file "filename" with the correct
        sintax so it can be loaded to compute its Groebner Basis '''
    fo = open(filename, "w")
    fo.write( 'declare_ring([Block("x", ' + \
              str( numVars ) + ', reverse=False)])\n' )
    fo.write( 'ideal = [' + '\n' )
    coma = ','
    for i in range( len(Poly) ):
        if ( i == ( len(Poly) - 1 ) ):
            coma = ''
        fo.write( Poly[i] + coma + '\n' )
    fo.write( ']\n' )
    fo.close()

def writePoly(Poly, fh, lenPK, maxNV, k, avgTime):
    ''' Store in file handler "fh" the contents of the vector of
        polynomials in "Poly" and some additional data'''
    for i in range( len(Poly) ):
        fh.write( "Polynomial " + str(i) + ":\n" + str(Poly[i])+"\n\n" );
    fh.write( "\nNumber of input equations:  {0}\n".format( lenPK ) )
    fh.write( "Total number of variables:  {0}\n".format( maxNV ) )
    fh.write( "Number of output equations: {0}\n".format( len(Poly) ) )
    fh.write( "Number of loops made:       {0}\n".format( k ) )
    fh.write( "Average elapsed time:       {0} secs.\n".format(avgTime) )

def subs(f, x, c):
    ''' Evaluates polynomial "f" in "x = c". "x" must be in the format "x(i)"
        and "c" MUST be 0 or 1. Returns the new polynomial with "x = c" '''
    i = x.index() 
    s = f.set() 
    if c == 0: 
        return Polynomial(s.subset0(i)) 
    else: 
        return Polynomial(s.subset1(i))+Polynomial(s.subset0(i))

def iniVarsAssig(noVars):
    ''' Returns a list of size "noVars" with values = -1, indicating the
        variable represented for each slot has not been assigned a value '''
    return [ -1 for i in range(noVars) ]

def addVarAssig(listVA, noVar, value):
    ''' In the list "listVA[noVar]" puts "value" '''
    listVA[ noVar ] = value

def subsVarsAssig(poly, listVA):
    ''' Substitutes in "poly" the values in "listVA" different from -1. The index in
        "listVA" with a 0 or 1 will be the index of the variable to substitute '''
    for i in range( len(listVA) ):
        if ( listVA[i] != -1 ):
            poly = subs( poly, x(i), listVA[i] )
    return poly

def getFstVarMon(monomial):
    ''' Returns the first variable in the monomial "monomial" '''
    return [ i for i in monomial.variables() ][0]

def getLstMonsDge1(poly):
    ''' Analyze "poly" and returns only the monomials which have degree
        greater or equal than 1 '''
    listMons = []
    aux      = [ i for i in poly.terms() ]
    for i in range( len(aux) ):
        if ( aux[i].deg() >= 1 ):
            listMons.append(aux[i])
    return listMons

def randVect(size):
    ''' Returns a random vector of size "size" with 0, 1 values
        randomly generated '''
    vect = []
    for i in range(size):
        vect.append( random.randint(0,1) )
    return vect

def equalZero(polys, y):
    ''' A set of polynomials "polys" equal to "y", are modified
        so that they returns a polynomial set equal to Zero, ready
        to compute a Groebner Basis. REMARK: Size of "polys" and "y"
        MUST be the same (this is not checked) '''
    for i in range( len(y) ):
        polys[i] += y[i]
    return polys

def endOfPoly(poly):
    ''' Analyse "poly" and returns a code: if it is empty (1), or has only one variable
        (2) or has only one variable plus the constant "1" (3). Or has only 1 (4).
        Otherwise returns (0) '''
    if ( poly == 0 ): 			# Polynomial empty (0)
        RC = 1
    elif ( (not poly.has_constant_part()) and
           (poly.n_variables() == 1 )):	# Number variables equal 1
        RC = 2
    elif ( poly.has_constant_part() and (poly.n_variables() == 1) ): 	# One variable + 1
        RC = 3
    elif ( poly.has_constant_part() and (poly.n_variables() == 0) ):    # Just 1
        RC = 4
    else:
        RC = 0
    return RC

###################################################
#                  Initialization                 #
###################################################
Debug = False
#Debug = True
msg  = '\nFile name with polynomials: '
mode = 'r'
RC, fpIn = getFileHandler(msg, mode)    # PublicKey file, to read
if ( RC != 0 ):
    sys.exit(1)

msg  = '\nFile name to store Groebner Basis: '
mode = 'w'
RC, fpOut = getFileHandler(msg, mode)
if ( RC != 0 ):
    fpIn.close()
    sys.exit(2)

msg   = '\nNumber of cycles to repeat: '
RC, k = getInteger(msg)
if ( RC != 0 ):
    fpIn.close()
    fpOut.close()
    sys.exit(3)
random.seed()                           # Initialize random number generator

###################################################
#                     Main code                   #
###################################################
PK, maxNV = readPK(fpIn)                    # Convert file to PolyBoRi format
maxNV    += 1				    # Not an index but a quantity
writePolyForPolyBoRi(PK, maxNV, "forPB")    # Generate file for Groebner Basis
data = load_file("forPB")                   # Import file to use in GB

acumm = 0.0

####### Computes Groebner Basis for a set of polynomials equal to a random "y"
for i in range( k-1 ):
    polys     = copy.copy(data.ideal)           # Copy original data ( = 0 )
    y         = randVect( len(data.ideal) )     # Generates random "y"
    polys     = equalZero(polys, y)             # Set of "polys" - "y" = 0
    startTime = time.time()
    gb = groebner_basis( polys )                # Computes Groebner Basis
    endTime   = time.time()
    acumm += (endTime - startTime)
    print ".",
    sys.stdout.flush()

####### Computes Groebner Basis for a set of polynomials equal to an
####### algebraic variety
startTime = time.time()
gb = groebner_basis( data.ideal )               # Computes last Groebner Basis
endTime   = time.time()                         # when "polys" = 0
acumm += (endTime - startTime)
print ".",
sys.stdout.flush()

if ( len(gb) < 5 ):
    print "\nComputed Groebner Basis:\n", gb
print "\nNumber of input equations:  {0}".format( len(PK) )
print "Total number of variables:  {0}".format( maxNV )
print "Number of output equations: {0}".format( len(gb) )
print "Number of loops made:       {0}".format( k )
print "Average elapsed time:       {0} secs.".format(acumm/k)

writePoly(gb, fpOut, len(PK), maxNV, k, acumm/k)

fpIn.close()
fpOut.close()

###################################################
#  Find values of variables using Groebner Basis  #
###################################################
print "\nFinding values of variables using the Groebner Basis"

declare_ring([Block("x", maxNV, reverse=False)])
listVars   = iniVarsAssig(maxNV)	# Initialize list holding values of assigned variables
listMDge1  = []				# List to store monomials with degree greater than 1

for i in range( len(gb) ):
    poly = gb[i]			# Read forward each polynomial in Groebner Basis file
    if (Debug): print "\n{0}. Working with:\n{1}".format(i,poly)

    poly = subsVarsAssig(poly, listVars)	# Evaluate "poly" in already found variables

    if (Debug): print "Variables already found:",listVars
    if (Debug): print "Poly after substituting current variables:\n{0}".format(poly)

    while ( True ):                             # If there are monomials with deg>=1 do:
        listMDge1    = getLstMonsDge1(poly)	# Get monomials to use in this cycle
        if ( len(listMDge1) > 0 ):              # If there are monomials left:
            var = getFstVarMon( listMDge1[0] )	# Get first variable of monomial with deg>1
        else:
            break
        if (Debug): print "Variable to assign value: {0}.".format(var),
        status = endOfPoly(poly)
        if   ( status == 0 ):   # Several monomials in poly, still pending
            addVarAssig(listVars, var.index(), 0)
            if (Debug): print "Assigned value:",0
            poly = subsVarsAssig(poly, listVars)
            continue
        elif ( status == 1 ):   # All terms in "poly" evaluated to cero
            break               # so, continue with next polynomial
        elif ( status == 2 ):   # Just one variable left, set it as zero and next poly
            addVarAssig(listVars, var.index(), 0)
            if (Debug): print "Assigned value:",0
            break
        elif ( status == 3 ):   # Just one variable left plus a constant,
            addVarAssig(listVars, var.index(), 1)   # so, set it as one and next poly
            if (Debug): print "Assigned value:",1
            break
        else:                   # status = 4, INCONSISTENCY!!!
            if (Debug): print "Inconsistency in polynomial {0}. 1 = 0!!!".format(i)
            break

print "Variables found:"
for i in range( len(listVars) ):
    if ( listVars[i] != -1 ):
        print "x({0}) = {1}".format( i, listVars[i] )
