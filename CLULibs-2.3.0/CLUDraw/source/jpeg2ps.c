/* ----------------------------------------------------------------
 * jpeg2ps: convert JPEG files to compressed PostScript Level 2 EPS
 * (C) 1994-96 Thomas Merz 
 *
 * 1.0  Dec. 94  first public release
 *
 * 1.1  Jan. 95  Several improvements:
 *               - invert color components for Adobe Photoshop CMYK
 *                 files; check APP14 marker to detect Adobe files.
 *               - process SOF2-SOF15 compression markers; print
 *                 warning because these are not guaranteed to work
 *                 in all PostScript Level 2 interpreters.
 *                 Files with compression types other than SOF0 and 
 *                 SOF1 have not really been tested yet.
 *               - Look for JFIF marker APP0 and use resolution
 *                 values, if any.
 * 1.2  Jan. 95  Fixed "unsigned" bug when reading JFIF density 
 *		 marker
 * 1.3  Jan. 95  Fixed bug in ASCII85 encoding (asc85ec.c). The
 *               ASCII85 EOF markers ~> could get separated in
 *               rare cases.
 * --------------------------------------------------------------*/

#define JPEG2PS_VERSION		"V1.3"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

#include "psimage.h"

#ifdef WIN32
#define READMODE "rb"           /* read JPEG files in binary mode */
#define WRITEMODE "wb"
#else
#define READMODE "r"
#define WRITEMODE "w"
#endif

#define A4
#ifdef A4
int PageHeight = 842;           /* a4 page */
int PageWidth  = 595;           /* a4 page */
#else
int PageHeight = 792;           /* letter page */
int PageWidth  = 612;           /* letter page */
#endif

int Margin     = 20;            /* safety margin */



extern  BOOL	AnalyzeJPEG (imagedata *image);
extern  int	ASCII85Encode (FILE *in, FILE *out);

BOOL JPEGtoPS(imagedata* JPEG, FILE *PSfile);

BOOL ConvertJPEGtoPS(const char* pcSrcFilename, const char* pcTrgFilename) 
{
	int opt;
	imagedata image;
	FILE *pTrgFile;

	image.filename = NULL;
	image.mode     = ASCII85;
	image.startpos = 0L;
	image.landscape= FALSE;
	image.dpi      = DPI_IGNORE;
	image.adobe    = FALSE;
	image.filename = (char *) pcSrcFilename;

	if (!image.filename)
		return FALSE;

	if ((image.fp = fopen(image.filename, READMODE)) == NULL) 
	{
		return FALSE;
	}

	if ((pTrgFile = fopen(pcTrgFilename, WRITEMODE)) == NULL) 
	{
		return FALSE;
	}

	if (!JPEGtoPS(&image, pTrgFile))   
		return FALSE;

	fclose(pTrgFile);
	fclose(image.fp);

	return TRUE;
}


#define BUFFERSIZE 1024
static char buffer[BUFFERSIZE];

/* We don't really need this because we use ASCII85 */
static void 
ASCIIHexEncode P2(FILE *, in, FILE *, out) {
  static char BinToHex[] = "0123456789ABCDEF";
  int i, CharsPerLine;
  size_t n;
  unsigned char *p;

  CharsPerLine = 0;
  putc('\n', out);

  while ((n = fread(buffer, 1, sizeof(buffer), in)) != 0)
    for (i = 0, p = (unsigned char *) buffer; i < n; i++, p++) {
      putc(BinToHex[*p>>4], out);           /* first nibble  */
      putc(BinToHex[*p & 0x0F], out);       /* second nibble */
      if ((CharsPerLine += 2) >= 64) {
        putc('\n', out);
        CharsPerLine = 0;
      }
    }

  putc('>', out);         /* EOD marker for PostScript hex strings */
}

static char *ColorSpaceNames[] = {"", "Gray", "", "RGB", "CMYK" };

BOOL 
JPEGtoPS (imagedata *JPEG, FILE *PSfile) 
{
  int llx, lly, urx, ury;        /* Bounding box coordinates */
  size_t n;
  float scale, sx, sy;           /* scale factors            */
  time_t t;
  int i;

  if (!AnalyzeJPEG(JPEG))       /* read image parameters and fill JPEG struct*/
  {
    return FALSE;
  }
/*
  fprintf(stderr, "Note on file '%s': %dx%d pixel, %d color component%s\n",
    JPEG->filename, JPEG->width, JPEG->height, JPEG->components,
    (JPEG->components == 1 ? "" : "s"));
*/
  /* "Use resolution from file" was requested, but we couldn't find any */
  if (JPEG->dpi == DPI_USE_FILE) 
  { 
    //fprintf(stderr, "Note: no resolution values found in JPEG file - using standard scaling.\n");
    JPEG->dpi = DPI_IGNORE;
  }

  if (JPEG->dpi == DPI_IGNORE) 
  {
    if (JPEG->width > JPEG->height) 	/* switch to landscape if needed */
	{
      JPEG->landscape = TRUE;
     // fprintf(stderr, 
//      	"Note: image width exceeds height - producing landscape output!\n");
    }
    if (!JPEG->landscape)        /* calculate scaling factors */
	{
      sx = (float) (PageWidth - 2*Margin) / JPEG->width;
      sy = (float) (PageHeight - 2*Margin) / JPEG->height;
    }
	else 
	{
      sx = (float) (PageHeight - 2*Margin) / JPEG->width;
      sy = (float) (PageWidth - 2*Margin) / JPEG->height;
    }
    scale = min(sx, sy);	/* We use at least one edge of the page */
  } 
  else 
  {
   // fprintf(stderr, "Note: Using resolution %d dpi.\n", (int) JPEG->dpi);
    scale = 72 / JPEG->dpi;     /* use given image resolution */
  }

  if (JPEG->landscape) 
  {
    /* landscape: move to (urx, lly) */
    urx = PageWidth - Margin;
    lly = Margin;
    ury = (int) (Margin + scale*JPEG->width + 0.9);    /* ceiling */
    llx = (int) (urx - scale * JPEG->height);          /* floor  */
  }
  else 
  {
    /* portrait: move to (llx, lly) */
    llx = lly = Margin;
    urx = (int) (llx + scale * JPEG->width + 0.9);     /* ceiling */
    ury = (int) (lly + scale * JPEG->height + 0.9);    /* ceiling */
  }

  time(&t);

  /* produce EPS header comments */
  fprintf(PSfile, "%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(PSfile, "%%%%Creator: jpeg2ps %s by Thomas Merz\n", JPEG2PS_VERSION);
  fprintf(PSfile, "%%%%Title: %s\n", JPEG->filename);
  fprintf(PSfile, "%%%%CreationDate: %s", ctime(&t));
  fprintf(PSfile, "%%%%BoundingBox: %d %d %d %d\n", 
                   llx, lly, urx, ury);
  fprintf(PSfile, "%%%%DocumentData: %s\n", 
                  JPEG->mode == BINARY ? "Binary" : "Clean7Bit");
  fprintf(PSfile, "%%%%LanguageLevel: 2\n");
  fprintf(PSfile, "%%%%EndComments\n");
  fprintf(PSfile, "%%%%BeginProlog\n");
  fprintf(PSfile, "%%%%EndProlog\n");
  fprintf(PSfile, "%%%%Page: 1 1\n");

  fprintf(PSfile, "/languagelevel where {pop languagelevel 2 lt}");
  fprintf(PSfile, "{true} ifelse {\n");
  fprintf(PSfile, "  (JPEG file '%s' needs PostScript Level 2!",
                  JPEG->filename);
  fprintf(PSfile, "\\n) dup print flush\n");
  fprintf(PSfile, "  /Helvetica findfont 20 scalefont setfont ");
  fprintf(PSfile, "100 100 moveto show showpage stop\n");
  fprintf(PSfile, "} if\n");

  fprintf(PSfile, "save\n");
  fprintf(PSfile, "/RawData currentfile ");

  if (JPEG->mode == ASCIIHEX)            /* hex representation... */
    fprintf(PSfile, "/ASCIIHexDecode filter ");
  else if (JPEG->mode == ASCII85)        /* ...or ASCII85         */
    fprintf(PSfile, "/ASCII85Decode filter ");
  /* else binary mode: don't use any additional filter! */

  fprintf(PSfile, "def\n");

  fprintf(PSfile, "/Data RawData << ");
  fprintf(PSfile, ">> /DCTDecode filter def\n");

  /* translate to lower left corner of image */
  fprintf(PSfile, "%d %d translate\n", (JPEG->landscape ? 
                   PageWidth - Margin : Margin), Margin);

  if (JPEG->landscape)                 /* rotation for landscape */
    fprintf(PSfile, "90 rotate\n");
      
  fprintf(PSfile, "%.2f %.2f scale\n", /* scaling */
                   JPEG->width * scale, JPEG->height * scale);
  fprintf(PSfile, "/Device%s setcolorspace\n", 
                  ColorSpaceNames[JPEG->components]);
  fprintf(PSfile, "{ << /ImageType 1\n");
  fprintf(PSfile, "     /Width %d\n", JPEG->width);
  fprintf(PSfile, "     /Height %d\n", JPEG->height);
  fprintf(PSfile, "     /ImageMatrix [ %d 0 0 %d 0 %d ]\n",
                  JPEG->width, -JPEG->height, JPEG->height);
  fprintf(PSfile, "     /DataSource Data\n");
  fprintf(PSfile, "     /BitsPerComponent %d\n", 
                  JPEG->bits_per_component);

  /* workaround for color-inverted CMYK files produced by Adobe Photoshop:
   * compensate for the color inversion in the PostScript code
   */
  if (JPEG->adobe && JPEG->components == 4) {
    //fprintf(stderr, "Note: Adobe-conforming CMYK file - applying workaround for color inversion.\n");
    fprintf(PSfile, "     /Decode [1 0 1 0 1 0 1 0]\n");
  }else {
    fprintf(PSfile, "     /Decode [0 1");
    for (i = 1; i < JPEG->components; i++) 
      fprintf(PSfile," 0 1");
    fprintf(PSfile, "]\n");
  }

  fprintf(PSfile, "  >> image\n");
  fprintf(PSfile, "  Data closefile\n");
  fprintf(PSfile, "  RawData flushfile\n");
  fprintf(PSfile, "  showpage\n");
  fprintf(PSfile, "  restore\n");
  fprintf(PSfile, "} exec");

  /* seek to start position of JPEG data */
  fseek(JPEG->fp, JPEG->startpos, SEEK_SET);

  switch (JPEG->mode) {
  case BINARY:
    /* important: ONE blank and NO newline */
    fprintf(PSfile, " ");
#ifdef DOS
    fflush(PSfile);         	  /* up to now we have CR/NL mapping */
    setmode(fileno(PSfile), O_BINARY);    /* continue in binary mode */
#endif
    /* copy data without change */
    while ((n = fread(buffer, 1, sizeof(buffer), JPEG->fp)) != 0)
      fwrite(buffer, 1, n, PSfile);
#ifdef DOS
    fflush(PSfile);                  	/* binary yet */
    setmode(fileno(PSfile), O_TEXT);    /* text mode */
#endif
    break;

  case ASCII85:
    fprintf(PSfile, "\n");

    /* ASCII85 representation of image data */
    if (ASCII85Encode(JPEG->fp, PSfile)) 
	{
    //  fprintf(stderr, "Error: internal problems with ASCII85Encode!\n");
		return FALSE;
    }
    break;

  case ASCIIHEX:
    /* hex representation of image data (not really used) */
    ASCIIHexEncode(JPEG->fp, PSfile);
    break;
  }
  fprintf(PSfile, "\n%%%%EOF\n");

  return TRUE;
}
