/************************************************************************/
/*                                                                      */
/*      fichier         : read_org.h                                    */
/*      projet          : amiglobe                                      */
/*      date création   : 28/08/94                                      */
/*      commentaire     : lecture du fichier donnees/organisations      */
/*      révision        : $VER: read_org.h 1.003 (10 Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      prototypes                                                      */
/************************************************************************/
/* création de la liste des organisations, remplissage de l'objet cycle */
/* et appel hook correspondant                                          */
void
Create_CY_organisation(void);
/* effacement de la liste des organisations et fermeture du fichier     */
void
Dispose_Organisation(void);

