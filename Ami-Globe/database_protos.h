BOOL	read_database(char *p_name);
void	Dtb_Init_Country(COUNTRY *In_Cty);
int	Dtb_Find_Country_Pos(char	*P_Name);
BOOL	Dtb_Read_Data_File(char *FileName);
void Dtb_Aff_Result(struct RastPort *rpG,
	int	In_Num,APTR Liste_Sel,E_Type_Graph In_Type_Graph);
void	Dtb_Create_Liste_Data(APTR List);
BOOL	Dtb_Read_Stats(void);

