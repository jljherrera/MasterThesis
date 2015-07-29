#!/bin/bash
# This script generates a private and public key to be used in
# the authentication or encryption protocol based in multivariate
# quadratic equations.
#
# A directory structure is necesary for this program to run properly:
# ./Keys:   With the programs to generate encrypted keys
# ./ASN.1:  It must have the programs to code an ASCII file to BER in ASN.1
# ./Base64: Here the programs to code a binary file (ASN.1 file) into Base64
#           must reside
# ./files:  Working directory, where produced files will be saved

echo $'\n'"    Program to generate Private and Public Keys"
echo $'\n'" Generates encrypted file with keys, decrypted files with"
echo      " integer representation of keys, BER/DER-ASN.1 coding of"
echo      " keys, a hash of the public key (integer representation)"
echo      " and finally a Base64 coding of the public key."

NoVars=$1

if test -z $NoVars 
then
   echo $'\n'"usage: "$0" NoVars"$'\n'
   exit 1
fi

echo $'\n'"Number of variables:" $NoVars

dirs="Keys ASN.1 Base64 files"
for i in `echo $dirs`
do
   aux=`file $i | cut -f2 -d" "`
   if test $aux != "directory"
   then 
      echo "Directory structure not good: " $i
      exit 2
   fi
done

####### Generate encrypted keys:
cd Keys
sage genkey.sage $NoVars Keys.enc
error=$?
if test $error != 0
then
   echo "An error ocurred while generating keys:" $error
   exit 3
fi

####### Generate private and public keys files:
sage getkeys.sage -S Keys.enc SK$NoVars PK$NoVars
error=$?
if test $error != 0
then
   echo "An error ocurred while decrypting keys:" $error
   exit 4
fi

####### Generate hash for public key:
python hashCL.py PK$NoVars.ir PK$NoVars.hash
error=$?
if test $error != 0
then
   echo "An error ocurred while hashing public key:" $error
   exit 5
fi

####### Moving generated files to "files" directory:
mv Keys.enc       ../files
mv SK$NoVars.ir   ../files
mv SK$NoVars.mv   ../files
mv PK$NoVars      ../files
mv PK$NoVars.ir   ../files
mv PK$NoVars.hash ../files

####### Generating BER/DER file:
cd ../
ASN.1/encode files/PK$NoVars.ir files/PK$NoVars.asn1
error=$?
if test $error != 0
then
   echo "An error ocurred while BER/DER'ing public key:" $error
   exit 5
fi

####### Generating Base64 file of ASN1 public key file:
Base64/ebase64 files/PK$NoVars.asn1 files/PK$NoVars.B64 "MQ Polynomials"
error=$?
if test $error != 0
then
   echo "An error ocurred while Base64'ing public key:" $error
   exit 6
fi

