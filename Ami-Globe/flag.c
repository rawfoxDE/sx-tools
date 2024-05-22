/************************************************************************/
/*                                                                      */
/*      fichier         : flag.c                                        */
/*      projet          : amiglobe                                      */
/*      date création   : mai 1994                                      */
/*      commentaire     : dessin des drapeaux                           */
/*      révision        : $VER: flag.c 1.001 (28 Aug 1994)              */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*      $HISTORY:                                                */
/*                                                              */
/*                     mai 1994 : 0.01 : version initiale
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <math.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include "amiglobe_types.h"
#include "3d_types.h"
#include "3d_protos.h"

#define BUFFER_SIZE 300

typedef struct Def_Token{
    char *Name;
    int Nb_Min_Arg; /* Nombre minimum d'argument */
    int Nb_Max_Arg; /* Nombre max d'argument (-1 -> illimite) */
};


typedef enum E_ACTION{
    A_NONE,
    A_END ,
    A_BACKG,
    A_APEN,
    A_OPEN,
    A_RECT,
    A_RECTF,
    A_CIRCLE,
    A_CIRCLEF,
    A_POLY,
    A_POLYF,
    A_ROTATE,
    A_FIVE_STAR,
    A_STAR_MULT,
    A_LAST
};

#define MAX_TOKEN   A_LAST

struct  Def_Token   Tab_Token[MAX_TOKEN]={
{"NONE"     ,0,0},
{"END"      ,0,0},
{"BACKG"    ,1,3},
{"APEN"     ,1,3},
{"OPEN"     ,1,3},
{"RECT"     ,4,4},
{"RECTF"    ,4,4},
{"CIRCLE"   ,3,4},      /* Si nombre de parametre=3 ->cercle,sinon ellipse */
{"CIRCLEF"  ,3,4},
{"POLY"     ,2,100},
{"POLYF"    ,2,100},
{"ROTATE"   ,1,1},
{"STAR"     ,2,4},      /* posx,posy,[size],[angle] */
{"STARM"    ,5,5}       /* posx,posy,nombre,size_ext,size_int */
};


typedef struct Def_Color{
    char    *Name;      /* Nom de la couleurs     */
    int r,v,b;      /* Valeurs des composantes*/
    int Num_Col;    /* Numero dans la palette */
};
struct  Def_Color   Tab_Color[]={
{ "black"   ,0,0,0      ,0},
{ "white"   ,0xff,0xff,0xff ,1},
{ "red"     ,0xff,0,0   ,2},
{ "green"   ,0,0xff,0   ,3},
{ "yellow"  ,0xff,0xff,0    ,4},
{ "blue"    ,0x00,0x00,0xff ,5},
{ "orange"  ,0xff,0x55,0x00 ,6},
{ ""        ,-1,-1,-1   ,-1}
};

#define MAX_ELEMS   1000    /* Nombre maximum d'items constituant un drap */

int dx,dy;

#define FLAG_X  3000    /* Taille du trabeau dans ses propres unitees */
#define FLAG_Y  2000    /* Taille du trabeau dans ses propres unitees */

CLIP    glob_clip;

void    InitColors(struct ColorMap *cm);

/**************************************************************************/

int CX(int x)
{
    return( (x*dx)/FLAG_X+glob_clip.minx);
}

/**************************************************************************/

int CY(int y)
{
    return( (y*dy)/FLAG_Y+glob_clip.miny);
}

/**************************************************************************/

void Flag_Draw(struct RastPort *rp,CLIP *p_clip,int *P_Tab,struct ColorMap *cm)
{
    int Nb_Arg;
    int i,Token;
    BOOL    Flg_End;
    int rx,ry,px,py,Sx,Sy;
    int Nb,Sext,Sint;
    float   Angle;


    static  Tab_Star[][2]={
        {-95,30},{-30,30},{0,100},{30,30},{95,30},{40,-0},
        {69,-70},{0,-20},{-69,-70},{-40,-0},{-1,-1}
        };
    glob_clip=*p_clip;
    dx=glob_clip.maxx-glob_clip.minx;
    dy=glob_clip.maxy-glob_clip.miny;

    InitColors(cm);

    BNDRYOFF(rp);
    Flg_End=FALSE;
    i=0;
    while(Flg_End==FALSE){
        Token=*P_Tab++;
        Nb_Arg=Token>>8;
        Token=Token&0xFF;
#ifdef DEBUG_LEV1
        printf("Token:%d Nb Arg:%d\n",Token,Nb_Arg);
#endif
        switch(Token){
            case A_NONE:
                /*printf("Action none!\n");*/
                break;
            case A_CIRCLEF:
            case A_CIRCLE:
                px=CX(P_Tab[0]);
                py=CY(P_Tab[1]);
                rx=CX(P_Tab[2]+P_Tab[0])-px;
                if(Nb_Arg==3){
                    ry=CY(P_Tab[2]+P_Tab[0])-CY(P_Tab[0]);
                }else{
                    ry=CY(P_Tab[3]+P_Tab[1])-CY(P_Tab[1]);
                }
                if(Token==A_CIRCLE){
                    DrawEllipse(rp,px,py,rx,ry);
                }else{
                    SetDrMd(rp,JAM2);
                    AreaEllipse(rp,px,py,rx,ry);
                    AreaEnd(rp);
                }
                break;
            case A_APEN:
                SetAPen(rp,Tab_Color[P_Tab[0]].Num_Col);
                break;
            case A_OPEN:
                SetOPen(rp,Tab_Color[P_Tab[0]].Num_Col);
                break;
            case A_RECT:
                Move(rp,CX(P_Tab[0]),CY(P_Tab[1]));
                Draw(rp,CX(P_Tab[2]),CY(P_Tab[1]));
                Draw(rp,CX(P_Tab[2]),CY(P_Tab[3]));
                Draw(rp,CX(P_Tab[0]),CY(P_Tab[3]));
                Draw(rp,CX(P_Tab[0]),CY(P_Tab[1]));
                break;
            case A_RECTF:
                RectFill(rp,            CX(P_Tab[0]),
                                CY(P_Tab[1]),
                                CX(P_Tab[2]),
                                CY(P_Tab[3])
                                );
                break;
            case A_POLY:
                Move(rp,CX(P_Tab[0]),CY(P_Tab[1]));
                for(i=2;i<Nb_Arg;i+=2){
                    Draw(rp,CX(P_Tab[i]),CY(P_Tab[i+1]));
                }
                break;

            case A_POLYF:
                AreaMove(rp,CX(P_Tab[0]),CY(P_Tab[1]));
                for(i=2;i<Nb_Arg;i+=2){
                    AreaDraw(rp,CX(P_Tab[i]),CY(P_Tab[i+1]));
                }
                AreaEnd(rp);
                break;

            case A_FIVE_STAR:
                if(Nb_Arg==2){
                    Sx=100;
                    Sy=100;
                }else{
                    Sx=P_Tab[2];
                    Sy=Sx;
                    if(Nb_Arg==4){
                        Angle=((float)P_Tab[3])*2*PI/360.0;
                    }else{
                        Angle=0;
                    }
                }
                i=0;
                while(Tab_Star[i][0]!=-1){
                    px=Tab_Star[i][0];
                    py=Tab_Star[i][1];
                    if(Angle!=0){
                        T3D_2D_Rot(Angle,&px,&py);
                    }
                    px=CX(P_Tab[0]+(px*Sx)/100);
                    py=CY(P_Tab[1]-(py*Sy)/100);
                    if(i==0){
                        AreaMove(rp,px,py);
                    }else{
                        AreaDraw(rp,px,py);
                    }
                    i++;
                }
                AreaEnd(rp);
                break;


            case A_STAR_MULT:
                Nb=P_Tab[2];
                Sext=P_Tab[3];
                Sint=P_Tab[4];

                for(i=0;i<Nb;i++){
                    Angle=(i*2-1)*2*PI/(Nb*2);
                    px=0;
                    py=-Sint;
                    T3D_2D_Rot(Angle,&px,&py);
                    if(i==0){
                        AreaMove(rp,CX(P_Tab[0]+px),CY(P_Tab[1]+py));
                    }else{
                        AreaDraw(rp,CX(P_Tab[0]+px),CY(P_Tab[1]+py));
                    }

                    Angle=(i)*2*PI/(Nb);
                    px=0;
                    py=-Sext;
                    T3D_2D_Rot(Angle,&px,&py);
                    AreaDraw(rp,CX(P_Tab[0]+px),CY(P_Tab[1]+py));

                }
                AreaEnd(rp);
                break;

            case A_BACKG:
                SetAPen(rp,Tab_Color[P_Tab[0]].Num_Col);
                RectFill(rp,glob_clip.minx,
                    glob_clip.miny,
                    glob_clip.maxx,
                    glob_clip.maxy);

                break;
            case A_END:
                Flg_End=TRUE;
                break;
            default:
                /*printf("Unknow action:%d\n",Token); */
                break;
        }
        P_Tab+=Nb_Arg;
        /* On se previent d'un eventuel probleme de bouclage */
        i++;
        if(i>1000)Flg_End=TRUE;
    }
    SetAPen(rp,0);
    Move(rp,CX(0),CY(0));
    Draw(rp,CX(0),CY(FLAG_Y));
    Draw(rp,CX(FLAG_X),CY(FLAG_Y));
    Draw(rp,CX(FLAG_X),CY(0));
    Draw(rp,CX(0),CY(0));
    
}

/**************************************************************************/

int Find_Token(char *p_token)
{
    int i;

    i=0;
    while((i<MAX_TOKEN)&&(stricmp(Tab_Token[i].Name, p_token )!=0)){
        i++;
    }
    if(i==MAX_TOKEN)i=0;
#ifdef DEBUG_LEV2
    printf("Token:%d\n",i);
#endif
    return(i);
}

/**************************************************************************/

BOOL    Get_Elem(char **pp,char *p_elem)
{
    int i=0;
    char    *Deb;

    Deb=p_elem;
    *p_elem=0;
    while((*(*pp) !=0)&&(*(*pp)!=',')&&(**pp!='\n')&&(*(*pp)!=' ')
            &&(*(*pp)!=0x09)){
        *p_elem=**pp;
        p_elem++;
        (*pp)++;
        i++;
        *p_elem=0;
    }
    while((*(*pp)==',')||(**pp==0x09)||(*(*pp)==' ')){
        (*pp)++;
    }

    if(i==0){
        return(FALSE);
    }else{
        return(TRUE);
    }

}
/**************************************************************************/
int Find_Color(char *P_Buf)
{
    int i;
    struct  Def_Color   *P_Temp;

    P_Temp=Tab_Color;
    i=0;
    while((P_Temp->r!=-1)&&(stricmp(P_Temp->Name, P_Buf )!=0)){
        P_Temp++;
        i++;
        
    }
    if(P_Temp->r==-1)i=0;
#ifdef DEBUG_LEV1
    printf("Color:%s %d\n",P_Buf,i);
#endif
    return(i);
}

void    InitColors(struct ColorMap *cm)
{
    int i;
    struct  Def_Color   *P_Temp;
/*    struct  TagItem My_Tab[2]={
                OBP_FailIfBad,FALSE,TAG_END};*/

    LONG    Num_Col;

    P_Temp=Tab_Color;
    if(GfxBase->LibNode.lib_Version>=39){

        i=0;
        while(P_Temp->r!=-1){
            Num_Col=FindColor( cm,
                    P_Temp->r<<28,
                    P_Temp->v<<28,
                    P_Temp->b<<28,
                    -1
                    );
            P_Temp->Num_Col=Num_Col;
            P_Temp++;
        }
    }else{
        /*printf("Fct a ecrire pour WB 2.0!\n");*/
    }
}

/**************************************************************************/

BOOL    read_one(FILE *fp,int   **p_current)
{
    BOOL    Flg_Cont,Encore;
    int Token;
    char    Buffer_Temp[BUFFER_SIZE],*P_Buf;
    char    Buffer_Token[BUFFER_SIZE];
    int Data,Nb_Var;
    int *p_avant;

    Flg_Cont=TRUE;

    /* On lit une ligne, tout en supprremant les lignes */
    /* de commentaires qui commencent par un ;      */
    do{
        P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
    }while((P_Buf!=NULL)&&(P_Buf[0]==';'));

    if(P_Buf==NULL){
        Flg_Cont=FALSE;
    }else{
#ifdef DEBUG_LEV1
        printf("Ligne:%s\n",P_Buf);
#endif
        Encore=Get_Elem(&P_Buf,Buffer_Token);
        Flg_Cont=Encore;
        if(Encore){
            p_avant=*p_current;
            (*p_current)++;
            Token=Find_Token(Buffer_Token);
            Nb_Var=0;
            while(Encore){
                Encore=Get_Elem(&P_Buf,Buffer_Token);
                if(Encore){
#ifdef DEBUG_LEV2
                    printf("Elem:%s\n",Buffer_Token);
#endif
                    if(isalpha(Buffer_Token[0])){
                        Data=Find_Color(Buffer_Token);
                    }else{
                        Data=atoi(Buffer_Token);

                    }
                    *(*p_current)=Data;
                    (*p_current)++;
                    Nb_Var++;
                }
            }
            if((Nb_Var < Tab_Token[Token].Nb_Min_Arg)||
               (Nb_Var > Tab_Token[Token].Nb_Max_Arg)){
                /*printf("Erreur sur le nombre de parametres!\n");*/
            }else{
#ifdef DEBUG_LEV1
                printf("Token:%d Nb_Var:%d\n",Token,Nb_Var);
#endif
            }
            *p_avant=Token + (Nb_Var<<8);
        }

    }
    **p_current=A_END;
    return(Flg_Cont);
}

BOOL Flag_Read(char *p_filename,int **PP_Tab_Drapeau)
{
    BOOL    Flg_Cont;
    FILE    *fp;
    int *P_Tab;

    *PP_Tab_Drapeau=malloc(sizeof(int)*(MAX_ELEMS));

    fp=fopen(p_filename,"r");
    if(fp!=NULL){
        P_Tab=*PP_Tab_Drapeau;
        Flg_Cont=TRUE;
        while(Flg_Cont==TRUE){
            Flg_Cont=read_one(fp,&P_Tab);
        }
        fclose(fp);
        return(TRUE);
    }else{
#ifdef DEBUG_LEV1
        printf("ERREUR: Ouverture fichier drapeau:%s!\n",p_filename);
#endif
        return(FALSE);
    }
}

