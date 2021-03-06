###################################################
#   Program implementing Encryption Protocol      #
#           (Server side - Beto)                  #
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
#                    Functions                    #
###################################################
def genY(dim):
    ''' Randomly generates a {0,1} vector "Y" of dimension "dim" '''
    Y = vector(GF(2), dim)
    found = False
    while ( not found ):
        for i in range( dim ):
            Y[i] = ZZ.random_element(2)
            if ( Y[i] == 1 ): found = True # At least one "1" in Y!!!
    return Y

###################################################
#                  Initialization                 #
###################################################
extPI = ".ir"   # File name extension for files with polys represented as ints
CTE  = 100000   # Just a constant to be added to the constant term of poly

#######
PKFile = "PK32"                 # Base file name for public key
PORT   = 12345
#######

s    = socket.socket()          # Create a socket object
host = socket.gethostname()     # Get local machine name
port = PORT                     # Reserve a port for this service.
s.bind((host, port))            # Bind to the port

###################################################
#                     Main code                   #
###################################################
print "\n************************************************************************"
print "*                         ENCRYPTION - DEMO                            *"
print "*Sender of an encrypted binary string: Encrypting a bit in a polynomial*"
print "************************************************************************"
####### Reading Public Key file represented as integers
print "\nReading file with Integer Representation of a public polynomial \
system ({0}{1})\n".format(PKFile, extPI)
n, o, pubIntRep = readInt(PKFile + extPI)
print "Number of variables:        ", n
print "Number of vinegar variables:", n-o
print "Number of oil variables:    ", o

####### Generate randomly a string to encrypt
print "A {0} bits string to encrypt, generated randomly".format(o)
Encryp = genY( o )
print "String to encrypt: ", Encryp

####### Define variables for polynomial environment
print "\nDefining data structures for polynomial management"
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

###################################################
#                 Code for protocol               #
###################################################
s.listen(5)                     # Now wait for client connection.

for i in range( len(Encryp) ):
    y = genY(o)         # Generate a random image for Pi:x \mapsto y
    print "\nWaiting for connection"
    c, addr = s.accept()            # Establish connection with client.
    print "{0}. Got connection from {1}. Sending 'y': {2}".\
          format(i+1, addr, y)
    c.send( str(y) )
    c.close()                # Close connection

    currBit = Encryp[i]
    found = False
    while ( not found ):
        indexPolys = selIdxPoly(Pi, selNumPoly(Pi))
        poly2Solve, newResult = sumPolys(Pi, y, indexPolys)
        if ( newResult == currBit ): found = True

    print "Waiting for connection"
    c, addr = s.accept()     # Establish connection with client.
    polyIR  = poly2Int(poly2Solve, n, 'x')
    print "{0}. Got connection from {1}. Sending p_LC: {2}".\
                 format(i+1, addr, polyIR)
    c.send( str(polyIR) )
    c.close()                # Close the connection

    
s.close()
print "\ndone!\n"
