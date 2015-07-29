###################################################
#           Program to read public key            #
#                   from a file                   #
#                                                 #
#   Jose Luis Juan Herrera Garcia       May, 2015 #
#   Thesis work                                   #
#   Computer science graduate studies             #
#   CINVESTAV                                     #
###################################################

reset() # Erase all previously defined variables

###################################################
#                      Add-on's                   #
###################################################
import copy
import sys
load('uov.sage')

###################################################
#                    Functions                    #
###################################################

###################################################
#                  Initialization                 #
###################################################
if ( len( sys.argv ) != 2 ):
    print "\nUsage: {0} PubKeyFile\n".format(sys.argv[0])
    sys.exit(1)
else:
    PKFile    = sys.argv[1]

extPI = ".ir"   # File name extension for files with polys represented as ints
extMV = ".mv"   # File name extension for files with Matrix an vector
CTE  = 100000   # Just a constant to be added to the constant term of poly
porc = 1.0/3.0  # Percentage of Oil variables

###################################################
#                     Main code                   #
###################################################
####### Reading Public Key file represented as integers
print "\nReading file with Integer Representation of a public polynomial \
system ({0}{1})\n".format(PKFile, extPI)
n, o, pubIntRep = readInt(PKFile + extPI)
print "Number of variables:        ", n
print "Number of vinegar variables:", n-o
print "Number of oil variables:    ", o

####### Define variables for polynomial environment
print "\nDefining data structures for polynomial management"
v = n-o         # Number of Vinegar variables
a = list(var('a%d' % i) for i in range(n))  # Define 'a' list.
x = list(var('x%d' % i) for i in range(n))  # Define 'x' list. No matter
                                            # its content
k = PolynomialRing( GF(2), len(a) + len(x),
                    a + x )                 # Polynomial ring in 'n' variables
for i in range(n):          # Now 'a' and 'x' list have generators
    a[i] = k.gen(i)
    x[i] = k.gen(len(a) + i)
y  = vector(GF(2), o)   # Evaluation of polynomial (PiUOV)
Pi = vector(k, o)       # 'o' polynomials will be generated

####### Coding an integer represented secret key, back to polynomial secret key
print "\nConverting read data of an integer representation public polynomial \
to a real polynomial system"
Pi = int2PolySys(pubIntRep, n, x)
printPolyVect(Pi, "Public polynomials:")

###################################################
#                 Code for protocol               #
###################################################
print "\ndone!\n"
