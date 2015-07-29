###################################################
#   Program implementing Encryption Protocol      #
#           (Client side - Alicia)                #
#                                                 #
#   Jose Luis Juan Herrera Garcia      June, 2015 #
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
import socket               # Import socket module

###################################################
#                  Initialization                 #
###################################################
extPI = ".ir"   # File name extension for files with polys represented as ints
extMV = ".mv"   # File name extension for files with Matrix an vector
CTE  = 100000   # Just a constant to be added to the constant term of poly
porc = 1.0/3.0  # Percentage of Oil variables

#######
PKFile = "PK32"                 # Base file name for public key
SKFile = "SK32"                 # Base file name for private key
PORT   = 12345
#######

###################################################
#                     Main code                   #
###################################################
print "\n******************************************\
************************************"
print "*                                 DECR\
YPTION - DEMO                          *"
print "* Receiver of an encrypted binary string: \
Decrypting a bit from a polynomial *"
print "******************************************\
************************************"
####### Reading Secret Key file represented as integers
print "\nReading file with Integer Representation of a secret polynomial \
system ({0}{1})\n".format(SKFile, extPI)
n, o, secIntRep = readInt(SKFile + extPI)
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
yUOV = vector(GF(2), o)     # Evaluation of polynomial (PiUOV)
Ms    = matrix(GF(2), n, n) # Matrix for affine transformation
vs    = vector(GF(2), n)    # Vector for affine transformation
PiUOV = vector(k, o)        # 'o' polynomials will be generated

####### Coding an integer represented secret key, back to polynomial secret key
print "\nConverting read data of an integer representation secret polynomial \
to a real polynomial system"
PiUOV = int2PolySys(secIntRep, n, a)

####### Reading matrix and vector of secret key
print "\nReading matrix and vector from file ({0}{1})\n".\
      format(SKFile, extMV)
n, Ms, vs = readMat(SKFile + extMV)

###################################################
#                 Code for protocol               #
###################################################
Decryp = vector(GF(2), o)       # To store decrypted vector

for j in range( o ):
    ####### Receive 'y' vector:
    s     = socket.socket()         # Create a socket object
    host  = socket.gethostname()    # Get local machine name
    port  = PORT                    # Reserve a port for the service.
    s.connect((host, port))
    aux = s.recv(1024)
    s.close                         # Close the socket when done

    aux = aux.replace('(','').replace(')','').replace(',','')
    aux = aux.split()
    for i in range( o ):
        yUOV[i] = Integer(aux[i])
    print "{0}. Received y: {1}".format(j+1, yUOV)

    ####### Find "X" solution for "yUOV":
    A = invertPiUOV(yUOV, o, v, a, PiUOV)
    xi = Sinv(Ms, A, vs)            # Compute 'xi'    
    
    ####### Receive 'p_LC':
    s     = socket.socket()         # Create a socket object
    host  = socket.gethostname()    # Get local machine name
    port  = PORT                    # Reserve a port for the service.

    s.connect((host, port))
    aux = s.recv(1024)
    s.close                         # Close the socket when done
    aux = aux.replace('[','').replace(']','').replace(',','')
    aux = aux.split()
    rxPolyIR = []
    for i in range( n + 2 ):
        rxPolyIR.append( Integer( aux[i] ) )
    print "{0}. Received p_LC: {1}".format(j+1, rxPolyIR)
    rxPoly = int2Poly(rxPolyIR, n, x)
    Decryp[j] = evalPoly(rxPoly, x, xi)
    print "Decrypted bit: {0}\n".format(Decryp[j])
    
print "Decrypted string: {0}\n".format(Decryp)
print "done!"
