/************************************************/
/* chargement d'un drapeau			*/
/************************************************/

extern BOOL Flag_Read(char *p_filename,int **PP_Tab_Drapeau);

/***********************************************/
/* Affichage d'un drapeau prealablement charge */
/***********************************************/

extern void Flag_Draw(struct RastPort *rp,CLIP *p_clip,int *P_Tab,struct ColorMap *cm);
