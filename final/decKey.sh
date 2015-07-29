#!/bin/bash
# This script decodes a Base64 public key file to its original 
# structure: Integer representation
#
# A directory structure is necesary for this program to run properly:
# ./Keys:   With the programs to generate encrypted keys
# ./ASN.1:  It must have the programs to code an ASCII file to BER in ASN.1
# ./Base64: Here the programs to code a binary file (ASN.1 file) into Base64
#           must reside
# ./files:  Working directory, where produced files will be saved

echo $'\n'"             Program to generate a Public Key"
echo $'\n'" From a Base64 file representing a Public Key. This script"
echo      " produces a file, with the original integer representation of"
echo      " the Public Key, producing first its ASN.1 binary represen"
echo      " tation. Then produces the hash of the public key and"
echo      " compares it with the received hash deciding if it is OK"

FileName=$1
hashFN=$2

if test -z $FileName 
then
   echo $'\n'"usage: "$0" FileNameB64 hashFileName"$'\n'
   exit 1
else
   if test -a files/$FileName
   then
      echo $'\n'"File files/"$FileName" exists"$'\n'
   else
      echo $'\n'"File files/"$FileName" does not exist"$'\n'
      exit 2
   fi
fi

if test -z $hashFN 
then
   echo $'\n'"usage: "$0" FileNameB64 hashFileName"$'\n'
   exit 1
else
   if test -a files/$hashFN
   then
      echo $'\n'"File files/"$hashFN" exists"$'\n'
   else
      echo $'\n'"File files/"$hashFN" does not exist"$'\n'
      exit 2
   fi
fi

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

####### Generating ASN1 public key file from Base64 file:
Base64/dbase64 files/$FileName files/$FileName.asn1
error=$?
if test $error != 0
then
   echo "An error ocurred while DeBase64'ing public key:" $error
   exit 6
fi

####### Generating Public Key file in integer representation:
ASN.1/decode files/$FileName.asn1 files/$FileName.ir
error=$?
if test $error != 0
then
   echo "An error ocurred while DeBER/DER'ing public key:" $error
   exit 5
fi

####### Generate hash for public key:
cd Keys
python hashCL.py ../files/$FileName.ir $FileName.hash
error=$?
if test $error != 0
then
   echo "An error ocurred while hashing public key:" $error
   exit 5
fi

####### Moving generated files to "files" directory:
mv $FileName.hash ../files

####### Comparing received hash file vs. generated hash file
cd ..
diff files/$hashFN files/$FileName.hash
error=$?
if test $error != 0
then
   echo "Hash of received public key INCORRECT!!!"
   echo "DO NOT USE this public key. It has been modified"
   exit 6
else
   echo "Hash of received public key correct"
   exit 0
fi

