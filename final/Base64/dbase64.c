/* ------------------------------------------------------------------------ *
 * File:        dbase64_						    *
 * Purpose:     Decoding strings with base64                                *
 * Author:      Jose Luis Herrera Garcia                                    *
 * ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CH_LINE 64	// Characters per line in encoded file

/* ---- Base64 Encoding/Decoding Table --- */
char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* decodeblock - decode 4 '6-bit' characters into 3 8-bit binary bytes */
void decodeblock(unsigned char in[], char *clrstr) {
  unsigned char out[4];
  int i;
  out[0] = in[0] << 2 | in[1] >> 4;
  out[1] = in[1] << 4 | in[2] >> 2;
  out[2] = in[2] << 6 | in[3] >> 0;
  out[3] = '\0';

  for (i=0; i<4; i++)
  {
    clrstr[i] = out[i];
  }
}

int b64_decode(char *b64src, char *clrdst) {
  int c, phase, i;
  unsigned char in[4];
  char *p;
  clrdst[0] = '\0';
  phase = 0; i=0;
  while(b64src[i]) {
    c = (int) b64src[i];
    if(c == '=') {
      decodeblock(in, clrdst); 
      break;
    }
    p = strchr(b64, c);
    if(p) {
      in[phase] = p - b64;
      phase = (phase + 1) % 4;
      if(phase == 0) {
        decodeblock(in, clrdst);
        in[0]=in[1]=in[2]=in[3]=0;
      }
    }
    i++;
  }
  return i-1;
}


int main(int ac, char **av) {
  typedef unsigned char uchar;
  uchar myb64[5];
  uchar mydst[4];
  FILE *fpout, *fpdec;
  const char *outFile;
  const char *decFile;
  int i, j;
  char ch;

  if (ac != 3)
  {
    printf("\nusage: %s EncodedFile DecodedFile\n\n", av[0]);
    exit(1);
  }
  else
  {
    outFile  = av[1];
    decFile  = av[2];
  }

/***** Open files *****/
  fpout = fopen(outFile, "rb");
  if (!fpout)
  {
    perror(outFile);
    exit(1);
  }
  fpdec = fopen(decFile, "wb");
  if (!fpdec)
  {
    perror(decFile);
    exit(1);
  }
 
/************ DECODE ***********/
  printf("\nDecoding: %s\n", outFile);
  i = 0;
  while ( ( ch = fgetc(fpout) ) != EOF )
  {
    myb64[i] = ch;
    if ( ch == '-' )	// Skip header and footer
    {
      while ( ( ch = fgetc(fpout) ) != '\n' );
    }
    if ( ch == '\n' )
    {
      continue;
    }
    i++;
    if ( i == 4 )
    {
      myb64[i] = '\0';
      i = 0;
      j = b64_decode(myb64, mydst);
      fwrite(mydst, 1, j, fpdec);
    }
  }
  fclose(fpout);
  fclose(fpdec);
  printf("Decoded in: %s\n\n", decFile);

  return 0;
}
