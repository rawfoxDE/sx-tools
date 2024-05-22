#include <exec/types.h>
#include <math.h>
#include "3d_types.h"
#include "3d_protos.h"

/****************************************/
/* Fonctions de 3d                                  */
/****************************************/

PREFS_3D        Prefs={ 0.0,0.0,0.0,
                                0,  0,  0,
                                320,100,
                                640,200,
                                300
                            };


int     dex=300;        /* distance de l'ecran a l'observateur */
int     dey=300;        /* distance de l'ecran a l'observateur */
int     d=6000; /* distance de l'observateur au pt 0   */

int     valeur_z=-50;

/********************* Rotation dans le plan ************/
void    T3D_2D_Rot(
        float   angle,
        int     *p_x,
        int     *p_y)
{
        int     x,y;
        x= (float)(*p_x) * cos(angle) + (float)(*p_y) * sin(angle);
        y=-(float)(*p_x) * sin(angle) + (float)(*p_y) * cos(angle);

        *p_x=(int)x;
        *p_y=(int)y;
}

/***************** Calcul de la projection 3D->2D ******/

void    calc_coord(
                int     x,
                int     y,
                int     z,
                int     *p_x,
                int     *p_y
                )
{
        if (d+z!=NULL)
    {
                *p_x=Prefs.centre_x+(x*dex)/(d+z);
                *p_y=Prefs.centre_y+(y*dey)/(d+z);
        }
}



/***************** Calcul de la projection inverse 2D->3D ******/

void    calc_coord_inv(
                int     x,
                int     y,
                int     z,
                int     *p_x,
                int     *p_y
                )
{
        *p_x=( (x - Prefs.centre_x)*(d+z))/dex;
        *p_y=( (y - Prefs.centre_y)*(d+z))/dey;
}


/* Converti un point du plan de la carte en sa projection 2d */
/* mais en perspective. Pour avoir la coord z, utiliser      */
/* T3D_ConvertZ                                              */

void T3D_Convert(int x,int y,int *px,int *py)
{
        int     z;

        z=valeur_z*10;

        x=10*(x-Prefs.centre_x);
        y=10*(y-Prefs.centre_y);

        T3D_2D_Rot(Prefs.az,&x,&y);
        T3D_2D_Rot(Prefs.ax,&y,&z);
        T3D_2D_Rot(Prefs.ay,&z,&x);

        calc_coord(x,y,z,px,py);
}

/**********************************************************************/

void T3D_ConvertZ(int x,int y,int z,int *px,int *py)
{

        z=10*(z+valeur_z);
        x=10*(x-Prefs.centre_x);
        y=10*(y-Prefs.centre_y);

        T3D_2D_Rot(Prefs.az,&x,&y);
        T3D_2D_Rot(Prefs.ax,&y,&z);
        T3D_2D_Rot(Prefs.ay,&z,&x);

        calc_coord(x,y,z,px,py);
}



/**********************************************************************/

void T3D_Convert_Inv(int x,int y,int *px,int *py)
{
        int     z;

        z=valeur_z;
        calc_coord_inv(x,y,valeur_z,px,py);
        T3D_2D_Rot((float)(2*PI-Prefs.ay),&z,px);
        T3D_2D_Rot((float)(2*PI-Prefs.ax),py,&z);
        T3D_2D_Rot((float)(2*PI-Prefs.az),px,py);

        *px=*px+Prefs.centre_x;
        *py=*py+Prefs.centre_y;

}

/**********************************************************************/

PREFS_3D        T3D_Set_Prefs(PREFS_3D In_Prefs)
{
        PREFS_3D Prefs_Temp;
        
        Prefs_Temp =    Prefs;
        Prefs      = In_Prefs;
        dex=Prefs.de * Prefs.sizex/320;
        dey=Prefs.de * Prefs.sizey/200;

        return(Prefs_Temp);
}
