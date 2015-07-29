/**************************************************
*   Program to ENCODE an Integer PK file to ASN.1 *
*                                                 *
*   Jose Luis Juan Herrera Garcia       May, 2015 *
*   Thesis work                                   *
*   Computer science graduate studies             *
**************************************************/
/**************************************************
*  Because of limitations of the library libtasn1 *
*  this program can only encode integers numbers  *
*  less than 63 bits !!!                          *
**************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtasn1.h"

#define  MAX_LENGTH_INT 100		// Max. number of digits of a number in file
#define  MAX_LENGTH_VAR 4		// Max. number of digits in a variable (n, o)
#define  MAX_DER_SIZE   150000		// Max. number of bytes in DER buffer

extern const ASN1_ARRAY_TYPE pkInt_asn1_tab[];

/********************************************************/
/* Function : main                                      */
/********************************************************/
int
main (int argc, char *argv[])
{
  FILE *fpin, *fpout;
  const char *fileNameIn;
  const char *fileNameOut;
  char  buffer[MAX_LENGTH_INT];
  char  n[MAX_LENGTH_VAR];	// Number of variables as a string
  char  o[MAX_LENGTH_VAR];	// Number of polynomials as a string
  int   nn;			// Number of variables as an integer
  int   oo;			// Number of polynomials as an integer
  int   i, j;
  int   result, der_len;
  unsigned char der[MAX_DER_SIZE];

/******* Define input and output files *******/
  if (argc < 3)
  {
    printf("\nusage: %s PublicKeyIntFile ASN1BinaryFile \n\n", argv[0]);
    exit(1);
  }
  else
  {
    fileNameIn   = argv[1];
    fileNameOut  = argv[2];
    fpin = fopen(fileNameIn, "r");
    if (!fpin) 
    {
      printf("\nError openning file: %s\n\n", fileNameIn);
      exit(1);
    }

    fpout = fopen(fileNameOut, "wb");
    if (!fpout)
    {
      fclose(fpin);
      printf("\nError openning file: %s\n\n", fileNameOut);
      exit(1);
    }
  }
  printf("\nEncoding: %s, into an ASN.1 file: %s\n", fileNameIn, fileNameOut);

/******* Read data from input file *******/
  if ( fscanf(fpin, "%s", buffer) != EOF ) strcpy(n, buffer);	// Read number of variables
  else
  {
    printf("\nData error (n variable), reading file: %s\n\n",fileNameIn);
    fclose(fpin);
    fclose(fpout);
    exit(2);
  }

  if ( fscanf(fpin, "%s", buffer) != EOF ) strcpy(o, buffer);	// Read number of polynomials
  else
  {
    printf("\nData error (o variable), reading file: %s\n\n",fileNameIn);
    fclose(fpin);
    fclose(fpout);
    exit(2);
  }

  nn = atoi(n) + 2;	// +2 because linear terms vector and constant term
  oo = atoi(o);

  char **polys = (char **)malloc(sizeof(char *) * oo);
  for ( i=0; i<oo; i++)
    polys[i] = (char *)malloc(sizeof(char) * MAX_LENGTH_INT * nn);

  i = j = 0;
  while ( fscanf(fpin, "%s", buffer) != EOF )	// Read integers representing each polynomial
  {
      if ( i == oo )
      {
        printf("\nToo much data in file: %s\n\n", fileNameIn);
        exit(3);
      }
      strcpy( &polys[i][j * MAX_LENGTH_INT], buffer );
      j++;
      if ( j == nn )
      {
        j = 0;
        i++;
      }
  }

/******* ASN.1 initialization *******/

  ASN1_TYPE PK_def = ASN1_TYPE_EMPTY;
  ASN1_TYPE PK_enc = ASN1_TYPE_EMPTY;
  char errorDescription[ASN1_MAX_ERROR_DESCRIPTION_SIZE];

  // Creates definition structures needed to manage the ASN.1 definitions
  result = asn1_array2tree(pkInt_asn1_tab, &PK_def, errorDescription);

  if (result != ASN1_SUCCESS)
  {
    asn1_perror(result);
    printf ("%s\n", errorDescription);
    exit (1);
  }

  // Creates a structure of type "PKInt" (from *.asn definition file)
  result = asn1_create_element(PK_def, "PKInt", &PK_enc);
  if ( result != ASN1_SUCCESS )
  {
    printf("\nCould not create a new element to ENCODE: %d\n", result);
    exit(1);
  }

/******* Write 'n' and 'o' to the structure *******/
  result = asn1_write_value(PK_enc, "PublicKey.noVars", n, 0);
  if ( result != ASN1_SUCCESS )
  {
    printf("Could not write value in 'noVars': %d\n", result);
    exit(1);
  }

  result = asn1_write_value(PK_enc, "PublicKey.noPoly", o, 0);
  if ( result != ASN1_SUCCESS )
  {
    printf("Could not write value in 'noPoly': %d\n", result);
    exit(1);
  }

/******* Write all integers representing the polynomials *******/
  for ( i=0; i<oo; i++ )
  {
    result = asn1_write_value(PK_enc, "PublicKey.polys","NEW", 1);	// Create a new register
    for ( j=0; j<nn; j++)
    {
      result = asn1_write_value(PK_enc, "PublicKey.polys.?LAST.intData",\
                                "NEW", 1);	// Create a new field for an integer
      result = asn1_write_value(PK_enc, "PublicKey.polys.?LAST.intData.?LAST",\
                                &polys[i][j*MAX_LENGTH_INT], 0);
      //printf("Wrote %s in loop (%d,%d)\n", &polys[i][j*MAX_LENGTH_INT], i, j);
      if ( result != ASN1_SUCCESS )
      {
        asn1_perror(result);
        printf("Could not write value in 'intData', i: %d\n", i);
        exit(4);
      }
    }
  }

  /*printf("------------ENC-----------------\n");
  asn1_print_structure(stdout, PK_enc, "", ASN1_PRINT_ALL);
  printf("--------------------------------\n");*/

/******* Encode written information *******/
  der_len = MAX_DER_SIZE;
  result = asn1_der_coding(PK_enc, "PublicKey", der, &der_len, errorDescription);
  if ( result != ASN1_SUCCESS )
  {
    printf("\nCould not create DER coding: %d\n", result);
    printf("Error description: %s\n", errorDescription);
    printf("Length needed MAX_DER_SIZE = %d\n", der_len);
    exit(5);
  }

  printf("...Done!\n\n");

/******* Writing DER encoding to output file *******/
  fwrite(der, 1, der_len, fpout);

/******* Closing program *******/
  asn1_delete_structure(&PK_enc);
  asn1_delete_structure(&PK_def);

  for (i=0; i<oo; i++)
    free(polys[i]);
  free(polys);

  fclose(fpin);
  fclose(fpout);
  return 0;
}
