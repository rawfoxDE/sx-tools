
/****************************************************************/
/*                                                              */
/*     fichier      : database.c                                */
/*     projet       : amiglobe                                  */
/*     date création: 1994                                      */
/*     commentaire  : gestion des données sur les pays          */
/*     révision     : $VER: database.c 0.004 (15 Oct 1994)
/*     copyright    : © 1994 Olivier Collard                    */
/*     $HISTORY:                                                */
/*                                                              */
/*     15 Oct 1994 : 000.004 :  free tab_data enleve pour la liste MUI LV_classement
/*     12 Oct 1994 : 000.003 :  Type_Donnee
/*     12 Oct 1994 : 000.002 :  liste de classement des pays
/*                     1994 : 0.01 : version initiale           */
/*                                                              */
/****************************************************************/


#include <exec/types.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>

/* Prototypes */
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/alib_protos.h>
#include <clib/macros.h>

/* MUI */
#include <libraries/mui.h>

#include "amiglobe_types.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "database_protos.h"
#include "fichier.h"
#include "amiglobe_interf.h"
#include "map_divers.h"

extern  COUNTRY *tab_country;/* tableau renseigne ici */
extern  int max_country;
extern  struct  ObjApp  *My_App;
extern  PREFERENCE Pref;

#define BUFFER_SIZE 200
#define MAX_LIGNE   10  /* Nombre de lignes max du cmt sur le pays */
#define MAX_DATA    50
ONE_TAB_DATA    Tab_Data[MAX_DATA];
int Max_Data=0;
E_Type_Data Type_Donnee;

/*****************************************/
/* Recherche d'un pays en fct de son nom */
/*****************************************/

int 
Dtb_Find_Country_Pos(char   *P_Name)
{
    int i;
    int num_cty;

    num_cty=-1;
    i=0;
    while ((i<max_country)&&(num_cty==-1))
    {
        if(strcmp(tab_country[i].name,P_Name)==0)
        {
            num_cty=i;
        }
        else
        {
            i++;
        }
    }
    if (num_cty!=-1)
        return(num_cty);
    else
    {
        i=0;
        while ((i<max_country)&&(num_cty==-1))
        {
            int longueur1,longueur2;
            longueur1=strlen(P_Name);
            longueur2=strlen(tab_country[i].name);
            if(strncmp(tab_country[i].name,P_Name,MIN(longueur1,longueur2))==0)
            {
                num_cty=i;
            }
            else
            {
                i++;
            }
        }
        return num_cty;
    }
}


/*****************************************/
/* Recherche d'un pays en fct de son nom */
/*****************************************/

COUNTRY *
Dtb_Find_Country(char  *P_Name)
{
    int i;
    int Flg_Found;
    COUNTRY *Ctry_Tmp;

    Flg_Found=FALSE;
    Ctry_Tmp=tab_country;
    i=0;
    while ((i<max_country)&&(Flg_Found==FALSE))
    {

        if(strcmp(Ctry_Tmp->name,P_Name)==0)
        {
            Flg_Found=TRUE;
        }
        else
        {
            if(i>MAX_COUNTRY)
            {
                /*printf("Erreur, nbre maximum de pays depasse!\n");*/
            }
            else
            {           
                Ctry_Tmp++;
                i++;
            }
        }
    }
    if (Flg_Found == FALSE)
    {
        Ctry_Tmp = NULL;
    }
    return(Ctry_Tmp);
}

BOOL    
get_int_line(FILE *fp,int *p_int)
/* retourne un entier d'après la lecture d'une ligne d'un fichier texte */
{
    BOOL    Flg_Cont;
    char    Buffer_Temp[BUFFER_SIZE],*P_Buf;

    Flg_Cont=TRUE;

    P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);

    if (P_Buf==NULL)
    {
        Flg_Cont=FALSE;
    }
    else
    {
        *p_int=atoi(P_Buf);
    }
    return(Flg_Cont);
}

/* Initialise tout les champs d'une structure ctry a des valeurs correctes */

void    
Dtb_Init_Country(COUNTRY *In_Cty)
{
    static  char    *P_Dummy="Non Defini";

    In_Cty->name=P_Dummy;
    In_Cty->population=0;
    In_Cty->superficie=0;
    In_Cty->PIB=0;
    In_Cty->off_name=P_Dummy;
    In_Cty->presentation=P_Dummy;
    In_Cty->P_Elem_Rem=NULL;

    In_Cty->pnb1=0;
    In_Cty->pnb2=0;
    In_Cty->pnb3=0;

}


/* Lit la base de donnees concernant un pays */

BOOL    
read_one_cty(FILE   *fp)
{
    char    Buffer_Temp[BUFFER_SIZE];
    char    Buf_Cmt[MAX_LIGNE][BUFFER_SIZE];
    char    *P_Buf;
    char    *P_Car;
    BOOL    Flg_Cont;
    int Etape,i,Ligne;
    int Len,Len_Total;
/*  ELEM_REM * element;*/
    COUNTRY *P_Ctry;    /* Pointeur sur le pays courant     */
    static COUNTRY  Dummy_Ctry; /* Utilise si on ne trouve pas le pays  */
    
    Flg_Cont=TRUE;
    Etape=0;
    while ((Flg_Cont==TRUE)&&(Etape<20))
    {
      switch (Etape)
      {
        case 0:

        /* Lecture du nom du pays  A SUPPRIMER */

        P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
        if(P_Buf!=NULL)
        {
            /* Acquisition du nom du pays dans P_Buf */     
            P_Car=stpchr(P_Buf,'|'); /*P_Car pointe sur le premier |*/
            if (P_Car!=NULL) 
                *P_Car=0; 
            P_Car = stpchr(P_Buf,0x0a);
            if (P_Car!=NULL)
                *P_Car=0; 
            
            P_Ctry = Dtb_Find_Country(P_Buf);
            if(P_Ctry==NULL)
            {
                /*printf("ERREUR:Pays %s non trouvé...\n",P_Buf);*/
                P_Ctry=&Dummy_Ctry;
                Dtb_Init_Country(P_Ctry);
            };
        }
        else
        {
            Flg_Cont=FALSE;
        }
        break;

        case 1:

        /* Lecture du nom officiel */
        P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
        if(P_Buf==NULL)
        {
            Flg_Cont=FALSE;
        }
        else
        {
            /* Suppression du retour chariot */
            Len=strlen(P_Buf);
            P_Buf[Len-1]=0;
            P_Ctry->off_name=malloc(Len);
            strcpy(P_Ctry->off_name,P_Buf);
        }
        break;

         case 2:
        /* Lecture de la surface */
        Flg_Cont=get_int_line(fp,&P_Ctry->superficie);
        break;

         case 3:
        /* Lecture du nombre d'habitant */
        Flg_Cont=get_int_line(fp,&P_Ctry->population);
        break;

         case 4:
        /* Lecture du PIB */
        Flg_Cont=get_int_line(fp,&P_Ctry->PIB);
        break;
        
         case 5:
        {
        ELEM_REM ** P_P_element=&(P_Ctry->P_Elem_Rem);
        P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
        P_Ctry->P_Elem_Rem=NULL;
        while ((P_Buf[0]!='&') && (P_Buf!=NULL))
        {
            ELEM_REM * nouvel_element=(ELEM_REM *)malloc(sizeof(ELEM_REM));
            switch (atoi(strtok(P_Buf,"$")))
            {
                case 0:nouvel_element->Type=CAPITALE;
                    break;
                case 1:nouvel_element->Type=VILLE;
                    break;
                case 2:nouvel_element->Type=MONUMENT;
                    break;
                case 3:nouvel_element->Type=MONTAGNE;
                    break;
            }   
            nouvel_element->Nom=strdup(strtok(NULL,"$"));
            nouvel_element->Lat=-atoi(strtok(NULL,"$"));
            nouvel_element->Lon=atoi(strtok(NULL,"$"));
            nouvel_element->caracteristique=atoi(strtok(NULL,"$"));
            *P_P_element=nouvel_element;
            nouvel_element->P_Next=NULL;
            P_P_element=&(nouvel_element->P_Next);
            P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
/*          printf("Element remarquable pour %s : %s lon:%d lat:%d carac:%d\n",
                P_Ctry->name,(P_Ctry->P_Elem_Rem)->Nom,
                (P_Ctry->P_Elem_Rem)->Lon,(P_Ctry->P_Elem_Rem)->Lat,
                (P_Ctry->P_Elem_Rem)->caracteristique);*/
        }   
        }
        break;

         case 6:
        Ligne=0;
        Len_Total=0;
        do
        {
            P_Buf=fgets(&Buf_Cmt[Ligne][0],BUFFER_SIZE,fp);
            if (P_Buf==NULL)
            {
                Flg_Cont=FALSE;
            };
            Ligne++;
            Len=strlen(P_Buf);
            Len_Total+=Len;
        }
          while ((Ligne<MAX_LIGNE) && Flg_Cont && (P_Buf[Len-2]!='#') );

        if (Flg_Cont)
        {
            P_Ctry->presentation=(char *)malloc(Len_Total);
            if (P_Ctry->presentation!=NULL)
            {
                P_Ctry->presentation[0]=0;
                for(i=0;i<Ligne;i++)
                {
                    strcat(P_Ctry->presentation,Buf_Cmt[i]);
                }
                P_Ctry->presentation[Len_Total-2]='\n';
                P_Ctry->presentation[Len_Total-1]=0;
            }
        }
                
        break;

        case 7:


/*      P_Ctry->pnb1=0;
        P_Ctry->pnb2=0;
        P_Ctry->pnb3=0;*/


        /* Lecture du dernier retour chariot */
        P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
        break;
         default:
        break;
       }/* End of case */
       Etape++;
    }
    return(Flg_Cont);
}


/* Lit la base de donnees concernant un pays */

BOOL    
read_database(char *p_name)
{
    char    Buffer_Temp[BUFFER_SIZE];
    BOOL    Flg_Cont;
    FILE    *fp;
    BOOL    Flg_Err=FALSE;
    int i;
    sprintf(Buffer_Temp,"%s.cty",p_name);
    fp=fopen(Buffer_Temp,"r");
    if (fp!=NULL)
    {
        Flg_Cont=TRUE;
        while(Flg_Cont==TRUE)
        {
            Flg_Cont=read_one_cty(fp);
        }
        fclose(fp);
/*        Dtb_Read_Data_File("stats/population_1993.stat");
        if (Max_Data>0)
            for (i=0;i<max_country;i++)
                tab_country[i].population=
                    (int)(((int *)Tab_Data[Max_Data-1].P_Tab)[i]);

        Dtb_Read_Data_File("stats/superficie.stat");
        if (Max_Data>1)
            for (i=0;i<max_country;i++)
                tab_country[i].superficie=
                    (float)(((float *)Tab_Data[Max_Data-1].P_Tab)[i]);*/
    }
    else
    {
        printf("ERROR: Cannot open the file:%s!\n",p_name);
        Flg_Err=TRUE;
    }
    return Flg_Err;
}

/* Lecture d'un fichier de donnees externe pour un pays */
BOOL    
Dtb_Read_Data_File(char *FileName)
{
    char    Buffer_Temp[BUFFER_SIZE];
    char    P_Name[BUFFER_SIZE];
    char    *P_Buf,*P_Car;
    long int *P_Tab;
    BOOL    Flg_Cont;
    FILE    *fp;
    int len;
    int Num;
    //int Val;
    int Num_Data;
    BOOL    Flg_Err=TRUE;

    if(fp=fopen(FileName,"r"))
    {
        Flg_Cont=FALSE;

        P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
        if (P_Buf!=NULL)
        {
            if (Max_Data<MAX_DATA)
            {
                /* Préparation de la nouvelle donnée            */
                /* détection du type, initialisation du tableau */
                /* Rappel: format de la première ligne:         */
                /*  I Population                                */
                Num_Data=Max_Data;
                P_Tab=malloc(4*max_country);
                Tab_Data[Num_Data].P_Tab=P_Tab;
                if (P_Tab!=NULL)
                {
                    int i=1;
                    Max_Data++;
                    Tab_Data[Num_Data].P_FileName=strdup(FileName);
                    while ( P_Buf[i]==' ')
                        i++;
                    Tab_Data[Num_Data].Name=strdup(&P_Buf[i]);
                        Tab_Data[Num_Data].Facteur=1.0;                 
                    for (i=0;i<max_country;i++)
                        P_Tab[i]=-~1;
                    switch (P_Buf[0])
                    {
                    case 'I':

                        Tab_Data[Num_Data].SizeOf_Elem=sizeof(long int);
                        Tab_Data[Num_Data].Type=DATA_INTEGER;

                        break;
                    case 'F':
                        Tab_Data[Num_Data].SizeOf_Elem=sizeof(float);
                        Tab_Data[Num_Data].Type=DATA_FLOAT;

                        break;
                    case 'C':
                    

                        break;
                    case 'P':
                        Tab_Data[Num_Data].SizeOf_Elem=sizeof(float);
                        Tab_Data[Num_Data].Type=DATA_PERCENT;

                        break;
                    default:
                        printf("%s: unknown data type\n",FileName);
                        Flg_Cont=FALSE;
                    }
                Flg_Cont=TRUE;
                }
            }
            else
            {
                /*printf("Erreur. Nb maximum de fichiers externe atteint!\n");*/
            }
        }
        while ((Flg_Cont==TRUE))
        {
            /* lecture de toutes les lignes du fichier */
            P_Buf = fgets(Buffer_Temp,BUFFER_SIZE,fp);
            if(P_Buf!=NULL)
            {
                P_Car = strchr(P_Buf,':');
                if (P_Car == NULL)
                    break;
                len=P_Car-P_Buf;
                while (P_Buf[len-1]==' ')
                    len--;
                strncpy(P_Name,P_Buf,len);
                P_Name[len]=0;
                Num=Dtb_Find_Country_Pos(P_Name);
                if(Num!=-1)
                {
                    char * pos;
                    switch ( Tab_Data[Num_Data].Type)
                    {

                    case DATA_INTEGER:
                        P_Tab[Num]=(long int)atoi(&P_Car[1]);
                        break;
                    case DATA_FLOAT:
                        {
                            float val=atof(&P_Car[1]);
                            memmove(&P_Tab[Num],&val,4);
                        }
                        break;
                    case DATA_CHAR:
                        break;
                    case DATA_PERCENT:
                        {
                            float val;
                        pos=strchr(&P_Car[1],'%');
                        if (pos!=NULL)
                            pos[0]=0;/* on efface le caractère '%' */
                        val=atof(&P_Car[1]);
                        memmove(&P_Tab[Num],&val,4);/* bourrin, non?*/
                        /*val=0;
                        memmove(&val,&P_Tab[Num],4);*/
                        }
                        break;
                    default:
                        break;
                    }
                }
                Flg_Err=FALSE;
            }
            else
            {
                Flg_Cont=FALSE;
            }
        }
        fclose(fp);
    } /*fin de si ouverture fichier*/
    return(Flg_Err);
}

/*****************************************************************************/

/* Fonction de comparaison pour le type entier */

int 
cmp_int(DATA_TRI *data1,DATA_TRI *data2)
{
    if (data1->valeur.data_int<data2->valeur.data_int)
        return 1;
    else 
    if(data1->valeur.data_int>data2->valeur.data_int)
        return -1;
    else
        return 0;
}

/* Fonction de comparaison pour le type flottant */

int 
cmp_float(DATA_TRI *data1,DATA_TRI *data2)
{
    if (data1->valeur.data_float<data2->valeur.data_float)
        return 1;
    else 
    if (data1->valeur.data_float>data2->valeur.data_float)
        return -1;
    return 0;
}

/*****************************************************************************/

void 
Dtb_Aff_Result(struct RastPort *rpG,int In_Num,APTR Liste_Sel,E_Type_Graph In_Type_Graph)
{
    int  i,nb_data;
    int      nb_plages;     /* Nombre de plages */
    long int   valeur_max;
    COUNTRY  *P_Cty;
    DATA_TRI *tab_data;
    E_Type_Data type_data;
    int *P_Tab_int,num;
    float *P_Tab_float;
        
/*  LONG id = -1;*/
    COUNTRY * pays;

    /* Recopie de la selection courante */
    /* Temporaire               */
    tab_data=(DATA_TRI *)malloc(sizeof(DATA_TRI)*max_country);
    if (tab_data==NULL)
        return;
    Type_Donnee=Tab_Data[In_Num].Type;
    P_Cty=tab_country;
    switch (Tab_Data[In_Num].Type)
    {
    case DATA_INTEGER:
        P_Tab_int=(int *)Tab_Data[In_Num].P_Tab;
        break;
    case DATA_FLOAT:
        P_Tab_float=(float *)Tab_Data[In_Num].P_Tab;
        break;
    case DATA_PERCENT:
        P_Tab_float=(float *)Tab_Data[In_Num].P_Tab;
        break;      
    }
    /* création du tableau des données reliées à leur pays*/
    for (nb_data=0;;nb_data++)
    {

        DoMethod(Liste_Sel,MUIM_List_GetEntry,nb_data,&pays);
        if (!pays) break;

        num=Dtb_Find_Country_Pos(pays->name);
        
        if(num!=-1)
        {
            switch (Tab_Data[In_Num].Type)
            {
            case DATA_INTEGER:
                tab_data[nb_data].valeur.data_int=P_Tab_int[num];
                break;
            case DATA_FLOAT:
            case DATA_PERCENT:
                tab_data[nb_data].valeur.data_float=P_Tab_float[num];
                break;      
            }
        }
        else
        {
            switch (Tab_Data[In_Num].Type)
            {
            case DATA_INTEGER:
                tab_data[nb_data].valeur.data_int=-~1;
                break;
            case DATA_FLOAT:
            case DATA_PERCENT:
                tab_data[nb_data].valeur.data_int=-~1;
                break;      
            }
        }
        tab_data[nb_data].Num_Pays=num;
    }
    /*printf("nb_data:%d\n",nb_data);*/


    /* tri des donnees*/
    switch(Tab_Data[In_Num].Type)
    {
        case DATA_INTEGER:
           qsort( (void *)tab_data, nb_data , sizeof(DATA_TRI) , cmp_int );
            valeur_max=tab_data[0].valeur.data_int;         
            /*printf("valeur max:%d\n",valeur_max);*/
            break;
        case DATA_PERCENT:
        case DATA_FLOAT:
           qsort( (void *)tab_data, nb_data , sizeof(DATA_TRI) ,cmp_float);
            valeur_max=tab_data[0].valeur.data_float;
            /*valeur_max=tab_data[0].valeur.data_float;*/
            /*printf("valeur_max:%d\n",valeur_max);*/
            break;      
        default:
            break;
    }

    nb_plages=Map_Get_Plages(rpG , 10 , type_data , nb_data ,0,100,In_Type_Graph);
    //Map_Init_Graph(rpG,Tab_Data[In_Num].Name);
    /* Pour tout les elements de la sélection, on va afficher les données   */
    /* en fonction du nombre de plages                  */

    if(valeur_max!=0)
    {
        if (In_Type_Graph==0)
        {
            int rang=0;/* rang des pays */
            int selectionne=-1;/* si un pays est sélectionné*/
            set(My_App->LV_classement,MUIA_List_Quiet,TRUE);
            set(My_App->TX_classement,MUIA_Text_Contents,
                Tab_Data[In_Num].Name);
            /* données seules */
            DoMethod(My_App->LV_classement,MUIM_List_Clear);
            for(i=0;i<nb_data;i++)
            {
                /* les valeurs égales à -~1 ne sont pas prises en compte*/
                if(tab_data[i].valeur.data_int!=-~1)
                {
                    rang++;
                    tab_data[i].Rang=rang;/* le rang commence à 1 */
                    if (tab_data[i].Num_Pays==Pref.country_sel)
                        selectionne=i;
                    /* fonction de MUI 2.0 */
                    DoMethod(My_App->LV_classement,MUIM_List_InsertSingle,
                        &tab_data[i],MUIV_List_Insert_Bottom);
                }
            }
            if (selectionne!=-1)
            {
                DoMethod(My_App->LV_classement,MUIM_List_Jump,selectionne);
                DoMethod(My_App->LV_classement,MUIM_List_Select,selectionne,
                    MUIV_List_Select_On,NULL);
            }
            set(My_App->LV_classement,MUIA_List_Quiet,FALSE);
            set(My_App->WI_Classement,MUIA_Window_Open,TRUE);
        }
        else
        {
           int rang=0;
           /*valeur_max=valeur_max/nb_plages;*/
           for(i=0;i<nb_data;i++)
           {
            if(tab_data[i].valeur.data_int!=-~1)
            {
                rang++;
                tab_data[i].Rang=rang;
            }
           }
           switch (In_Type_Graph)
           {
           case GRAPH_COL:
                aff_donnees(rpG,tab_data,valeur_max,type_data,nb_plages,nb_data);
                break;
           default:
                break;

           }
                    /*&tab_data[i],
                    (int)((tab_data[i].valeur.data_int)/valeur_max)
                    );*/


        }
    }
    if (valeur_max==0 || In_Type_Graph!=0)
        free(tab_data);
    Map_End_Graph(rpG);
}
/***********************************************************************/
void
Dtb_Create_Liste_Data(APTR List)
{
    int i;
    
/*    for(i=0;i<Max_Data;i++){
        DoMethod(List,
            MUIM_List_Remove,MUIV_List_Remove_First);
    
    }*/
    DoMethod(List,MUIM_List_Clear);
    for (i=0;i<Max_Data;i++)
    {
        DoMethod(List,
            MUIM_List_Insert,&Tab_Data[i].Name,1,
            MUIV_List_Insert_Bottom);
    }
}

/***********************************************************************/
BOOL    
Dtb_Read_Stats(void)
{
    static BOOL Initialise=FALSE;
    /* si on ne trouve aucun fichier de stat, on continue quand même*/
    BOOL    Flg_Err=FALSE;
    if (Initialise==FALSE)
    {
        char * nom_fichier=ouvrir_nouveau_fichier("Stats/#?.stat");
        while (nom_fichier!=NULL)
        {
            char buffer[256];
            strcpy(buffer,"stats/");
            /*Flg_Err=Dtb_Read_Data_File(buffer);
            Flg_Err=Dtb_Read_Data_File("stats/chomage.stat");
            Flg_Err=Dtb_Read_Data_File("stats/death_rate.stat");
            Flg_Err=Dtb_Read_Data_File("stats/superficie.stat");
            Flg_Err=Dtb_Read_Data_File("stats/land_boundaries.stat");*/
            Flg_Err=Dtb_Read_Data_File(strcat(buffer,nom_fichier));
            nom_fichier=ouvrir_nouveau_fichier("Stats/#?.stat");
        }
        Dtb_Create_Liste_Data(My_App->LV_Datas);
        Initialise=TRUE;
    }
    return(Flg_Err);
}

