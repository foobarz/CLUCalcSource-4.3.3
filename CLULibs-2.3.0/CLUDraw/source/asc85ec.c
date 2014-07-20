/* ASCII85 encoding for PostScript Level 2 */
/* Thomas Merz 1994 */

#include <stdio.h>
#include <fcntl.h>

#ifdef WIN32
#include <io.h>
#include <stdlib.h>
#endif

#include "psimage.h"

typedef unsigned char byte;

static unsigned char buf[4];
static unsigned long power85[5] = { 1L, 85L, 85L*85, 85L*85*85, 85L*85*85*85};

/* read 0-4 Bytes. result: number of bytes read */
static int 
ReadSomeBytes P1(FILE *, in)
{
  register int count, i;

  for (count = 0; count < 4; count++) {
    if ((i = getc(in)) == EOF)
      break;
    else
      buf[count] = (byte) i;
  }
  return count;
}

static void 
outbyte P2(byte, c, FILE *, out)
{    /* output one byte */
  static int outbytes = 0;

  if (fputc(c, out) == EOF) {
    //fprintf(stderr, "jpeg2ps: write error - exit!\n");
    return;
  }
  if (++outbytes > 63) {  	/* insert line feed */
    fputc('\n', out);
    outbytes = 0;
  }
}

int
ASCII85Encode P2(FILE *, in, FILE *, out)
{
  register int i, count;
  unsigned long word, v;

  /* 4 bytes read ==> output 5 bytes */
  while ((count = ReadSomeBytes(in)) == 4) {
    word = ((unsigned long)(((unsigned int)buf[0] << 8) + buf[1]) << 16) +
                   (((unsigned int)buf[2] << 8) + buf[3]);
    if (word == 0)
      outbyte('z', out);       /* shortcut for 0 */
    else
      /* calculate 5 ASCII85 bytes and output them */
      for (i = 4; i >= 0; i--) {
	v = word / power85[i];
	outbyte((byte)v + '!', out);
	word -= v * power85[i];
      }
  }

  word = 0;

  if (count != 0) {   /* 1-3 bytes left */
    for (i = count-1; i >= 0; i--)   /* accumulate bytes */
      word += (unsigned long)buf[i] << 8 * (3-i);
    
    /* encoding as above, but output only count+1 bytes */
    for (i = 4; i >= 4-count; i--) {
      v = word / power85[i];
      outbyte((byte)v + '!', out);
      word -= v * power85[i];
    }
  }

  fputc('~', out);	/* EOD marker */
  fputc('>', out);
  return 0;
}
