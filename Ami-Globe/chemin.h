/************************************************************************/
/*                                                                      */
/*      fichier         : chemin.h                                      */
/*      projet          : amiglobe                                      */
/*      date création   : 31/08/94                                      */
/*      commentaire     : calcul et dessin du plus court chemin entre   */
/*                        le point origine et le point destination      */
/*      révision        : $VER: chemin.h 1.001 (31 Aug 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/



/************************************************************************/
/*      fonctions publiques                                             */
/************************************************************************/

void
Update_Points(void);/* cette fonction DOIT ETRE APPELEE à chaque */
                    /* changement des points orig ou dest        */

void 
Draw_Chemin(struct RastPort * rpG);
