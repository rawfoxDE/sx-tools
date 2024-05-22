/************************************************************************/
/*                                                                      */
/*      fichier         : projection.h                                  */
/*      projet          : amiglobe                                      */
/*      date création   : 10/09/94                                      */
/*      commentaire     : fonctions de conversion de                    */
/*                        points écran <-> point de la carte            */
/*      révision        : $VER: projection.h 1.002 (10 Sep 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/
void
Update_New_Clip(void);

void conv_clip(void);
/* converti un point écran en un point de la carte*/
void conv_inv_xy(
        int     *px,int *py
        );
/* converti un point de la carte en point écran*/
void conv_xy(int *px,int *py);
void 
correct_clip(CLIP * clip);


