/************************************************************************/
/*                                                                      */
/*      fichier         : read_org.c                                    */
/*      projet          : amiglobe                                      */
/*      date création   : 28/08/94                                      */
/*      commentaire     : lecture du fichier donnees/organisations      */
/*      révision        : $VER: read_org.c 1.008 (10 Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      includes                                                        */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <utility/hooks.h>
#include <clib/alib_protos.h>

#include "read_org.h"
#include "amiglobe_interf.h"

/************************************************************************/
/*      variables externes                                              */
/************************************************************************/
extern struct  ObjApp  *My_App;

/************************************************************************/
/*      variables privées                                               */
/************************************************************************/
FILE * OrganisationFile=NULL;
long int OrganisationPosition[50]; /* position de la description de l'organisation
                             dans le fichier*/
char ** OrganisationList=NULL;
/************************************************************************/
/*      fonctions privées                                               */
/************************************************************************/
void
Show_Organisation_Description(int numero);


/************************************************************************/
/*      defines                                                         */
/************************************************************************/
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds

/************************************************************************/
/*      hooks                                                           */
/************************************************************************/

void SAVEDS ASM cycle_func(void)
{
    ULONG sel;
    get(My_App->CY_organisation,MUIA_Cycle_Active,&sel);
    Show_Organisation_Description(sel);
}

struct Hook cycle_hook =
{
        {NULL,NULL},
        (void *)cycle_func,
        NULL,NULL
};

/************************************************************************/
/*      implémentation                                                  */
/************************************************************************/

/* -------------------------- Create_CY_Organisation ---------------------------

 Commentaire:crée le cycle gadget CY_Organisation en fonction de ce qu'il y a
             dans le fichier donnees/organisations

*/

void
Create_CY_organisation(void)
{
    char * buffer=(char *)malloc(8192);
    int i=1;
    OrganisationList=(char **)malloc(50*sizeof(char *));
/*    int j; */

    OrganisationFile= fopen ("donnees/organisations","r");
    if (OrganisationFile!=NULL)
    {
        fgets(buffer,8192,OrganisationFile);
        OrganisationList[0]=(char *)malloc(strlen(buffer));
        strncpy(OrganisationList[0],buffer,strlen(buffer)-1);
        OrganisationList[0][strlen(buffer)-1]='\0';
        OrganisationPosition[0]=ftell(OrganisationFile);
        while (buffer[0]!='#')
        {
            while (buffer[0]!='$')
                fgets(buffer,8192,OrganisationFile);
            fgets(buffer,8192,OrganisationFile);
            if (buffer[0]!='#')
            {
                OrganisationList[i]=(char *)malloc(strlen(buffer));
                strncpy(OrganisationList[i],buffer,strlen(buffer)-1);
                OrganisationList[i][strlen(buffer)-1]='\0';
                OrganisationPosition[i]=ftell(OrganisationFile);
                i++;
            }
            else
                OrganisationList[i]=NULL;
        }
/*        MUI_DisposeObject(My_App->CY_organisation); */
        set(My_App->CY_organisation,MUIA_Cycle_Entries,OrganisationList);
        DoMethod(My_App->CY_organisation,MUIM_Notify,MUIA_Cycle_Active,
                MUIV_EveryTime, My_App->CY_organisation,2,MUIM_CallHook,&cycle_hook);
    }
/*    for (j=0;j<i;j++)
        free(OrganisationList[j]);*/
/*    free(OrganisationList); */
    free(buffer);
    get(My_App->CY_organisation,MUIA_Cycle_Active,&i);
    Show_Organisation_Description(i);
}

/* ----------------------- Show_Organisation_Description -----------------------

 Commentaire:crée le floattext correspondant au nom de la description en paramètre

*/

void
Show_Organisation_Description(int numero)
{
    if (OrganisationFile !=NULL)
    {
        char * buffer=(char *)malloc(8192);
        char * texte=(char *)malloc(8192*2);
        texte[0]='\0';
        fseek(OrganisationFile,OrganisationPosition[numero],SEEK_SET);
        fgets(buffer,8192,OrganisationFile);
/*        set(My_App->TX_nombre_adherent,
            MUIA_Text_Contents,buffer);*/
        fgets(buffer,8192,OrganisationFile);
        set(My_App->TX_organisation_officiel,
            MUIA_Text_Contents,buffer);
        fgets(buffer,8192,OrganisationFile);
        while (buffer[0]!='$')
        {
            texte=strcat(texte,buffer);
            fgets(buffer,8192,OrganisationFile);
        }
        set(My_App->LV_organisation,MUIA_Floattext_Justify,TRUE);
        set(My_App->LV_organisation,MUIA_Floattext_Text,texte);
        free(texte);
        free(buffer);
    }
}
/* ---------------------------- Dispose_Organisation ----------------------------

 Commentaire:libère la mémoire occupée par la liste des organisations
             clôt le fichier de description
*/

void
Dispose_Organisation(void)
{
    int i=0;
    if (OrganisationList!=NULL)
    {
        while (OrganisationList[i]!=NULL)
            free(OrganisationList[i++]);
        free(OrganisationList);
    }
    if (OrganisationFile!=NULL)
        fclose(OrganisationFile);
}

