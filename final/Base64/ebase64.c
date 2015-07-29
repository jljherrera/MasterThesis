/* ------------------------------------------------------------------------ *
 * File:        ebase64_						    *
 * Purpose:     Encoding strings with base64                                *
 * Author:      Jose Luis Herrera Garcia                                    *
 * ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CH_LINE 64	// Characters per line in encoded file

/* ---- Base64 Encoding/Decoding Table --- */
char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* encodeblock - encode 3 8-bit binary bytes as 4 '6-bit' characters */
void encodeblock( unsigned char in[], char b64str[], int len ) {
    unsigned char out[5];
    out[0] = b64[ in[0] >> 2 ];
    out[1] = b64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? b64[ ((in[1] & 0x0f) << 2) |
             ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ in[2] & 0x3f ] : '=');
    out[4] = '\0';
    strncat(b64str, out, sizeof(out));
}

/* encode - base64 encode a stream, adding padding if needed */
void b64_encode(unsigned char *clrstr, char *b64dst, int toEncode)
{
  b64dst[0] = '\0';
  if( toEncode ) {
    encodeblock( clrstr, b64dst, toEncode );
  }
}

int main(int ac, char **av) {
  typedef unsigned char uchar;
  uchar mysrc[4];
  uchar myb64[5];
  FILE *fp, *fpout;
  const char *filename;
  const char *outFile;
  const char *text;
  int i, nchars;
  char ch[1];
  const int ch_line = ( CH_LINE * 3 ) / 4;

  if (ac < 3)
  {
    printf("\nusage: %s BinaryFile Base64File [\"some text\"]\n\n", av[0]);
    exit(1);
  }
  else
  {
    filename = av[1];
    outFile  = av[2];
    fp = fopen(filename, "rb");
    if (!fp) 
    {
      perror(filename);
      exit(1);
    }

    fpout = fopen(outFile, "wb");
    if (!fpout)
    {
      perror(outFile);
      exit(1);
    }
    if ( ac == 4 )
    {
      text = av[3];
      fprintf(fpout, "-----BEGIN %s-----\n", text);
    }
  }

/************ ENCODE ***********/
  printf("\nEncoding: %s\n", filename);
  i = 0;
  nchars = 0;
  while ( 1 == fread(ch, 1, 1, fp) )
  {
    mysrc[i] = ch[0];
    i++;
    nchars++;
    if ( i == 3 )
    {
      mysrc[i] = '\0';
      b64_encode(mysrc, myb64, i);
      i = 0;
      fprintf(fpout, "%s", myb64);
    }
    if ( nchars == ch_line )
    {
      nchars = 0;
      fprintf(fpout, "\n");
    }
  }
  if ( i != 0 )
  {
    mysrc[i] = '\0';
    b64_encode(mysrc, myb64, i);
    i = 0;
    fprintf(fpout, "%s\n", myb64);
  }
  else
  {
    fprintf(fpout, "\n");
  }
  if ( ac == 4 )
  {
    fprintf(fpout, "-----END %s-----\n", text);
  }

  fclose(fp);
  fclose(fpout);
  printf("Encoded in: %s\n\n", outFile);
  
  return 0;
}
