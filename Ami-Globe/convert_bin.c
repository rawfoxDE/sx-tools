/* Convertion d'une serie de pt qui est a la precision maximum a une */
/* serie de points qui ne garde que les point de precision 5            */

#include <exec/types.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define MAX_PT  10000

int tst_end(
        FILE    *input
        )
{
        char c;
        int     code_ret;

        /* lecture du prochain caractere */
        do
    {
                c=getc(input);
                if (c==EOF)
            return(TRUE);
                if (c=='#')
        {
                        do
            {
                                c=getc(input);
                        }
              while((c!=0x0a)&&(c!=EOF));
                        c=getc(input);
                }

        }
      while((!isalnum(c))&&(c!='E'));
        if ((!isalpha(c))||(c!='E'))
    {
                ungetc(c,input);
                code_ret=FALSE;
        }
    else
    {
                code_ret=TRUE;
        }
        return(code_ret);
}

void convert_map(
        char *p_filename,
        char *out_filename
        )
{
        FILE    *input;
        FILE    *output;
        char    c1,c2,c;
        BOOL    flg_end,flg_border,flg_zone;
        int     num_border;
        short   int     num_pt;
        short   int     *tab_temp;
        int     mx,my,MX,MY;
        int     x,y,code;
        short   int     tab_format[4];

        flg_end=FALSE;

        input = fopen(p_filename,"r");
        output = fopen(out_filename,"w");
        if  (input && output )
    {
                tab_temp=malloc(sizeof(short int)*MAX_PT*3);

                /* Premiere chose, la taille max de notre carte...*/

                fscanf( input ,"%d %d %d %d"  ,&mx,&my,&MX,&MY);
                tab_format[0]=mx;
                tab_format[1]=my;
                tab_format[2]=MX;
                tab_format[3]=MY;
                fwrite( tab_format,sizeof(short int),4,output);
                //printf("%ld%ld%ld%ld\n"  ,mx,my,MX,MY);


                //printf("Lecture des borders\n");
                flg_zone=TRUE;
                num_border=0;
        
        /* BIZARRE: la zone de données ne semble commencer qu'au 16 eme
        octet */
                do
        {
                        flg_border=TRUE;
                        num_pt=0;
                        do
            {
                                c1=getc(input);
                                c2=getc(input);
                                if(c2=='#')
                {
                                    do
                    {
                                                c=getc(input);
                                        }
                      while  ((c!=0x0a)&&(c!=EOF));
                                        flg_border=FALSE;

                                }
                else
                {
                                        ungetc(c2,input);
                                        ungetc(c1,input);
                                
                                        fscanf(input,"%d %d %d",&code,&x,&y);
                                        tab_temp[num_pt*3+0]=code;
                                        tab_temp[num_pt*3+1]=x;
                                        tab_temp[num_pt*3+2]=-y;
                                        if (num_pt>MAX_PT)
                    {
                                                /*printf("ERREUR:Depassement du nb max de pt\n");*/
                                        }
                    else
                    {
                                                num_pt++;
                                        }

                                }
                        }
              while(flg_border==TRUE);

                
                        /* On s'assure que le dernier point est bien de niveau 5 */
                        if (tab_temp[(num_pt-1)*3+0]<5)
            {
                                tab_temp[(num_pt-1)*3+0]=5;
                        }
                        /*printf("Border[%d]:%d pt\n",num_border,num_pt);*/
                        fwrite(&num_pt ,sizeof(short int)  ,1     ,output);
                        fwrite(tab_temp,sizeof(short int)*3,num_pt,output);

                        num_border++;

                        if(tst_end(input)==TRUE){
                                /*printf("End of border def\n");*/
                                flg_zone=FALSE;
                        }

                }
          while(flg_zone==TRUE);
                fclose(input);
                fclose(output);
        }
}

main(int        argc,char **argv)
{
        if (argc!=3)
    {
                printf("Conversion fichier .data\nUsage %s <fichier source> <fichier dest>\n",argv[0]);
        }
    else
    {
                convert_map(argv[1],argv[2]);
        }
}
