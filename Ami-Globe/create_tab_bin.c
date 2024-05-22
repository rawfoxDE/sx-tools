        
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>

/* Prototypes */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/macros.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stat.h>
#include <math.h>

#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds

#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>

#include "amiglobe_types.h"


#define MIN_RESO        3
CLIP    clip_cur;
CLIP    clip_max;
int     max_border=0;

/* Tableau de pointeurs sur les vecteurs */
        
BORDER  *tab_vect;

void tstpt(
        int     x,
        int     y,
        int     *pmx,
        int     *pmy,
        int     *pMX,
        int     *pMY
        )
{
        if (x<*pmx)
    {
                *pmx=x;
        }
        if (x>*pMX)
    {
                *pMX=x;
        }
        if (y<*pmy)
    {
                *pmy=y;
        }
        if (y>*pMY)
    {
                *pMY=y;
        }
}

/*********************************************************** read_map() ***/

BOOL read_map_bin(
        char *p_filename,
        char *p_out
        )
{
        char    Buffer_Name[100];
        FILE    *input,*fp_out;
        int     mx,my,MX,MY;
        int     i;

        int     flg_zone;
        int     flg_end;
        int     num_bord;
        int     Last_Code=0;
        /* Vecteur courant */
        short   int     *tab_temp,*tab_deb;
        char    *p_deb;
        struct  vect *v;
        short   int     nb_vect,nb_vect_small;
        int     nb_read,taille;
        struct  stat    my_stat;
        short   int     minx_real,miny_real,maxx_real,maxy_real;
        BOOL    Flg_Err;
        LONG    pos;

        /*printf("Lecture de %s\n",p_filename);*/
        flg_end=FALSE;
        Flg_Err=TRUE;

        sprintf(Buffer_Name,"%s.pt_bin",p_filename);

        if( (input = fopen(p_filename,"r"))&&(fp_out = fopen(p_out,"w")))
        {
                /* Premiere chose, la taille max de notre carte...*/

                fread( &minx_real,sizeof(short int),1,input);
                fwrite(&minx_real,sizeof(short int),1,fp_out);
                fread( &miny_real,sizeof(short int),1,input);
                fwrite(&miny_real,sizeof(short int),1,fp_out);
                fread( &maxx_real,sizeof(short int),1,input);
                fwrite(&maxx_real,sizeof(short int),1,fp_out);
                fread( &maxy_real,sizeof(short int),1,input);
                fwrite(&maxy_real,sizeof(short int),1,fp_out);
                clip_cur.minx=minx_real;
                clip_cur.miny=miny_real;
                clip_cur.maxx=maxx_real;
                clip_cur.maxy=maxy_real;
                clip_max=clip_cur;
                /*printf("minx:%d miny:%d maxx:%d maxy:%d\n",
                        minx_real,miny_real,maxx_real,maxy_real);*/

                stat(p_filename,&my_stat);
                
                taille=my_stat.st_size;
                taille=taille-4*sizeof(short int);

                printf("Taille:%d\n",taille);

                tab_temp=(short int *)malloc(taille);

                tab_deb=tab_temp;

                if (tab_temp==NULL)
                {
                        printf("ERREUR avec allocation memoire pour le fichier\n");
                }else{
                   nb_read=fread(tab_temp,1,taille,input);
                   
                   /* lecture du reste du fichier */
                   if (nb_read!=taille)
                   {
                        printf("ERROR: File truncated..\n");
                   }else{

                        p_deb=(char *)tab_temp;

                        /*printf("Lecture des borders\n");*/
                        num_bord=0;
                        flg_zone=TRUE;

                        do
                        {
#ifdef  TOMDEBUG
                                printf("Vecteur numero:%d\n",num_bord);
#endif
                                /* Lecture de composantes d'un bord */
                                mx=100000;
                                my=100000;
                                MX=-100000;
                                MY=-100000;

                                pos=sizeof(short int)*((tab_temp-tab_deb)+4);
                                nb_vect=*tab_temp++;/* 5eme mot = nb de vecteurs */
                                tab_vect[num_bord].p_vect=(struct vect *)tab_temp;
                                tab_temp+=nb_vect*3;
                                v=tab_vect[num_bord].p_vect;

                                /* On cree la zone de clipping, mais en */
                                /* excluant les lacs et rivieres        */
                                for (i=0;i<nb_vect;i++)
                                {                                       
                                        tstpt(v->x,v->y,&mx,&my,&MX,&MY);
                                        v++;
                                }

                                v=tab_vect[num_bord].p_vect;                
                                /* On s'assure que le dernier point est bien de niveau 5 */
                                if (nb_vect>1)v[nb_vect-1].Code=5;

                                /* On s'assure que le code du premier pt est bien */
                                /* un indicateur de nouveau segment                       */

                                if (tab_vect[num_bord].p_vect[0].Code<1000)
                                {
                                        tab_vect[num_bord].p_vect[0].Code=Last_Code;
                                }
                                Last_Code=tab_vect[num_bord].p_vect[0].Code;
        
                                /* Creation du nouveau tableau */

                                nb_vect_small=0;
                                for(i=0;i<nb_vect;i++){
                                        if(v[i].Code>=MIN_RESO){
                                                nb_vect_small++;
                                        }
                                }

                                /* Ecriture de la position ds le fichier origine */
                                fwrite(&pos,sizeof(LONG),1,fp_out);
                        
                                /* Ecriture du nveau nb de pt */
                                fwrite(&nb_vect_small,sizeof(short int),1,fp_out);

                                for(i=0;i<nb_vect;i++){
                                        if(v[i].Code>=MIN_RESO){
        
                                                fwrite( &v[i],
                                                        sizeof(struct vect),
                                                        1,fp_out);

                                        }
                                }


                                if (num_bord>MAX_BORDER)
                                {
                                        /*printf("ERREUR, depassement nombre de border!\n");*/
                                }
                                else
                                {
                                        num_bord++;
                                }
                                if ((char *)tab_temp>=taille+p_deb)
                                {
                                        flg_zone=FALSE;
                                }
                        }
                        while(flg_zone==TRUE);
                        max_border=num_bord;
                        Flg_Err=FALSE;
                    }
                }
                fclose(fp_out);
        }
        return(Flg_Err);
}

main(int argc,char **argv)
{
        tab_vect=(BORDER *)malloc(sizeof(BORDER)*MAX_BORDER);

        if(argc>2){
                read_map_bin(argv[1],argv[2]);
        }
}

