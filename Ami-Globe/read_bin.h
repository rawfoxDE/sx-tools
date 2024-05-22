/************************************************************************/
/*                                                                      */
/*      fichier         : read_bin.h                                    */
/*      projet          : amiglobe                                      */
/*      date création   : 27/08/94                                      */
/*      commentaire     : prototypes des fonctions de                   */
/*                        lecture des fichiers new_lac.Pt_bin           */
/*                        création du tableau des points                */
/*      révision        : $VER: Nom 1.001 (27 Aug 1994) */
/*      copyright       : Olivier Collard, Thomas Landspurg             */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*      Variables externes                                              */
/************************************************************************/



/************************************************************************/
/*      Prototypes                                                      */
/************************************************************************/

BOOL 
read_map(char *p_filename);
BOOL 
read_map_bin(char *p_filename);
BOOL    
read_zones(char *p_filename, struct  RastPort        *rpG);
void    
save_zones( char *p_filename);
void
close_files(void);
