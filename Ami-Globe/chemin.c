/************************************************************************/
/*                                                                      */
/*      fichier         : chemin.c                                      */
/*      projet          : amiglobe                                      */
/*      date création   : 29/08/94                                      */
/*      commentaire     : calcul et dessin du plus court chemin entre   */
/*                        le point origine et le point destination      */
/*      révision        : $VER: chemin.c 1.011 (11 Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <graphics/rastport.h>
#include <math.h>
#include <stdlib.h>
#include <clib/graphics_protos.h>
#include <stdio.h>

#include "amiglobe_types.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "chemin.h"

/************************************************************************/
/*      defines                                                         */
/************************************************************************/
/* pas de parcours de l'angle alpha */
#define PAS_ANGLE 100

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/
extern PREFERENCE Pref;

/************************************************************************/
/*      variables privées                                               */
/************************************************************************/
enum pole { POLE_NORD, POLE_SUD}  Pole;
enum sens { GAUCHE,DROITE } Sens;
double gamma=0,alpha=0;
double beta;
double a,b,c;
double tan_b_sur_2,tan_c_sur_2;
double x,y;
#define COLOR_CHEMIN 6

/************************************************************************/
/*      implémentation                                                  */
/************************************************************************/

/* ------------------------------- Update_Points -------------------------------

 Commentaire:met à jour les variables gamma,a,b et c en raison d'une
             nouvelle position des points orig et dest
             DOIT ETRE APPELE A CHAQUE MODIFICATION DES POINTS ORIG ET DEST
             cf cerveau d'Olivier Collard pour comprendre

*/

void
Update_Points(void)
{
    /*  a,b et c sont les côtés d'un triangle sphérique. Le  point  (b,c) */
    /*  est  soit  le  pôle  nord,  soit  le pôle sud (cf variable Pole). */
    /*  point_orig est sur le point (a,c) et point_dest sur  (a,b)  alpha */
    /*  est  l'angle au sommet (b,c) (pôle nord ou pôle sud) gamma, quant */
    /*  à lui, est l'angle au sommet (a,b) (soit  point_dest)  a  est  le */
    /*  plus  court  chemin entre point_orig et point_dest Le parcours de */
    /*  l'angle alpha peut se faire soit vers la  gauche,  soit  vers  la */
    /*  droite  (cf  variable Sens). Le parcours commence toujours depuis */
    /*  le point destination.                                             */
    b= 9000+Pref.point_dest.latitude;
    b= b*PI/18000;
    c= 9000+Pref.point_orig.latitude;
    c *=PI/18000;
    alpha=Pref.point_dest.longitude-Pref.point_orig.longitude;
    if (alpha<0)
    {
        Sens=DROITE;/* point origine à droite du point destination */
        alpha=-alpha;
    }
    else
        Sens=GAUCHE;/*point origine à gauche du point destination */
    if (alpha>18000)
    {
        if (Sens==GAUCHE)
            Sens=DROITE;
        else
            Sens=GAUCHE;
        alpha=36000-alpha;
    }
    alpha *=PI/18000;
    /* calcul de a*/
    a=acos(cos(b)*cos(c)+sin(b)*sin(c)*cos(alpha));
    /* calcul de gamma */
    gamma=acos((cos(c)-cos(a)*cos(b))/(sin(a)*sin(b)));
    /* calcul de beta */
    beta=acos(-cos(gamma)*cos(alpha)+sin(gamma)*sin(alpha)*cos(b));
    /* détermination du pôle de référence Pole */
    if (gamma+beta > PI)
    {
        Pole=POLE_SUD;
        b=PI-b;
        c=PI-c;
        gamma=acos((cos(c)-cos(a)*cos(b))/(sin(a)*sin(b)));
        beta=acos(-cos(gamma)*cos(alpha)+sin(gamma)*sin(alpha)*cos(b));
    }
    else
        Pole=POLE_NORD;
    /* pré-calcul des sin et des cos */
    tan_c_sur_2=tan(c/2);
    tan_b_sur_2=tan(b/2);
}


/* -------------------- Draw_Chemin(struct RastPort  rpG) ---------------------

 Commentaire:dessine le chemin le plus court entre Pref.point_orig et
             Pref.point_dest

*/

void
Draw_Chemin(struct RastPort * rpG)
{
    if ( (Pref.point_orig.existe==TRUE) &&
         (Pref.point_dest.existe==TRUE))
/*  && (Pref.Flg_Aff_Chemin==TRUE)    */
    {
        /* on assume que Update_Points a été fait*/
        double c1,a1;
        /*,b1,a1; */
        double angle=0,coupe=100;
        int retard=0;
        int x,y;
        SetAPen(rpG,COLOR_CHEMIN);
        MyMove(rpG,Pref.point_dest.longitude,Pref.point_dest.latitude);
        if (Sens==GAUCHE)
            if (Pref.point_dest.longitude-alpha*18000/PI < -18000)
                coupe=(Pref.point_dest.longitude+18000)*PI/18000;
            else
                coupe=-100;
        if (Sens==DROITE
            && Pref.point_dest.longitude+alpha*18000/PI > 18000)
            coupe=(18000-Pref.point_dest.longitude)*PI/18000;
        while (abs(angle)<abs(alpha))
        {
            a1=asin(sin(angle)*sin(b)*sin(beta));
            if (gamma+angle>PI)
            {
                c1= atan(tan_c_sur_2*cos((abs(alpha-angle)-beta)/2)
                                 /cos((abs(alpha-angle)+beta)/2))
                  - atan(tan_c_sur_2*sin((abs(alpha-angle)-beta)/2)
                                 /sin((abs(alpha-angle)+beta)/2));
            }
            else
            {
                c1= atan(tan_b_sur_2*cos((gamma-angle)/2)/cos((gamma+angle)/2))
                  + atan(tan_b_sur_2*sin((gamma-angle)/2)/sin((gamma+angle)/2));
            }
            if (Sens==GAUCHE)
                x=Pref.point_dest.longitude-angle*18000/PI-retard;
            else
                x=Pref.point_dest.longitude+angle*18000/PI-retard;
            switch (Pole)
            {
            case POLE_NORD:
                y=-9000+c1*18000/PI;
                break;
            case POLE_SUD:
                y=9000-c1*18000/PI;
                break;
            }
            MyDraw(rpG,x,y,5);
            if (angle==coupe)
            {
                if (Sens == GAUCHE )
                {
                    coupe=-100;
                    retard=-36000;
                    MyMove(rpG,18000,y);
                }
                else
                {
                    coupe =100;
                    retard=36000;
                    MyMove(rpG,-18000,y);
                }
            }
            angle+=PI*Pref.prof/PAS_ANGLE;
            if (    (angle>coupe && Sens==DROITE)
                 || (angle<coupe && Sens==GAUCHE)     )
                angle=coupe;
        }
        MyDraw(rpG,Pref.point_orig.longitude,Pref.point_orig.latitude,5);
    }
}
