/**************************************************
*   Program to DECODE an ASN.1 file to Integer PK *
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
#define  MAX_LENGTH_CMD 80		// Max. number of characters for a build parameter

void         clrBuff(char *, int);

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
  unsigned char  buffer[MAX_LENGTH_INT];
  unsigned char  newBuffer[2*MAX_LENGTH_INT];
  char  params[MAX_LENGTH_CMD];
  char  str[MAX_LENGTH_VAR];	// Number of polynomial or integer of a polynomial
  char  n[MAX_LENGTH_VAR];	// Number of variables as a string
  char  o[MAX_LENGTH_VAR];	// Number of polynomials as a string
  int   nn;			// Number of variables as an integer
  int   oo;			// Number of polynomials as an integer
  int   i, j;
  int   result, der_len;
  unsigned char der[MAX_DER_SIZE], ch[1];

/******* Define input and output files *******/
  if (argc < 3)
  {
    printf("\nusage: %s ASN1BinaryFile PublicKeyIntFile\n\n", argv[0]);
    exit(1);
  }
  else
  {
    fileNameIn   = argv[1];
    fileNameOut  = argv[2];
    fpin = fopen(fileNameIn, "rb");
    if (!fpin) 
    {
      printf("\nError openning file: %s\n\n", fileNameIn);
      exit(1);
    }

    fpout = fopen(fileNameOut, "w");
    if (!fpout)
    {
      fclose(fpin);
      printf("\nError openning file: %s\n\n", fileNameOut);
      exit(1);
    }
  }
  printf("\nDecoding an ASN.1 file (%s), into a text file (%s)\n", fileNameIn, fileNameOut);

/******* ASN.1 initialization *******/

  ASN1_TYPE PK_def = ASN1_TYPE_EMPTY;
  ASN1_TYPE PK_dec = ASN1_TYPE_EMPTY;
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
  result = asn1_create_element(PK_def, "PKInt.PublicKey", &PK_dec);
  if ( result != ASN1_SUCCESS )
  {
    printf("\nCould not create a new element to DECODE: %d\n", result);
    exit(1);
  }

/******* Read data from input file *******/
  der_len = i = 0;
  while ( 1 == fread(ch, 1, 1, fpin) )
  {
    der[i] = ch[0];
    i++;
    der_len++;
  }

  result = asn1_der_decoding(&PK_dec, der, der_len, errorDescription);
  if ( result != ASN1_SUCCESS )
  {
    printf("\nCould not make a DER decoding: %d\n", result);
    printf("Error description: %s\n", errorDescription);
    asn1_perror(result);
    exit(2);
  }

  /*printf("------------DEC-----------------\n");
  asn1_print_structure(stdout, PK_dec, "", ASN1_PRINT_ALL);
  printf("--------------------------------\n");*/

  clrBuff(buffer, MAX_LENGTH_INT);	// Necesary because asn1 functions just put data in buffer
					// without terminating it with 0x0!!!
  der_len = MAX_LENGTH_INT;
  result = asn1_read_value(PK_dec, "noVars", buffer, &der_len);
  if ( result != ASN1_SUCCESS )
  {
    printf("\nCould not make a DER decoding (noVars): %d\n", result);
    printf("Error description: %s\n", errorDescription);
    exit(1);
  }
  nn = atoi(buffer);

  clrBuff(buffer, MAX_LENGTH_INT);
  der_len = MAX_LENGTH_INT;
  result = asn1_read_value(PK_dec, "noPoly", buffer, &der_len);
  if ( result != ASN1_SUCCESS )
  {
    printf("\nCould not make a DER decoding (noPoly): %d\n", result);
    printf("Error description: %s\n", errorDescription);
    exit(1);
  }
  oo = atoi(buffer);
  printf("Number of variables:   %d\nNumber of polynomials: %d\n", nn, oo);

/******* Write 'n' and 'o' to output file ******/
  sprintf(str, "%d", nn);
  fprintf(fpout, "%s\n", str);
  sprintf(str, "%d", oo);
  fprintf(fpout, "%s\n", str);

// ******* Read all integers representing the polynomials *******
  nn = nn + 2;			// +2 because of linear terms vector and constant term
  for ( i=0; i<oo; i++ )
  {
    for ( j=0; j<nn; j++)
    {
      der_len = MAX_LENGTH_INT;
      sprintf(o, "%d", i+1);		// Number of polynomial as string in 'o'
      sprintf(n, "%d", j+1);		// Number of integer as string in 'n'
      strcpy(params, "polys.?");
      strcat(params, o);
      strcat(params, ".intData.?");
      strcat(params, n);
      clrBuff(buffer, MAX_LENGTH_INT);
      result = asn1_read_value(PK_dec, params, buffer, &der_len);

      /******* Write that integer (as a string) to output file *******/
      fprintf(fpout, "%s", buffer);
      fprintf(fpout, " ");
      if ( j == nn - 1 )
        fprintf(fpout, "\n");

      if ( result != ASN1_SUCCESS )
      {
        asn1_perror(result);
        printf("Could not write value in 'intData', i: %d\n", i);
        exit(4);
      }
    }
  }

  printf("...Done!\n\n");

/******* Closing program *******/
  asn1_delete_structure(&PK_dec);
  asn1_delete_structure(&PK_def);

  fclose(fpin);
  fclose(fpout);
  return 0;
}


void clrBuff(char * buffer, int lenBuff)
{
  int i;

  for (i=0; i<lenBuff; i++)
    buffer[i] = 0x0;
}
