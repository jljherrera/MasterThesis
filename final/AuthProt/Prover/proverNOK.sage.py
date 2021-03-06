# This file was *autogenerated* from the file proverNOK.sage
from sage.all_cmdline import *   # import sage library
_sage_const_2 = Integer(2); _sage_const_1 = Integer(1); _sage_const_5 = Integer(5); _sage_const_1p0 = RealNumber('1.0'); _sage_const_1024 = Integer(1024); _sage_const_10 = Integer(10); _sage_const_3p0 = RealNumber('3.0'); _sage_const_12346 = Integer(12346); _sage_const_12345 = Integer(12345); _sage_const_100000 = Integer(100000)###################################################
#  Program implementing Authentication Protocol   #
#           (Client side - Prover)                #
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
CTE  = _sage_const_100000    # Just a constant to be added to the constant term of poly
porc = _sage_const_1p0 /_sage_const_3p0   # Percentage of Oil variables

#######
PKFile = "PK32"                 # Base file name for public key
SKFile = "SK32"                 # Base file name for private key
Rep    = _sage_const_10                      # Number of times to repeat challenge
PORT_TX = _sage_const_12346                  # Port to transmit
PORT_RX = _sage_const_12345                  # Port to receive
#######

ss   = socket.socket()          # Create a socket object SERVER
host = socket.gethostname()     # Get local machine name
prt  = PORT_TX                  # Reserve a port for this service.
ss.bind((host, prt))            # Bind to the port

###################################################
#                     Main code                   #
###################################################
print "\n*************************************"
print "*                DEMO               *"
print "*  Authentication protocol: Prover  *"
print "*************************************"
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
k = PolynomialRing( GF(_sage_const_2 ), len(a) + len(x),
                    a + x )                 # Polynomial ring in 'n' variables
for i in range(n):          # Now 'a' and 'x' list have generators
    a[i] = k.gen(i)
    x[i] = k.gen(len(a) + i)
yUOV = vector(GF(_sage_const_2 ), o)     # Evaluation of polynomial (PiUOV)
Ms    = matrix(GF(_sage_const_2 ), n, n) # Matrix for affine transformation
vs    = vector(GF(_sage_const_2 ), n)    # Vector for affine transformation
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
ss.listen(_sage_const_5 )                    # Now wait for client connection.

for j in range( Rep ):
    ####### Receive 'y' vector:
    s     = socket.socket()         # Create a socket object
    host  = socket.gethostname()    # Get local machine name
    port  = PORT_RX                 # Reserve a port for the service.
    s.connect((host, port))
    aux = s.recv(_sage_const_1024 )
    s.close                         # Close the socket when done

    aux = aux.replace('(','').replace(')','').replace(',','')
    aux = aux.split()
    for i in range( o ):
        yUOV[i] = Integer(aux[i])
    print "{0}. Received y: {1}".format(j+_sage_const_1 , yUOV)

    ####### Find "X" solution for "yUOV":
    A = invertPiUOV(yUOV, o, v, a, PiUOV)
    xi = Sinv(Ms, A, vs)            # Compute 'xi'    
    
    ####### Receive 'p_LC':
    s     = socket.socket()         # Create a socket object
    host  = socket.gethostname()    # Get local machine name
    port  = PORT_RX                 # Reserve a port for the service.

    s.connect((host, port))
    aux = s.recv(_sage_const_1024 )
    s.close                         # Close the socket when done
    aux = aux.replace('[','').replace(']','').replace(',','')
    aux = aux.split()
    rxPolyIR = []
    for i in range( n + _sage_const_2  ):
        rxPolyIR.append( Integer( aux[i] ) )
    print "{0}. Received p_LC: {1}".format(j+_sage_const_1 , rxPolyIR)
    rxPoly = int2Poly(rxPolyIR, n, x)
    #compY = evalPoly(rxPoly, x, xi)
    compY = ZZ.random_element(_sage_const_2 )    # This will cause the authentication to fail
    print "Computed bit: {0}\n".format(compY)
    print "\nWaiting for connection"
    cc, addr = ss.accept()          # Establish connection with client.
    print "{0}. Got connection from {1}. Sending 'compY': {2}".\
          format(j+_sage_const_1 , addr, compY)
    cc.send( str(compY) )
    cc.close()                # Close connection
    
print "done!"
