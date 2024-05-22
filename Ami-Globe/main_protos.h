/* Prototypes for functions defined in
main.c
 */

void quit(void );
void cmd1(void );
void cmd_redraw(void );
void cmd_fill(void );
void cmd_select_country(void );
void cmd_sel_continent(void );
void cmd_localisation(void);
void cmd_info(void );
void cmd_move_pt(void );
void cmd_show_pt(void );
void cmd_zoom(void );
void cmd_zoom_out(void );
void cmd_edit(void );

void aff_distance(void);
void aff_surface(void);
void aff_elem_rem(struct RastPort *,int numero);
void reselect(struct RastPort *);

void HandleEvent(APTR object);
void WaitButton(void);
void main_loop(void);

void Aff_Error(char *Texte,char *Bouton);
