###################################################
#   Program to generate keys for authentication   #
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
load('cryptokeys.sage')
        
###################################################
#                  Initialization                 #
###################################################
if ( len( sys.argv ) != 3 ):
    print "\nUsage: genkey NumVars EncryptedFile\n\n"
    sys.exit(1)
else:
    try:
        n = int( sys.argv[1] )
    except:
        print "\nNumVars must be an integer number\n\n"
        sys.exit(2)
    encFile = sys.argv[2]
    
extPI = ".ir"   # File name extension for files with polys represented as ints
extMV = ".mv"   # File name extension for files with Matrix an vector
CTE  = 100000   # Just a constant to be added to the constant term of poly
porc = 1.0/3.0  # Percentage of Oil variables

o = int(n*porc) # Number of Oil variables
v = n-o         # Number of Vinegar variables


a = list(var('a%d' % i) for i in range(n))  # Define 'a' list.
x = list(var('x%d' % i) for i in range(n))  # Define 'x' list. No matter
                                            # its content
k = PolynomialRing( GF(2), len(a) + len(x),
                    a + x )                 # Polynomial ring in 'n' variables
for i in range(n):          # Now 'a' and 'x' list have generators
    a[i] = k.gen(i)
    x[i] = k.gen(len(a) + i)

y    = vector(GF(2), o)     # Evaluation of polynomial (Pi)
yUOV = vector(GF(2), o)     # Evaluation of polynomial (PiUOV)

Ms    = matrix(GF(2), n, n) # Matrix for affine transformation
vs    = vector(GF(2), n)    # Vector for affine transformation
Pi    = vector(k, o)        # 'o' polynomials will be generated
PiUOV = vector(k, o)        # 'o' polynomials will be generated

set_random_seed()	    # Set a random seed for RNG

###################################################
#                     Main code                   #
###################################################
print "\nSecret and Public Key generation for Zero Knowledge \
Authentication\n"
print "Number of variables:        ", n
print "Number of vinegar variables:", v
print "Number of oil variables:    ", o

##### Generate elements for affine transformation:
print "\nComputing 'Ms' matrix"
genRndMatrix(Ms)
print "\nComputing 'vs' vector"
genRndVector(vs)

####### Generate polynomial "P'" (PiUOV):
print "\nGenerating polynomial PiUOV. {0} equations".format(o)
genPolyVectorUOV(o, v, a, PiUOV, Debug = True)

####### Generate polynomial "P" (Pi):
print "\nGenerating polynomial Pi. {0} equations".format(o)
A = S(Ms, vector(x), vs)    # First generate affine transformation
evalPolyVect(PiUOV, a, A, Pi, Debug = True)

####### Writing encrypted file with private and public key
pp = readPP("\nPlease enter pass-phrase to encrypt private key file: ")
writeKeysEnc(PiUOV, Ms, vs, Pi, pp, easyK, encFile)

print "\ndone!\n"

