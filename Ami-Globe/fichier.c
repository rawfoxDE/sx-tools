/****************************************************************/
/*                                                              */
/*     fichier      : fichier.c                                 */
/*     projet       : Amiglobe                                  */
/*     date création: mars 1994                                 */
/*     commentaire  : fonctions d'accès aux fichiers            */
/*     révision     : $VER: fichier.c 0.002 (04 Oct 1994)       */
/*     copyright    : © 1994 Olivier Collard                    */
/*     $HISTORY:                                                */
/*                                                              */
/*              mars 1994 : 0.01 : version initiale, projet     */
/*                                 Shannon-Fano                 */
/*              4 oct 1994 : 0.002 : version initiale, projet   */
/*                                  Amiglobe                    */
/*                                                              */
/****************************************************************/




/* includes spécifiques pour Amiga: */
#include <dos/dosasl.h>   /* gestion des jokers */
#include <clib/dos_protos.h> /* gestion des jokers */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fichier.h"

/* -------------------------- ouvrir_nouveau_fichier ---------------------------

 Comment: a chaque appel, donne un pointeur sur un fichier
          correspondant à nom
          si nom change, reprise depuis le début

*/

char  *
ouvrir_nouveau_fichier(char * nom)
// fonction specifique a chaque systeme (noms)
{
    static struct AnchorPath __aligned * anchor=NULL;
    // spécifique AmigaDOS: contient le 'pattern' à mettre en
    // correspondance avec les noms de fichiers
    // vaut NULL au premier appel
    /*char nom_fichier [32];*/
    /*FILE * fichier=NULL;  */
    static char * pattern="";
    // pattern de recherche; s'il est different de nom, 
    // il faut recommencer la recherche
    if (strcmp(pattern,nom)!=0)
    {
        anchor=NULL;
        free(pattern);
        pattern=strdup(nom);
    }
    if (anchor==NULL)
    {
        // initialisation de anchor lors du premier appel
        anchor = (struct AnchorPath __aligned *)malloc
                (sizeof(struct AnchorPath ));
        memset(anchor,0,sizeof(struct AnchorPath));

        // dans le cas du premier appel
        if (MatchFirst(pattern,anchor)!=0)
        {
            /*printf("Erreur dans la recherche du fichier\n");*/
            return NULL;
        }
        /*printf("trouve:%s\n",(anchor->ap_Info).fib_FileName);*/
        // recherche d'un fichier
        return (anchor->ap_Info).fib_FileName;
    }
    else
    {
        // recherche d'autres fichiers
        if (MatchNext(anchor)!=0)
        {
            /*printf("pas d'autres fichiers trouves\n");*/
            // si ne trouve plus d'autres fichiers,
            // positions_nom[i] vaut "" et positions[i] vaut
            // la position de fin du dernier fichier
            MatchEnd(anchor);
            // fermeture de anchor
            return NULL;
        }
        else
        {
            /*printf("trouve:%s\n",(anchor->ap_Info).fib_FileName);*/
            return (anchor->ap_Info).fib_FileName;
        }
    }
}

