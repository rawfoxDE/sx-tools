/************************************************************************/
/*                                                                      */
/*      fichier         : projection.c                                  */
/*      projet          : amiglobe                                      */
/*      date création   : 10/09/94                                      */
/*      commentaire     : fonctions de conversion de                    */
/*                        points écran <-> point de la carte            */
/*      révision        : $VER: projection.c 1.008 (30 Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <exec/types.h>
#include <stdio.h>
#include <clib/macros.h>
#include <libraries/fastsincos.h>
#include "amiglobe_types.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "3D_types.h"
#include "3D_protos.h"
#include "projection.h"

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/
extern PREFERENCE Pref;
extern CLIP clip_proj;
extern CLIP clip_max;
extern struct Library * FastsincosBase;

/************************************************************************/
/*      variables privées                                               */
/************************************************************************/
double cos_b2=0;
double sin_b2=0;
int mx=0;
int my=0;
double win_ratio;


/************************************************************************/
/*      implémentation                                                  */
/************************************************************************/

/*************************************************************************/
void conv_clip(void)
{
    switch(Pref.Type_Proj)
    {
        case PROJ_GLOBE:
            clip_proj=Pref.clip_cur;
            /*{
                int mx=(Pref.clip_cur.minx+Pref.clip_cur.maxx)/2;
                clip_proj=Pref.clip_cur;
                if (mx-clip_proj.minx>9000)
                    clip_proj.minx=mx-9000;
                if (clip_proj.maxx-mx>9000)
                    clip_proj.maxx=mx+9000;
            }*/
            break;
        case PROJ_MERCATOR:
            clip_proj.maxy=
                sin(Map_Convert_Angle(Pref.clip_cur.maxy))*clip_max.maxy;
            clip_proj.miny=
                sin(Map_Convert_Angle(Pref.clip_cur.miny))*clip_max.maxy;
            clip_proj.maxx=Pref.clip_cur.maxx;
            clip_proj.minx=Pref.clip_cur.minx;
            break;
        case PROJ_FLAT:
            clip_proj=Pref.clip_cur;
            break;
        default:
            clip_proj=Pref.clip_cur;
    }
}

/* ------------------------------ Update_New_Clip ------------------------------

 Commentaire:cette fonction recalcule les constantes servant à conv_xy
             et conv_inv_xy à chaque changement du clip_cur
             CETTE FONCTION DOIT ETRE APPELEE A CHAQUE NOUVEAU CLIP_CUR

*/

void
Update_New_Clip(void)
{
    static double b2;
    mx=(Pref.clip_cur.maxx+Pref.clip_cur.minx)/2;
    my=(Pref.clip_cur.maxy+Pref.clip_cur.miny)/2;
    b2=my*PI/18000;
    cos_b2=cos(b2);
    sin_b2=sin(b2);
    win_ratio=Pref.win_width/Pref.win_height;

}

/****************************************************************************/
/* converti un point écran en un point de la carte*/
void conv_inv_xy(
        int     *px,int *py
        )
{
    double  f_angle;//,f_lat,f_lon;
    int     x,y;
    x=*px;
    y=*py;

    if(Pref.Flg_Proj_3D)
    {
        T3D_Convert_Inv(x,y,px,py);
    }
    switch(Pref.Type_Proj)
    {
    case PROJ_GLOBE:
    {
            static float alpha; /* peut être <0 */

            static float gamma; /* peut être <0 */
            static float c;     /* [0,PI]      */
            static float a;     /* [0,PI]      */
    x=(x*(Pref.clip_cur.maxx-Pref.clip_cur.minx))/(Pref.win_height*win_ratio)+
            Pref.clip_cur.minx-mx;
    y=(y*(Pref.clip_cur.maxy-Pref.clip_cur.miny))/(Pref.win_height)+
            Pref.clip_cur.miny-my;
            a=acos(-(double)y/(double)clip_max.maxy);
            if (FastsincosBase!=NULL)
            {
                gamma=asin((double)x/((double)clip_max.maxx*fastsin(a)));
                if (fabs ((float)y/(float)clip_max.maxy) <=1
                    && fabs( (float)x / ((float)clip_max.maxx*fastcos(a-PI/2)) ) <=1 )
                {
    /*          if (f_lat>PI/2)
                {
                    f_lat=PI-f_lat;
                    if (f_lon>0)
                        f_lon-=PI;
                    else
                        f_lon+=PI;
                }
                else
                if (f_lat<PI/2)
                {
                        f_lat=-PI+f_lat;
                        if (f_lon>0) 
                                f_lon-=PI;
                        else
                                f_lon+=PI;
                }*/     
                    c=acos(fastcos(a)*cos_b2-fastsin(a)*sin_b2*fastcos(gamma));
                    alpha=asin(fastsin(a)*fastsin(gamma)/fastsin(c));
                    *px=alpha*18000/PI+mx;
                    if (*px>18000)
                        *px-=18000;
                    if (*px<-18000)
                        *px+=18000;
                    *py=-9000+c*clip_max.maxy*2/PI; 
                }
                else
                {
                    if (x<0) 
                        *px=-clip_max.maxx;
                    else
                        *px=clip_max.maxx;
                    if (y<0)
                        *py=-clip_max.maxy;
                    else
                        *py=clip_max.maxy;
                }
            }
            else
            {
                gamma=asin((double)x/((double)clip_max.maxx*sin(a)));
                if (fabs ((float)y/(float)clip_max.maxy) <=1
                    && fabs( (float)x / ((float)clip_max.maxx*cos(a-PI/2)) ) <=1 )
                {
    /*          if (f_lat>PI/2)
                {
                    f_lat=PI-f_lat;
                    if (f_lon>0)
                        f_lon-=PI;
                    else
                        f_lon+=PI;
                }
                else
                if (f_lat<PI/2)
                {
                        f_lat=-PI+f_lat;
                        if (f_lon>0) 
                                f_lon-=PI;
                        else
                                f_lon+=PI;
                }*/     
                    c=acos(cos(a)*cos_b2-sin(a)*sin_b2*cos(gamma));
                    alpha=asin(sin(a)*sin(gamma)/sin(c));
                    *px=alpha*18000/PI+mx;
                    *py=-9000+c*clip_max.maxy*2/PI;
                }
                else
                {
                    if (x<0) 
                        *px=-clip_max.maxx;
                    else
                        *px=clip_max.maxx;
                    if (y<0)
                        *py=-clip_max.maxy;
                    else
                        *py=clip_max.maxy;
                }
            }
    }
    break;
    case PROJ_MERCATOR:
        x=(x*(Pref.clip_cur.maxx-Pref.clip_cur.minx))/(Pref.win_width)+Pref.clip_cur.minx;
        y=(y*(clip_proj.maxy-clip_proj.miny))/(Pref.win_height)+clip_proj.miny;
        if (y>clip_max.maxy)
            y=clip_max.maxy;
        if (y<clip_max.miny)
            y=clip_max.miny;
        f_angle=(double)asin((float)((float)y/(float)clip_max.maxy));
        y=f_angle*(clip_max.maxx-clip_max.minx)/(2*PI);
        *px=x;
        *py=y;          
        break;

    case PROJ_FLAT:
    default:
        x=(x*(Pref.clip_cur.maxx-Pref.clip_cur.minx))/(Pref.win_width)+Pref.clip_cur.minx;
        y=(y*(Pref.clip_cur.maxy-Pref.clip_cur.miny))/(Pref.win_height)+Pref.clip_cur.miny;
        *px=x;
        *py=y;
        break;
    }
}

/****************************************************************************/
/* converti un point de la carte en point écran*/
void conv_xy(int *px,int *py)
{
    float   f_angle;//,f_lat,f_lon;
    int     x,y;
/*      int clipmaxy,clipminy;*/

    x=*px;
    y=*py;

    switch (Pref.Type_Proj)
    {

    case PROJ_MERCATOR:
            f_angle=Map_Convert_Angle(y);
            y=sin(f_angle)*(clip_max.maxy);
*px=(((x-Pref.clip_cur.minx)*(Pref.win_width))/(Pref.clip_cur.maxx - Pref.clip_cur.minx));
*py=(((y-clip_proj.miny)*(Pref.win_height))/(clip_proj.maxy - clip_proj.miny));
            
/*                      *py=y;
            *px=x;      */
            break;
    case PROJ_GLOBE:
        {
            static float alpha; /* peut être <0 */

            static float gamma; /* peut être <0 */
            static float c;     /* [0,PI]      */
            static float a;     /* [0,PI]      */
            static float aux1;
            static float aux2;
            /**********************************************************/
            /* changement de repère en coordonnées sphériques         */
            /* principe du triangle sphérique ABC: A est le pôle nord */
            /* B parcours la carte, C est le nouveau pôle nord        */
            /* on connait le coté b (soit [A,C]); il ne bouge pas, et */
            /* calculé dans Update_New_Clip                           */
            /* c dépend de la latitude du point B considéré           */
            /* l'angle alpha (angle au sommet A) dépend de la longitu-*/
            /* de du point B considéré                                */
            /* Les formules de résolution du triangle sphérique nous  */
            /* donnent le côté a et l'angle gamma, donnant respective-*/
            /* ment les nouvelles latitude et longitude du point B    */
            /**********************************************************/
            alpha=(x-mx)*PI/18000;
            c=(y+9000)*PI/18000;
            if (Pref.prof==5 && FastsincosBase!=NULL)
            {
                /* calcul de a et gamma                                   */
                aux1=cos_b2*fastcos(c)+sin_b2*fastsin(c)*fastcos(alpha);
                a=acos(aux1);
                aux2=fastsin(c)*fastsin(alpha)/fastsin(a);
                if (a!=0)
                    /* asin nous donne la solution entre -PI/2 et PI/2 */
                    gamma=asin(aux2);
                else
                    gamma=0;

                /* détermination des x et y*/
                y=-aux1*clip_max.maxy;
/*                if (c>a && gamma<alpha)
                    x=fastsin(a)*clip_max.maxx;
                else
                if (c<a && gamma>alpha)
                    x=-sin(a)*clip_max.maxx;
                else*/
                    /* la solution donnée par asin était bonne    */
//                    if (gamma!=0 || x==mx || abs(x-mx)==18000)
                        x=fastsin(a)*aux2*clip_max.maxx;
/*                    else
                        if ((x-mx+36000)%36000<18000)
                            x=fastsin(a)*clip_max.maxx;
                        else
                            x=-fastsin(a)*clip_max.maxx;
*/            }
            else
            {
                /* calcul de a et gamma                                   */
                aux1=cos_b2*cos(c)+sin_b2*sin(c)*cos(alpha);
                a=acos(aux1);
                aux2=sin(c)*sin(alpha)/sin(a);
                if (a!=0)
                    /* asin nous donne la solution entre -PI/2 et PI/2 */
                    gamma=asin(aux2);
                else
                    gamma=0;

                /* détermination des x et y*/
                y=-aux1*clip_max.maxy;
    /*          if (ABS(c)>ABS(a) && ABS(gamma)<ABS(alpha))
                    x=sin(a)*clip_max.maxx;
                else
                if (ABS(c)<ABS(a) && ABS(gamma)>ABS(alpha))
                    x=-sin(a)*clip_max.maxx;
                else */
                    /* la solution donnée par asin était bonne    */
                    if (gamma!=0 || x==mx || abs(x-mx)==18000)
                        x=sin(a)*aux2*clip_max.maxx;
                    else
                        if ((x-mx+36000)%36000<18000)
                            x=sin(a)*clip_max.maxx;
                        else
                            x=-sin(a)*clip_max.maxx;
            }
            /* transformation finale en coordonnées écran              */
*px=(((x-Pref.clip_cur.minx+mx)*(Pref.win_height*win_ratio))/(Pref.clip_cur.maxx - Pref.clip_cur.minx));
*py=(((y-Pref.clip_cur.miny+my)*(Pref.win_height))/(Pref.clip_cur.maxy - Pref.clip_cur.miny));
        }
            break;
    case PROJ_FLAT:
    default:
        x=(((x-Pref.clip_cur.minx)*(Pref.win_width))/(Pref.clip_cur.maxx - Pref.clip_cur.minx));        
        y=(((y-Pref.clip_cur.miny)*(Pref.win_height))/(Pref.clip_cur.maxy - Pref.clip_cur.miny));
        *py=y;
        *px=x;
        break;
    }
    if (Pref.Flg_Proj_3D)
    {
            T3D_Convert(x,y,px,py);
    }
}

void 
correct_clip(CLIP * clip)
{
        /*rend le clip rectangle*/
        int sx=clip->maxx-clip->minx;
        static int SX=0;
        static int SY=0;
        if (SX==0)
        {
                SX=clip_max.maxx-clip_max.minx;
                SY=clip_max.maxy-clip_max.miny;
        }
        switch (Pref.Type_Proj)
        {
        case PROJ_GLOBE:
                clip->maxy=sx*SY/SX+clip->miny;
                break;  
                
        case PROJ_MERCATOR:
                {
/*              float coef=Map_Convert_Angle(SY*sx/SX)
                                        +asin((double)(clip->miny/clip_max.maxy));*/
                double coef2=((double)(SY*sx))/(double)SX;
                double coef3=(double)coef2/(double)clip_max.maxy;
                double coef=coef3+sin(clip->miny*PI/18000);
                if (coef<=1)
                        clip->maxy=asin(coef)*clip_max.maxy*2/PI;
                else
                {
                        /*deuxieme cas*/
/*
                        clip->miny=sin(PI/2-Map_Convert_Angle(SY*sx/SX))
                                                *clip_max.maxy;
                        clip->maxy=clip_max.maxy;*/
                }
                }
                break;
        case PROJ_FLAT:
                clip->maxy=sx*SY/SX+clip->miny;
                break;
        default:
                clip->maxy=sx*SY/SX+clip->miny;
                break;
        }
}                       
        

