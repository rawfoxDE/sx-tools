/************************************************************************/
/*                                                                      */
/*      fichier         : map_divers.c                                  */
/*      projet          : amiglobe                                      */
/*      date création   : juillet 94                                    */
/*      commentaire     : Affichage des éléments remarquables           */
/*      révision        : $VER: map_divers.c 2.002 (10 Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*     $HISTORY:                                                */
/*                     juillet 1994 : 0.01 : version initiale           */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/

#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <utility/hooks.h>
#include "amiglobe_types.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "database_protos.h"
#include "map_divers.h"

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/

extern int num_first_color;
extern PREFERENCE Pref;
extern COUNTRY * tab_country;
extern struct Screen * sC;
extern TRAJECTOIRE ** tab_trajectoire;

/************************************************************************/
/*      defines                                                         */
/************************************************************************/

#define COLOR_SELECT_PAYS   1
#define COLOR_SELECT_SEG    2
#define COLOR_BLEU      0
#define COLOR_COTE      1

/************************************************************************/
/*      prototypes fonctions privées                                    */
/************************************************************************/

void Map_Aff_Capitale (struct RastPort * rpG,ELEM_REM * element);
void Map_Aff_Ville    (struct RastPort * rpG,ELEM_REM * element);
void Map_Aff_Montagne (struct RastPort * rpG,ELEM_REM * element);
void Map_Aff_Monument (struct RastPort * rpG,ELEM_REM * element);


/************************************************************************/
/*      implémentation                                                  */
/************************************************************************/

/* -------------------------- est_dedans(int x,int y) --------------------------

 Commentaire: renvoie TRUE si le point (x,y) est dans le clip_cur

*/

BOOL
est_dedans(int x,int y)
{
    if ((x>Pref.clip_cur.minx) && (x<Pref.clip_cur.maxx) && 
        (y>Pref.clip_cur.miny) && (y<Pref.clip_cur.maxy))
        return TRUE;
    else
        return FALSE;
}


void Map_Aff_Elem_Rem(struct RastPort * rpG,ELEM_REM * element)
{
    CLIP element_clip;      
    SetAPen(rpG,COLOR_SELECT_SEG+num_first_color);
    element_clip.minx=element_clip.maxx=element->Lon;
    element_clip.miny=element_clip.maxy=element->Lat;
    if (tstclip(Pref.clip_cur.minx,Pref.clip_cur.miny,
            Pref.clip_cur.maxx,Pref.clip_cur.maxy,
            &element_clip)==TRUE)
    //if (est_dedans(element->Lon,element->Lat)==TRUE)
    {
        switch (element->Type)
        {
            case CAPITALE:
                Map_Aff_Capitale(rpG,element);
                break;
            case VILLE:
                Map_Aff_Ville(rpG,element); 
                break;
            case MONUMENT:
                Map_Aff_Monument(rpG,element);
                break;
            case MONTAGNE:
                Map_Aff_Montagne(rpG,element);
                break;
            default:
                break;
        }
    }
}

/* -------- Map_Aff_Capitale (struct RastPort  rpG,ELEM_REM  element) --------

 Commentaire: dessine sur la carte une capitale

*/

void
Map_Aff_Capitale (struct RastPort  * rpG,ELEM_REM * element)
{
    int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
    if (element->caracteristique+300000>50*sx)
    {
        int delta=sqrt((double)element->caracteristique)/500+2;
        int minx=element->Lon-delta;
        int maxx=element->Lon+delta;
        int miny=element->Lat-delta*sC->Height*640/sC->Width/512;
        int maxy=element->Lat+delta*sC->Height*640/sC->Width/512;
        conv_xy(&minx,&miny);
        conv_xy(&maxx,&maxy);
        SetAPen(rpG,6+num_first_color);
        Move(rpG,minx,miny);
        Draw(rpG,maxx,miny);
        Draw(rpG,maxx,maxy);
        Draw(rpG,minx,maxy);
        Draw(rpG,minx,miny);
        if (element->caracteristique+300000>175*sx)
            IPrintFond(rpG,maxx,(maxy+miny)/2,element->Nom,
                    -num_first_color);
    }
}

void Map_Aff_Ville (struct RastPort * rpG,ELEM_REM * element)
{
    int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
    if (element->caracteristique+200000>50*sx)
    {
        int delta=sqrt((double)element->caracteristique)/500+2;
        int minx=element->Lon-delta;
        int maxx=element->Lon+delta;
        int y1=element->Lat;
        int y2=element->Lat;
        conv_xy(&minx,&y1);
        conv_xy(&maxx,&y2);
        delta=(maxx-minx)/2;
        SetAPen(rpG,5+num_first_color);
        DrawEllipse(rpG,(minx+maxx)/2,y1,delta,delta*sC->Height*640/sC->Width/512);
        if (element->caracteristique+200000>175*sx)
            IPrintFond(rpG,maxx,y2,element->Nom,-num_first_color);
    }
}

void Map_Aff_Montagne (struct RastPort * rpG,ELEM_REM * element)
{
    int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
    if (element->caracteristique>sx/2)
    {
        float delta=element->caracteristique/300;
        int minx=element->Lon-delta/4;
        int maxx=element->Lon+delta/4;
        int miny=element->Lat-delta/2;
        int maxy=element->Lat+delta/2;
        conv_xy(&minx,&miny);
        conv_xy(&maxx,&maxy);
        SetAPen(rpG,2L);
        Move (rpG,minx,maxy);
        Draw (rpG,maxx,maxy);
        Draw (rpG,(minx+maxx)/2,miny);
        Draw (rpG,minx,maxy);
        if (element->caracteristique>sx)
            IPrintFond (rpG,maxx,maxy,element->Nom,-num_first_color);
    }
}

void Map_Aff_Monument (struct RastPort * rpG,ELEM_REM * element)
{
    int sx=Pref.clip_cur.maxx-Pref.clip_cur.minx;
    if (element->caracteristique>sx/50)
    {
        int delta=element->caracteristique/20;
        int x=element->Lon-delta;
        int y=element->Lat;
        int x1,y1,x2,y2;
        conv_xy(&x,&y);
        SetAPen(rpG,5+num_first_color);
        Move (rpG,x,y);
        x=element->Lon+delta;
        y=element->Lat;
        conv_xy(&x,&y);
        Draw (rpG,x,y);
        x1=element->Lon-delta/2;
        y1=element->Lat-delta*0.83;
        x2=element->Lon+delta/2;
        y2=element->Lat+delta*0.83;
        conv_xy(&x1,&y1);
        conv_xy(&x2,&y2);
        Move (rpG,x1,y1);
        Draw (rpG,x2,y2);
        Move (rpG,x1,y2);
        Draw (rpG,x2,y1);
        if (element->caracteristique>sx/25)
            IPrintFond (rpG,x2,y,element->Nom,-num_first_color);
    }           
}

void map_aff_point(struct RastPort * rpG, POINT * point, int couleur)
{
    if (point->existe==TRUE)
    {
        int x=point->longitude;
        int y=point->latitude;
        if (tstclip(x,y,x,y,&Pref.clip_cur)==TRUE)
        {
            conv_xy(&x,&y);
            SetAPen(rpG,COLOR_SELECT_SEG+num_first_color+couleur);
            Move(rpG,x-12,y);
            Draw(rpG,x-2,y);
            Move(rpG,x+2,y);
            Draw(rpG,x+12,y);
            Move(rpG,x,y-12*sC->Height*640/(sC->Width*512));
            Draw(rpG,x,y-2*sC->Height*640/(sC->Width*512));
            Move(rpG,x,y+12*sC->Height*640/(sC->Width*512));
            Draw(rpG,x,y+2*sC->Height*640/(sC->Width*512));
        }
    }
}
/*
LONG __asm aff_nom_elem_rem (
    register __a2 char **array, 
    register __a1 ELEM_REM * element)
{
    static char buffer[20];
    sprintf(buffer,"%s",element->Name);
    *array=buffer;
    return (0);
}*/

/****************************************************************************/

void
aff_donnees(struct RastPort *rpG,DATA_TRI * donnee,int valeur_max,E_Type_Data type,int nb_plage,int nb_data)
{
    /* on suppose les données triées */
    int i;
    int color=5;
    int nb_pays=0;
    for (i=0;i<nb_data;i++)
    {
        /* methode n°1: nombre égal de pays ayant la même couleur */
        nb_pays++;
        if (nb_pays>nb_data/6)
        {
            nb_pays=0;
            color--;
        }
        /*  methode n°2: distribution égale de la valeur
        switch (type)
        {
          case DATA_FLOAT:
          case DATA_PERCENT:
              color=donnee[i].valeur.data_float*6/valeur_max;
              break;
          case DATA_INTEGER:
          default:
              color=donnee[i].valeur.data_int*6/valeur_max;
              break;
        } */
    /*      switch (type)
            {
            case POPULATION:
                color=log10((double)tab_country[i].population)-4;
                break;
            case SUPERFICIE:
                color=log10((double)tab_country[i].superficie)-3;
                break;
            case DENSITE:
                color=log10((double)tab_country[i].population/tab_country[i].superficie)*6/3;
                break;
            case PNB:
                color=log10((double)tab_country[i].PIB)-2;
                break;
            case PNB_HAB:
                color=(log10((double)tab_country[i].PIB/tab_country[i].population)-2)*6/4;
                break;
            }*/
            if (color<0)
                color=0;
            if (color>5)
                color=5;        
            fill_country(donnee[i].Num_Pays,color+6,rpG);
            /*printf("couleur :%d\n",color);*/
        
    }
}

/* ------------------------------ point_est_dans -------------------------------

 Commentaire:renvoie TRUE si le point de coordonnées (x,y) est dans le
             clip clip.

*/

BOOL
point_est_dans(int x,int y,CLIP * clip)
{
    if (x>=clip->minx && x<=clip->maxx
        && y>=clip->miny && y<=clip->maxy)
        return TRUE;
    else
        return FALSE;
}


