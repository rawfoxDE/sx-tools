typedef enum {POPULATION,SUPERFICIE,PAR_HABITANT,AUTRE,PNB,
				DENSITE,PNB_HAB}E_Type_Donnee;

typedef enum {DATA_INTEGER,DATA_FLOAT,DATA_CHAR,DATA_PERCENT,DATA_AUTRE}E_Type_Data;

typedef enum {GRAPH_SIMPLE,GRAPH_BAR,GRAPH_BAR3D,GRAPH_COL}E_Type_Graph;

typedef	struct	One_Tab_Data
{
	E_Type_Data	Type;
	int	SizeOf_Elem;
	float	Facteur;	/* Facteur multiplicatif		*/
	char	*Name;		/* Ptr sur le nom a affiche 		*/
	char	*P_FileName;	/* Ptr sur le nom du fichier source	*/
	char	*Descript;	/* Ptr sur le texte descriptif		*/
	APTR	P_Tab;
}ONE_TAB_DATA;

typedef	struct	data_tri
{
	union	
    {
		int	data_int;
		float	data_float;
		char	*data_char;
	}  valeur;
	WORD	Num_Pays;
	WORD 	Rang;
}  DATA_TRI;

