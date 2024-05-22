
/************ I usually put these in a file called format.h ****/

#define LO_RES

#ifdef HI_RES
#define XMAX 640
#define YMAX 200
#define PLANES 4
#define MAXCOL (1<<PLANES)
#define XASPECT 5
#define YASPECT 11
#endif

#ifdef LO_RES
#define XMAX 320
#define YMAX 200
#define PLANES 5
#define MAXCOL (1<<PLANES)
#define XASPECT 10
#define YASPECT 11
#endif

/* EA handy make a long from 4 chars macros redone to work with Aztec*/
#ifndef MAKE_ID
#define MAKE_ID(a, b, c, d)\
    ( ((long)(a)<<24) + ((long)(b)<<16) + ((long)(c)<<8) + (long)(d) )
#endif

/* these are the IFF types I deal with */
#define FORM MAKE_ID('F', 'O', 'R', 'M')
#define ILBM MAKE_ID('I', 'L', 'B', 'M')
#define BMHD MAKE_ID('B', 'M', 'H', 'D')
#define CMAP MAKE_ID('C', 'M', 'A', 'P')
#define BODY MAKE_ID('B', 'O', 'D', 'Y')

/* and these are the IFF types I ignore but don't squawk about */
#define GRAB MAKE_ID('G', 'R', 'A', 'B')
/*#define DEST MAKE_ID('D', 'E', 'S', 'T')*/
#define SPRT MAKE_ID('S', 'P', 'R', 'T')
#define CAMG MAKE_ID('C', 'A', 'M', 'G')
#define CRNG MAKE_ID('C', 'R', 'N', 'G')
#define CCRT MAKE_ID('C', 'C', 'R', 'T')
#define DPPV MAKE_ID('D', 'P', 'P', 'V')

#define ANIM MAKE_ID('A', 'N', 'I', 'M')
#define ANHD MAKE_ID('A', 'N', 'H', 'D')
#define DLTA MAKE_ID('D', 'L', 'T', 'A')

/* Some macros for raster memory allocation ... redefine if you're
   sensible and manage memory locally */

/* ralloc - raster alloc*/
#define ralloc(amount)  (PLANEPTR)AllocMem((long)(amount), MEMF_CHIP)
/* rfree - raster free*/
#define rfree(pt, amount)   FreeMem( (pt), (long)(amount) )

/*line_bytes = the number of words * 2 (for bytes) a raster line takes up */
#define line_bytes(width)   (((width+15)>>4)<<1)

/* psize - plane size in bytes (an even number) of a raster given
   width and height */
#define psize(width, height) ( line_bytes(width)*height)

/* the place to throw excess bits */
#define bit_bucket(hdlr, length) my_Seek(hdlr, (long)(length), OFFSET_CURRENT)


union bytes4
    {
    char b4_name[4];
    long b4_type;
    };

struct iff_chunk
    {
    union bytes4 iff_type;
    long iff_length;
    };

struct form_chunk
    {
    union bytes4 fc_type; /* == FORM */
    long fc_length;
    union bytes4 fc_subtype;
    };

struct old_BitMapHeader
    {
    UWORD w, h;
    UWORD x, y;
    UBYTE nPlanes;
    UBYTE masking;
    UBYTE compression;
    UBYTE pad1;
    UWORD transparentColor;
    UBYTE xAspect, yAspect;
    WORD pageWidth, pageHeight;
    };

/*ILBM_info is the structure read_iff returns, and is hopefully all
  you need to deal with out of the iff reader routines below*/
struct ILBM_info
    {
    struct old_BitMapHeader header;
    UBYTE   *cmap;  /*say hey aztec don't like odd length structures*/
    struct BitMap bitmap;
    int Nb_Col;
    unsigned long   int camg;
    };


/* I sure wish C function "prototypes" were real and not just ANSI */
extern int  write_iff(char *name, unsigned char *colors, struct BitMap *bits,
    short xoff, short yoff, short width, short compressed); 

/* Anyone know where some useful minterms are defined? */
#define COPY_MINTERM        0xc0

/***

        A meditation for the guru from the Diamond Sutra -

        So shall you think of all this fleeting world:
        A star at dawn, a bubble in a stream;
        A flash of lightning in a summer cloud,
        A flickering lamp, a phantom, and a dream.

***/


extern struct ILBM_info *read_iff( char *name,short just_colors);

extern  void free_planes(register struct BitMap *bmap);
