/************************************************************************/
/*                                                                      */
/*      fichier         : iff_fct.c                                     */
/*      projet          : amiglobe                                      */
/*      date création   : juin 1994                                     */
/*      commentaire     : fonctions relatives a l'affichage des images  */
/*                        (fond, donnees externes, etc...)
/*      révision        : $VER: iff_fct.c 1.003 (29 Sep 1994)
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*     $HISTORY:                                                */
/*             juin        1994 : 0.01 : version initiale           */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <graphics/displayinfo.h>
#include <graphics/display.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include "iff/jiff.h"

/* Prototypes */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/macros.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stat.h>
#include <math.h>

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/

//extern  struct Library *IntuitionBase;
extern  struct GfxBase  *GfxBase;
extern  struct Library *DataTypeBase;
extern  struct Screen * sC;

/************************************************************************/
/*      variables privées                                               */
/************************************************************************/

USHORT  palette_glob[32];

/************************************************************/
/* Procedure to put the colors of the picture on the screen */
/************************************************************/

void put_ea_cmap(unsigned   char    *cmap,int colors)
{
    int i;

    if (colors > MAXCOL)    /*color clipping*/
        colors = MAXCOL;
    for (i=0; i<colors; i++)
    {
        palette_glob[i] = 
            ((cmap[0] & 0xf0) << 4) + (cmap[1] & 0xf0) + ((cmap[2] & 0xf0) >> 4);
        cmap += 3;
    }
}

/***********************/
/* Call the IFF loader */
/***********************/

struct  Screen  *Iff_Charge_Iff(    char    *name)
{
    struct  ILBM_info   *info;
    struct  RastPort    EcranRastPort;
    struct  Screen  *s;
    struct  Window  *win;
    struct  RastPort    *rp;
    static  struct TagItem    ScreenTags[8] = {
    SA_Left,          0,
    SA_Top,           0,
    SA_Width,         320,
    SA_Height,        200,
    SA_Depth,         5,
    SA_Type,          CUSTOMSCREEN,
    SA_DisplayID,     PAL_MONITOR_ID,
    TAG_DONE };

    InitRastPort(&EcranRastPort);
    if ( (info = read_iff(name,0) ) != NULL){
        ScreenTags[2].ti_Data=(ULONG)info->header.w;
        ScreenTags[3].ti_Data=(ULONG)info->header.h;
        ScreenTags[4].ti_Data=(ULONG)info->header.nPlanes;
        ScreenTags[6].ti_Data=PAL_MONITOR_ID;
        if(info->header.w>320){
        ScreenTags[6].ti_Data|=MODE_640;
        }
        if(info->header.h>256){
        ScreenTags[6].ti_Data|=INTERLACE;
        }
        if(info->header.nPlanes==6){
        ScreenTags[6].ti_Data|=HOLDNMODIFY;
        }
/*
        ScreenTags[6].ti_Data|=info->camg;
*/
            if ( ( s = OpenScreenTagList( 0l, ScreenTags ))!=0){

            if(win=OpenWindowTags(
                NULL,
                WA_Left,    0,
                WA_Top,     0,
                WA_Width,   s->Width,
                WA_Height,  s->Height,
                WA_CustomScreen,s,
                WA_Flags,   WFLG_SMART_REFRESH|WFLG_ACTIVATE|WFLG_CLOSEGADGET,
                WA_IDCMP,   IDCMP_CLOSEWINDOW,
                TAG_DONE))
        {
            rp = win->RPort;
            put_ea_cmap(&info->cmap,32 );
            EcranRastPort.BitMap = &info->bitmap;
            ClipBlit( &EcranRastPort, 0L, 0L,
                rp, 0L, 0L,
                (rp->BitMap->BytesPerRow)*8,
                rp->BitMap->Rows, (long)COPY_MINTERM);
        }else{
            CloseScreen(s);
            s=NULL;
        }
        }
        free_planes(&info->bitmap);
    }else{
        Aff_Error("ERROR loading IFF file\n","OK");
    }
    return(s);
}

/*********************************************************/
Object *Iff_New_Charge_Iff(char *pc,struct Screen *In_Scr)
{
    ULONG *cregs = NULL, numcolors, i, r, g, b;
    struct dtFrameBox dtf = {NULL};
    struct FrameInfo fri = {NULL};
    struct BitMapHeader *bmhd;
    ULONG modeid = INVALID_ID;
    struct gpLayout gpl;
    struct BitMap *bm;
    struct RastPort rp;
    Object *o;


    o=0;
    if (DataTypeBase){

        /* Obtain a pointer to the picture object */
        if (o = NewDTObject ((APTR) pc,
                     DTA_SourceType, DTST_FILE,
                     DTA_GroupID, GID_PICTURE,
                     PDTA_Remap, FALSE,
                     TAG_DONE))
        {
            /* Ask the object what kind of environment it needs */
            dtf.MethodID = DTM_FRAMEBOX;
            dtf.dtf_FrameInfo = &fri;
            dtf.dtf_ContentsInfo = &fri;
            dtf.dtf_SizeFrameInfo = sizeof (struct FrameInfo);
            if (DoMethodA (o, &dtf) && fri.fri_Dimensions.Depth)
            {

            /* Layout the object on our process */
            gpl.MethodID = DTM_PROCLAYOUT;
            gpl.gpl_GInfo = NULL;
            gpl.gpl_Initial = 1;
            if (DoMethodA (o, &gpl))
            {
                /* Get the object information */
                GetDTAttrs (o,
                    PDTA_ModeID, &modeid,
                    PDTA_CRegs, &cregs,
                    PDTA_NumColors, &numcolors,
                    PDTA_BitMapHeader, &bmhd,
                    PDTA_BitMap, &bm,
                    TAG_DONE);

                /* Make sure we have a bitmap to attach to the screen */
                if (bm)
                {
                    InitRastPort(&rp);
                        rp.BitMap=bm;
                    /* Load the color map */
                    if (cregs)
                    {
                      if(In_Scr!=NULL){
                    numcolors = 2 << (In_Scr->RastPort.BitMap->Depth - 1);
                    for (i = 0; i < numcolors; i++)
                    {
                        r = cregs[i * 3 + 0];
                        g = cregs[i * 3 + 1];
                        b = cregs[i * 3 + 2];
                        SetRGB32 (&In_Scr->ViewPort, i, r, g, b);
                    }
                      }
                    }

                }else{
                    Aff_Error("ERROR opening screen\n(not enough chip memory)\n","OK");
                }
            }
            }else{
            Aff_Error("This is not a displayable picture\n","Too Bad");
            }
        }
    }else{
        Aff_Error("This function requires WB3.0 (Datatypes) to work!","Shit");
/*
        scr=Charge_Iff(pc);
*/
    }
    return(o);
}

void    Iff_Free_Obj(Object *o)
{
    if(o!=NULL){
        if (DataTypeBase){
    
            /* Get rid of the object */
                DisposeDTObject (o);

        }else{
        }
    }
}


void    Iff_Copy_Fond(Object *o,struct RastPort *rpG)
{
    struct BitMapHeader *bmhd;
    ULONG modeid = INVALID_ID;
    struct BitMap *bm;
    struct RastPort rp;


    if ((DataTypeBase)&&(o!=NULL)){

        /* Get the object information */
        GetDTAttrs (o,
            PDTA_ModeID, &modeid,
            PDTA_BitMapHeader, &bmhd,
            PDTA_BitMap, &bm,
            TAG_DONE);

        /* Make sure we have a bitmap to attach to the screen */
        if (bm)
        {
            long dest_x=0;
            long dest_y=0;
            InitRastPort(&rp);
                rp.BitMap=bm;
            while (dest_x<sC->Width)
            {
                /*printf("dest_x=%d\n",dest_x);*/
                while (dest_y<sC->Height)
                {
                    ClipBlit( &rp, 0L, 0L,
                            rpG, dest_x, dest_y,
                            bmhd->bmh_Width, /*bm->BytesPerRow*8/bm->Depth,*/
                            bmhd->bmh_Height, /*bm->Rows*/
                            (long)COPY_MINTERM);
                    dest_y+=bmhd->bmh_Height;/*bm->Rows;*/
                }
                dest_y=0;
                dest_x+=bmhd->bmh_Width;/*bm->BytesPerRow*8/bm->Depth;*/
            }
         }
      
      }
}



/*********************************************************/
struct  Screen *Iff_Load_And_Open(char *pc)
{
    struct dtFrameBox dtf = {NULL};
    struct FrameInfo fri = {NULL};
    struct BitMapHeader *bmhd;
    ULONG modeid = INVALID_ID;
    struct Screen *scr;
    struct Window *win;
    struct BitMap *bm;
    struct RastPort rp;
    ULONG *cregs = NULL, numcolors, i, r, g, b;
    Object *o;

    scr=NULL;
    o=Iff_New_Charge_Iff(pc,NULL);
    if(o){  
        /* Ask the object what kind of environment it needs */
        dtf.MethodID = DTM_FRAMEBOX;
        dtf.dtf_FrameInfo = &fri;
        dtf.dtf_ContentsInfo = &fri;
        dtf.dtf_SizeFrameInfo = sizeof (struct FrameInfo);
        if (DoMethodA (o, &dtf) && fri.fri_Dimensions.Depth)
        {
            /* Get the object information */
            GetDTAttrs (o,
                PDTA_ModeID, &modeid,
                PDTA_CRegs, &cregs,
                PDTA_NumColors, &numcolors,
                PDTA_BitMapHeader, &bmhd,
                PDTA_BitMap, &bm,
                TAG_DONE);

            /* Make sure we have a bitmap to attach to the screen */
            if (bm)
            {
            /* Open the screen */
            if (scr = OpenScreenTags (NULL,
                  SA_Width, (ULONG) fri.fri_Dimensions.Width,
                  SA_Height, (ULONG) fri.fri_Dimensions.Height,
                  SA_Depth, (ULONG) fri.fri_Dimensions.Depth,
                  SA_DisplayID, modeid,
                  SA_ShowTitle, FALSE,
                  SA_Quiet, TRUE,
                  SA_Behind, TRUE,
                  TAG_DONE))
            {
                InitRastPort(&rp);
                    rp.BitMap=bm;

                if(win=OpenWindowTags(
                NULL,
                WA_Left,    0,
                WA_Top,     0,
                WA_Width,   scr->Width,
                WA_Height,  scr->Height,
                WA_CustomScreen,scr,
                WA_Flags,   WFLG_SMART_REFRESH|WFLG_ACTIVATE|WFLG_CLOSEGADGET,
                WA_IDCMP,   IDCMP_CLOSEWINDOW,
                TAG_DONE))
                {
                        ClipBlit( &rp, 0L, 0L,
                          win->RPort, 0L, 0L,
                          fri.fri_Dimensions.Width,
                          fri.fri_Dimensions.Height,
                          (long)COPY_MINTERM);
                /* Load the color map */
                if (cregs)
                {
                numcolors = 2 << (scr->RastPort.BitMap->Depth - 1);
                for (i = 0; i < numcolors; i++)
                {
                    r = cregs[i * 3 + 0];
                    g = cregs[i * 3 + 1];
                    b = cregs[i * 3 + 2];
                    SetRGB32 (&scr->ViewPort, i, r, g, b);
                }
                }
                }
                /* Bring the screen to the front now */
                ScreenToFront (scr);
            }
            }
        }
        /* Get rid of the object */
            DisposeDTObject (o);
    }
    return(scr);
}
