
/**********************************************************************
*
*           jiff.c   Jim Kent's iff - ilbm  reader
*
* This is the (sortof) short (sortof) simple no-frills IFF reader
* to get something out of DPaint, Images, or the Animator.  It
* works well with the Aztec C compiler.  It should work with Lattice
* but you never know until you try it.  I haven't.
*
* I've included a simple main program.  This is just to make it
* stand alone.  Since amiga screen initializations are massive, all
* it does as is is read it into a BitMap, and then free up the BitMap.
* Should crash it if it's gonna crash though.
*
* The main interface to this is through the routine read_iff(filename).
* This returns a ILBM_info structure-pointer on success, and NULL on
* failure.  It cleans up after itself on failure.
*
* I hope you will find this useful and easy to use.  Please forgive
* my funky indentation style?  Well at least I'm consistent!
*
* To demonstrate what a nice guy I am even though I'm far from wild
* about the IFF standard I'm placing this in the public domain.  When
* you remove the DEBUG and PARANOID definitions the code is only
* 1536 bytes long.
*
*       -Jim Kent  April 22, 1986
************************************************************************/

#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <libraries/dos.h>
#include "jiff.h"


#define TAILLE_BUFFER   1024
char    buffer[TAILLE_BUFFER];
int nb=0;

/*this is an all too common state of software development.  Get rid
  of this define as soon as it runs*/
/*
#define DEBUG
*/
/*this is the normal state of software development.  Seriously undefine
  this to make it shut up about errors and reduce code size half way
  through beta testing... */
/*
#define PARANOID
*/
static struct ILBM_info *read_ilbm(), *read_body();

/* OK this code is almost re-entrant.  Pass this guy from above to
   make it really re-entrant.  (Why do you need a reentrant ILBM
   reader though??  Maybe for Dale ... ) */
static struct ILBM_info root_info;  /*static so get initialized to zero*/


#ifdef PARANOID
/* a little paranoid routine that say's where we got before EOF */
static void
iff_truncated(where)
int where;
{
free_planes(&root_info.bitmap);
}
#endif


int my_Read(hdlr,p,taille)
BPTR    hdlr;
char    *p;
int taille;
{
    int i;

    for(i=0;i<taille;i++){
        if(nb==0){
            Read(hdlr,&buffer,TAILLE_BUFFER);
            nb=TAILLE_BUFFER;
        }
        p[i]=buffer[TAILLE_BUFFER-nb];
        nb--;
    }
    return(taille);
}

int my_Seek(hdlr,taille,type)
BPTR    hdlr;
int taille,type;
{
    int i;

    if(taille<=nb){
        nb=nb-taille;
    }else{
        taille=taille-nb;
        Seek(hdlr,taille,type);
    }
}

BPTR    my_Open(name,mode)
char    *name;
long    int mode;
{
    nb=0;
    return(Open(name,mode));
}

struct ILBM_info *read_iff( char *name,short just_colors)
{
struct ILBM_info *info = &root_info;
BPTR hdlr;
struct form_chunk chunk;

    info->camg=0;
if ((hdlr = my_Open(name, MODE_OLDFILE) ) == 0)
    {
    return(NULL);
    }

if ( my_Read(hdlr,&chunk, sizeof(struct form_chunk)) !=sizeof(struct form_chunk))
    {
#ifdef PARANOID
    iff_truncated(0);
#endif
    Close(hdlr);
    return(NULL);
    }

if (chunk.fc_type.b4_type != FORM)
    {
    Close(hdlr);
    return(NULL);
    }

if (chunk.fc_subtype.b4_type != ILBM)
    {
#ifdef PARANOID
    printf("FORM not an ILBM - %s\n", name);
#endif
    Close(hdlr);
    return(NULL);
    }

#ifdef DEBUG
printf("FORM %ld ILBM\n", chunk.fc_length);
#endif

info = read_ilbm(hdlr, info, chunk.fc_length - sizeof(chunk), just_colors);
Close(hdlr);
return(info);
}

static
struct ILBM_info *
read_ilbm(hdlr, info, length, just_colors)
BPTR *hdlr;
struct ILBM_info *info;
long length;
short just_colors;
{
struct iff_chunk chunk;
int i;
long read_in = 0;
int got_header = FALSE;  /*to make sure gots the header first*/
int got_cmap = FALSE;  /*make sure get cmap before "BODY" */

/*make sure the Planes are all NULL so can free up memory easily
  on error abort */
for (i=0; i<8; i++)
    info->bitmap.Planes[i] = NULL;

while (read_in < length)
    {
    if (my_Read(hdlr,&chunk, sizeof(chunk)) !=sizeof(chunk))
        {
#ifdef PARANOID
        iff_truncated(1);
#endif
        return(NULL);
        }
    switch (chunk.iff_type.b4_type)
        {
        case BMHD:
#ifdef DEBUG
            printf("\tBMHD %ld\n", chunk.iff_length);
#endif
            if (my_Read(hdlr,&info->header, sizeof(info->header)) != sizeof(info->header))
                {
#ifdef PARANOID
                iff_truncated(2);
#endif
                return(NULL);
                }
            got_header = TRUE;
            break;
        case CAMG:
            my_Read(hdlr,&info->camg, 4);
            break;
        case CMAP:
            if (!got_header){
                    return(NULL);
            }
            info->cmap=(UBYTE *)malloc(chunk.iff_length/3);
            if(info->cmap){

                if(my_Read(hdlr,info->cmap, (int)chunk.iff_length) !=(int)chunk.iff_length){
                    return(NULL);
                }
                info->Nb_Col=chunk.iff_length/3;
            }else{
                info->Nb_Col=0;         
                bit_bucket(hdlr, chunk.iff_length);
            }
            got_cmap = TRUE;
            if (just_colors)
                return(info);
            break;
        case BODY:
            if (!got_cmap)
                {
                return(NULL);
                }
            return( read_body(hdlr, info, chunk.iff_length) );

        default:    /*squawk about unknown types if PARANOID */
        case GRAB:  /*ignore documented but unwanted types*/
/*        case DEST:  */
        case SPRT:
        case CRNG:
        case CCRT:
            bit_bucket(hdlr, chunk.iff_length);
            break;
        }
    read_in += chunk.iff_length + sizeof(chunk);
    }
return(NULL); 
}



static struct ILBM_info *read_body(
    BPTR    hdlr,
    register struct ILBM_info *info,
    long length
)
{
struct ILBM_header *header;
struct BitMap *bm;
int i, j;
int rlength;
int plane_offset;


/* ok a little more error checking */
if (info->header.compression != 0 && info->header.compression != 1)
    {
    return(NULL);
    }

/*set up the bitmap part that doesn't involve memory allocation first -
  hey this part does get done, and let's be optimistic...*/
info->bitmap.BytesPerRow = line_bytes(info->header.w);
info->bitmap.Rows = info->header.h;
info->bitmap.Depth = info->header.nPlanes;
info->bitmap.Flags = info->bitmap.pad = 0;

rlength = info->bitmap.Rows * info->bitmap.BytesPerRow;

for (i=0; i<info->header.nPlanes; i++)
    {
    if ((info->bitmap.Planes[i] = ralloc(rlength)) == NULL)
        {
        free_planes( &info->bitmap );
        return(NULL);
        }
    }
plane_offset = 0;
for (i=0; i<info->bitmap.Rows; i++)
    {
    /* this test should be in the inner loop for shortest code,
       in the outer loop for greatest speed, so sue me I compromised */
    if (info->header.compression == 0)
        {
        for (j = 0; j < info->bitmap.Depth; j++)
            {
            if ( my_Read(hdlr,info->bitmap.Planes[j] + plane_offset,
                info->bitmap.BytesPerRow) !=info->bitmap.BytesPerRow)
                {
#ifdef PARANOID
                iff_truncated(6);
#endif
                free_planes( &info->bitmap);
                return(NULL);
                }
            }
        }
    else
        {
        register char *dest;
        char    value;
        register int so_far, count;  /*how much have unpacked so far*/

        for (j = 0; j < info->bitmap.Depth; j++)
            {
            so_far = info->bitmap.BytesPerRow;
            dest = (char *)info->bitmap.Planes[j] + plane_offset;
            while (so_far > 0)
                {
                my_Read(hdlr,&value,1);
                if (value == 128)
                    {
                    }
                else if (value > 0)
                    {
                    count = (int)value + 1;
                    so_far -= count;
                    if ( my_Read(hdlr,dest, count) !=count)
                        {
#ifdef PARANOID
                        iff_truncated(7);
#endif
                        free_planes( &info->bitmap);
                        return(NULL);
                        }
                    dest += count;
                    }
                else 
                    {
                    count = (int)-value + 1;
                    so_far -= count;
                    my_Read(hdlr,&value,1);
                    while (--count >= 0)  /*this is fastest loop in C */
                        *dest++ = value;
                    }
                }
            if (so_far != 0)
                {
#ifdef PARANOID
                printf("compression quite screwed up, aborting %d\n", so_far);
#endif
                free_planes( &info->bitmap);
                return(NULL);
                }
            }
        }
    plane_offset += info->bitmap.BytesPerRow;
    }
return(info);
}


void free_planes(register struct BitMap *bmap)
{
PLANEPTR plane;
long length;
short i;

length = bmap->BytesPerRow * bmap->Rows;

for (i=0; i<8; i++)
    if ( (plane = bmap->Planes[i]) != NULL)
        rfree(plane, length);
}


