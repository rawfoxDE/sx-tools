#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifdef _DCC
#define __inline
#endif

#include "AmiGlobe_Interf.h"

struct ObjApp * CreateApp(void)
{
	struct ObjApp * Object;

	APTR	GROUP_ROOT_Cmd, GR_commandes, GR_infos, GR_infos_latlon, LA_lon, LA_latitude;
	APTR	GR_grp_pays_courant, LA_Sel, LA_courant, GROUP_ROOT_parametres, GR_Param_Gene;
	APTR	LA_Lakes, LA_Rivers, LA_Latitudes, LA_label_Country_Name, LA_coord;
	APTR	LA_Fill_Pays, LA_Elements, LA_Trajectoires, LA_Distance, LA_Scale;
	APTR	LA_Area, GR_grp_12, GR_3d, obj_aux0, obj_aux1, obj_aux2, obj_aux3;
	APTR	obj_aux4, obj_aux5, Space_0, GROUP_ROOT_2, GROUP_ROOT_4, obj_aux6;
	APTR	obj_aux7, obj_aux8, obj_aux9, obj_aux10, obj_aux11, GROUP_ROOT_5;
	APTR	GROUP_ROOT_6, GR_grp_11, GROUP_ROOT_10, GR_grp_30, GR_grp_24, Space_1;
	APTR	GR_Edit_Donnee, GR_File_Donnee, GR_Virtuel, obj_aux12, obj_aux13;
	APTR	GR_Infos_Donnees, LA_Nom_Titre, LA_Nom_Label, LA_Type_Titre, LA_Titre_Label;
	APTR	Space_2, GR_Operations, GROUP_ROOT_8, GR_Vertical_Gauche, GR_Horizontal_Gauche;
	APTR	GR_Vertical_Centre, GR_Action, GR_Vertical_Droite, GROUP_ROOT_13;
	APTR	GR_grp_28, obj_aux14, obj_aux15, GR_grp_29, GROUP_ROOT_organisation;
	APTR	GR_info_organisation, LA_organisation_officiel, GROUP_ROOT_INFO_ELEM;
	APTR	GR_grp_41, LA_lat, GR_grp_42, LA_latitude_element, GROUP_ROOT_17;
	APTR	GR_grp_45, LA_classement, GROUP_ROOT_info, REC_label_0;

	if (!(Object = AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);

	Object->STR_TX_lon = NULL;
	Object->STR_TX_lat = NULL;
	Object->STR_TX_Selectionne = NULL;
	Object->STR_TX_courant = NULL;
	Object->STR_TX_Selected_Element = NULL;
	Object->STR_TX_Pays = NULL;
	Object->STR_TX_Latitude = NULL;
	Object->STR_TX_Longitude = NULL;
	Object->STR_TX_Caracteristique = NULL;
	Object->STR_TX_organisation_officiel = NULL;
	Object->STR_TX_nom_element = NULL;
	Object->STR_TX_type_element = NULL;
	Object->STR_TX_pays_element = "?";
	Object->STR_TX_longitude_element = "?";
	Object->STR_TX_latitude_element = "?";
	Object->STR_TX_caracteristique_element = NULL;
	Object->STR_TX_classement = NULL;

	Object->CY_ProjContent[0] = "flat";
	Object->CY_ProjContent[1] = "Mercator's";
	Object->CY_ProjContent[2] = "spherical";
	Object->CY_ProjContent[3] = "3D";
	Object->CY_ProjContent[4] = NULL;
	Object->RA_Type_DataContent[0] = "Donnee Reel";
	Object->RA_Type_DataContent[1] = "Donnee Virtuelle (formee d'autres donnees)";
	Object->RA_Type_DataContent[2] = NULL;
	Object->RA_label_4Content[0] = "X";
	Object->RA_label_4Content[1] = "/";
	Object->RA_label_4Content[2] = "+";
	Object->RA_label_4Content[3] = "-";
	Object->RA_label_4Content[4] = NULL;
	Object->RA_Type_GraphContent[0] = "Data only";
	Object->RA_Type_GraphContent[1] = "BarChart data (2d)";
	Object->RA_Type_GraphContent[2] = "BarChart 3D";
	Object->RA_Type_GraphContent[3] = "Color plages";
	Object->RA_Type_GraphContent[4] = NULL;
	Object->CY_organisationContent[0] = "error: informations not available";
	Object->CY_organisationContent[1] = NULL;

	Object->BT_Zoom = SimpleButton("_Zoom In");

	Object->BT_Zoom_Out = SimpleButton("Z_oom Out");

	Object->BT_Redraw = SimpleButton("_Redraw");

	Object->BT_Zoom_Reset = SimpleButton("Rese_t");

	Object->BT_Info = SimpleButton("_Infos");

	Object->CY_Proj = CycleObject,
		MUIA_HelpNode, "CY_Proj",
		MUIA_Cycle_Entries, Object->CY_ProjContent,
	End;

	GR_commandes = GroupObject,
		MUIA_Weight, 0,
		MUIA_Group_Horiz, TRUE,
		Child, Object->BT_Zoom,
		Child, Object->BT_Zoom_Out,
		Child, Object->BT_Redraw,
		Child, Object->BT_Zoom_Reset,
		Child, Object->BT_Info,
		Child, Object->CY_Proj,
	End;

	LA_lon = Label("Lon:");

	Object->TX_lon = TextObject,
		MUIA_HelpNode, "TX_lon",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Frame, MUIV_Frame_Text,
		MUIA_Text_Contents, Object->STR_TX_lon,
	End;

	LA_latitude = Label("Lat:");

	Object->TX_lat = TextObject,
		MUIA_HelpNode, "TX_lat",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Frame, MUIV_Frame_Text,
		MUIA_Text_Contents, Object->STR_TX_lat,
	End;

	GR_infos_latlon = GroupObject,
		MUIA_Weight, 45,
		MUIA_Group_Horiz, TRUE,
		Child, LA_lon,
		Child, Object->TX_lon,
		Child, LA_latitude,
		Child, Object->TX_lat,
	End;

	LA_Sel = Label("sel:");

	Object->TX_Selectionne = TextObject,
		MUIA_HelpNode, "TX_Selectionne",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Frame, MUIV_Frame_Text,
		MUIA_Text_Contents, Object->STR_TX_Selectionne,
		MUIA_Text_SetMin, TRUE,
	End;

	LA_courant = Label("   ");

	Object->TX_courant = TextObject,
		MUIA_HelpNode, "TX_courant",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Frame, MUIV_Frame_Text,
		MUIA_Text_Contents, Object->STR_TX_courant,
	End;

	GR_grp_pays_courant = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, LA_Sel,
		Child, Object->TX_Selectionne,
		Child, LA_courant,
		Child, Object->TX_courant,
	End;

	GR_infos = GroupObject,
		MUIA_Weight, 0,
		MUIA_Group_Horiz, TRUE,
		Child, GR_infos_latlon,
		Child, GR_grp_pays_courant,
	End;

	GROUP_ROOT_Cmd = GroupObject,
		Child, GR_commandes,
		Child, GR_infos,
	End;

	Object->WI_Cmd = WindowObject,
		MUIA_Window_Title, "AmiGlobe v0.99",
		MUIA_HelpNode, "WI_Cmd",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		MUIA_Window_SizeGadget, FALSE,
		WindowContents, GROUP_ROOT_Cmd,
	End;

	LA_Lakes = Label("Lakes");

	Object->CH_Lacs = CheckMark(FALSE);

	LA_Rivers = Label("Rivers");

	Object->CH_Riv = CheckMark(FALSE);

	LA_Latitudes = Label("Squaring");

	Object->CH_LonLat = CheckMark(FALSE);

	LA_label_Country_Name = Label("Country Names");

	Object->CH_Noms = CheckMark(FALSE);

	LA_coord = Label("Coordinates");

	Object->CH_Coord = CheckMark(FALSE);

	LA_Fill_Pays = Label("Fill Countries");

	Object->CH_Fill_Pays = CheckMark(FALSE);

	LA_Elements = Label("Elements");

	Object->CH_Elements = CheckMark(FALSE);

	LA_Trajectoires = Label("Paths");

	Object->CH_Trajectoires = CheckMark(FALSE);

	LA_Distance = Label("Distance");

	Object->CH_Distance = CheckMark(FALSE);

	LA_Scale = Label("Scale");

	Object->CH_Scale = CheckMark(FALSE);

	LA_Area = Label("Area");

	Object->CH_Area = CheckMark(FALSE);

	GR_Param_Gene = GroupObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Show:",
		MUIA_Group_Columns, 2,
		Child, LA_Lakes,
		Child, Object->CH_Lacs,
		Child, LA_Rivers,
		Child, Object->CH_Riv,
		Child, LA_Latitudes,
		Child, Object->CH_LonLat,
		Child, LA_label_Country_Name,
		Child, Object->CH_Noms,
		Child, LA_coord,
		Child, Object->CH_Coord,
		Child, LA_Fill_Pays,
		Child, Object->CH_Fill_Pays,
		Child, LA_Elements,
		Child, Object->CH_Elements,
		Child, LA_Trajectoires,
		Child, Object->CH_Trajectoires,
		Child, LA_Distance,
		Child, Object->CH_Distance,
		Child, LA_Scale,
		Child, Object->CH_Scale,
		Child, LA_Area,
		Child, Object->CH_Area,
	End;

	Object->SL_x = SliderObject,
		MUIA_HelpNode, "SL_x",
		MUIA_Slider_Min, 0,
		MUIA_Slider_Max, 180,
		MUIA_Slider_Level, 0,
	End;

	obj_aux1 = Label2("X");

	obj_aux0 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux1,
		Child, Object->SL_x,
	End;

	Object->SL_y = SliderObject,
		MUIA_HelpNode, "SL_y",
		MUIA_Slider_Min, 0,
		MUIA_Slider_Max, 180,
		MUIA_Slider_Level, 0,
	End;

	obj_aux3 = Label2("Y");

	obj_aux2 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux3,
		Child, Object->SL_y,
	End;

	Object->SL_z = SliderObject,
		MUIA_HelpNode, "SL_z",
		MUIA_Slider_Min, 0,
		MUIA_Slider_Max, 180,
		MUIA_Slider_Level, 0,
	End;

	obj_aux5 = Label2("Z");

	obj_aux4 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux5,
		Child, Object->SL_z,
	End;

	Object->IM_Rectangle = ImageObject,
		MUIA_Image_Spec, 32,
		MUIA_HelpNode, "IM_Rectangle",
		MUIA_Frame, MUIV_Frame_ImageButton,
		MUIA_Image_FreeVert, TRUE,
		MUIA_Image_FreeHoriz, TRUE,
		MUIA_FixHeight, 50,
		MUIA_FixWidth, 100,
	End;

	GR_3d = GroupObject,
		MUIA_Weight, 101,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "3D",
		Child, obj_aux0,
		Child, obj_aux2,
		Child, obj_aux4,
		Child, Object->IM_Rectangle,
	End;

	Space_0 = HVSpace;

	GR_grp_12 = GroupObject,
		Child, GR_3d,
		Child, Space_0,
	End;

	GROUP_ROOT_parametres = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, GR_Param_Gene,
		Child, GR_grp_12,
	End;

	Object->WI_Param = WindowObject,
		MUIA_Window_Title, "Display Parameters",
		MUIA_HelpNode, "WI_Param",
		MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_parametres,
	End;

	Object->LV_Pays = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Pays = ListviewObject,
		MUIA_HelpNode, "LV_Pays",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_DoubleClick, TRUE,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Pays,
	End;

	GROUP_ROOT_2 = GroupObject,
		Child, Object->LV_Pays,
	End;

	Object->WI_Sel_Cty = WindowObject,
		MUIA_Window_Title, "Country",
		MUIA_HelpNode, "WI_Sel_Cty",
		MUIA_Window_ID, MAKE_ID('2', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_2,
	End;

	Object->BT_Move = SimpleButton("_Move_Pt");

	Object->BT_Show_Pt = SimpleButton("_Show Pt");

	Object->BT_Show_Seg = SimpleButton("_Show Seg");

	Object->BT_Edit = SimpleButton("_edit");

	Object->BT_Create_Pays = SimpleButton("_Cree Pays");

	Object->BT_New_Border = SimpleButton("New border");

	Object->BT_New_Point = SimpleButton("New Point");

	Object->BT_End_Border = SimpleButton("End Border");

	Object->STR_Nom_Border = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_Nom_Border",
	End;

	obj_aux7 = Label2("nom:");

	obj_aux6 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux7,
		Child, Object->STR_Nom_Border,
	End;

	Object->STR_Code = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_Code",
	End;

	obj_aux9 = Label2("Code");

	obj_aux8 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux9,
		Child, Object->STR_Code,
	End;

	Object->SL_Code_Point = SliderObject,
		MUIA_HelpNode, "SL_Code_Point",
		MUIA_Slider_Min, 0,
		MUIA_Slider_Max, 5,
		MUIA_Slider_Level, 0,
	End;

	obj_aux11 = Label2("Code point");

	obj_aux10 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux11,
		Child, Object->SL_Code_Point,
	End;

	GROUP_ROOT_4 = GroupObject,
		Child, Object->BT_Move,
		Child, Object->BT_Show_Pt,
		Child, Object->BT_Show_Seg,
		Child, Object->BT_Edit,
		Child, Object->BT_Create_Pays,
		Child, Object->BT_New_Border,
		Child, Object->BT_New_Point,
		Child, Object->BT_End_Border,
		Child, obj_aux6,
		Child, obj_aux8,
		Child, obj_aux10,
	End;

	Object->WI_Cmd_Panel = WindowObject,
		MUIA_Window_Title, "Cmd Panel",
		MUIA_HelpNode, "WI_Cmd_Panel",
		MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_4,
	End;

	Object->LV_Continent = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Continent = ListviewObject,
		MUIA_HelpNode, "LV_Continent",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Continent,
	End;

	GROUP_ROOT_5 = GroupObject,
		Child, Object->LV_Continent,
	End;

	Object->WI_Sel_Cont = WindowObject,
		MUIA_Window_Title, "Continent",
		MUIA_HelpNode, "WI_Sel_Cont",
		MUIA_Window_ID, MAKE_ID('4', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_5,
	End;

	Object->STR_PA_chargement_fond = String("", 80);

	Object->PA_chargement_fond = PopButton(MUII_PopFile);

	Object->PA_chargement_fond = PopaslObject,
		MUIA_HelpNode, "PA_chargement_fond",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, Object->STR_PA_chargement_fond,
		MUIA_Popstring_Button, Object->PA_chargement_fond,
	End;

	Object->BT_Redraw_Ext_Data = TextObject,
		ButtonFrame,
		MUIA_Weight, 63,
		MUIA_ControlChar, 'r',
		MUIA_Text_Contents, "Background On",
		MUIA_Text_PreParse, "\033c",
		MUIA_Text_HiChar, 'r',
		MUIA_HelpNode, "BT_Redraw_Ext_Data",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

	Object->BT_Effacer_Fond = TextObject,
		ButtonFrame,
		MUIA_Weight, 42,
		MUIA_ControlChar, 'e',
		MUIA_Text_Contents, "Background Off",
		MUIA_Text_PreParse, "\033c",
		MUIA_Text_HiChar, 'e',
		MUIA_HelpNode, "BT_Effacer_Fond",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

	GR_grp_11 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_SameSize, TRUE,
		MUIA_Group_HorizSpacing, TRUE,
		MUIA_Group_VertSpacing, TRUE,
		Child, Object->BT_Redraw_Ext_Data,
		Child, Object->BT_Effacer_Fond,
	End;

	GROUP_ROOT_6 = GroupObject,
		MUIA_Weight, 690,
		Child, Object->PA_chargement_fond,
		Child, GR_grp_11,
	End;

	Object->WI_Chargement = WindowObject,
		MUIA_Window_Title, "Loading",
		MUIA_HelpNode, "WI_Chargement",
		MUIA_Window_ID, MAKE_ID('5', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_6,
	End;

	Object->LV_Elements = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Elements = ListviewObject,
		MUIA_HelpNode, "LV_Elements",
		MUIA_Weight, 966,
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_DoubleClick, TRUE,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Elements,
	End;

	GR_grp_30 = GroupObject,
		Child, Object->LV_Elements,
	End;

	Object->TX_Selected_Element = TextObject,
		MUIA_HelpNode, "TX_Selected_Element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_Selected_Element,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_Pays = TextObject,
		MUIA_HelpNode, "TX_Pays",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_Pays,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_Latitude = TextObject,
		MUIA_HelpNode, "TX_Latitude",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_Latitude,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_Longitude = TextObject,
		MUIA_HelpNode, "TX_Longitude",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_Longitude,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_Caracteristique = TextObject,
		MUIA_HelpNode, "TX_Caracteristique",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_Caracteristique,
		MUIA_Text_SetMin, TRUE,
	End;

	Space_1 = HVSpace;

	Object->BT_Element_Origine = SimpleButton("Source point");

	Object->BT_Element_Destination = SimpleButton("Target point");

	GR_grp_24 = GroupObject,
		MUIA_Weight, 30,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Informations",
		Child, Object->TX_Selected_Element,
		Child, Object->TX_Pays,
		Child, Object->TX_Latitude,
		Child, Object->TX_Longitude,
		Child, Object->TX_Caracteristique,
		Child, Space_1,
		Child, Object->BT_Element_Origine,
		Child, Object->BT_Element_Destination,
	End;

	GROUP_ROOT_10 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, GR_grp_30,
		Child, GR_grp_24,
	End;

	Object->WI_Sel_Elem = WindowObject,
		MUIA_Window_Title, "Element",
		MUIA_HelpNode, "WI_Sel_Elem",
		MUIA_Window_ID, MAKE_ID('6', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_10,
	End;

	Object->RA_Type_Data = RadioObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Type de donnee",
		MUIA_HelpNode, "RA_Type_Data",
		MUIA_Radio_Entries, Object->RA_Type_DataContent,
	End;

	Object->STR_fichier_Donnee = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_fichier_Donnee",
	End;

	Object->IM_Fichier_Donnee = ImageObject,
		MUIA_Image_Spec, 20,
		MUIA_HelpNode, "IM_Fichier_Donnee",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_Frame, MUIV_Frame_ImageButton,
		MUIA_Image_FreeVert, TRUE,
		MUIA_Image_FreeHoriz, TRUE,
		MUIA_FixHeight, 10,
		MUIA_FixWidth, 8,
	End;

	GR_File_Donnee = GroupObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Selection du fichier de donnees",
		MUIA_Group_Horiz, TRUE,
		Child, Object->STR_fichier_Donnee,
		Child, Object->IM_Fichier_Donnee,
	End;

	Object->LV_Data1 = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Data1 = ListviewObject,
		MUIA_HelpNode, "LV_Data1",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Data1,
	End;

	Object->RA_label_4 = RadioObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "radio4_title",
		MUIA_HelpNode, "RA_label_4",
		MUIA_Radio_Entries, Object->RA_label_4Content,
	End;

	Object->LV_Data2 = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Data2 = ListviewObject,
		MUIA_HelpNode, "LV_Data2",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Data2,
	End;

	Object->STR_Coef = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_Coef",
	End;

	obj_aux13 = Label2("Coef Mult:");

	obj_aux12 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux13,
		Child, Object->STR_Coef,
	End;

	GR_Virtuel = GroupObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Virtuel",
		MUIA_Group_Horiz, TRUE,
		Child, Object->LV_Data1,
		Child, Object->RA_label_4,
		Child, Object->LV_Data2,
		Child, obj_aux12,
	End;

	LA_Nom_Titre = Label("Nom de la donnee:");

	LA_Nom_Label = Label("<nom>");

	LA_Type_Titre = Label("Type de la donnee");

	LA_Titre_Label = Label("<titre>");

	Object->BT_Maj = SimpleButton("_Maj");

	Space_2 = HVSpace;

	GR_Infos_Donnees = GroupObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Infos",
		MUIA_Group_Columns, 2,
		MUIA_Group_HorizSpacing, FALSE,
		Child, LA_Nom_Titre,
		Child, LA_Nom_Label,
		Child, LA_Type_Titre,
		Child, LA_Titre_Label,
		Child, Object->BT_Maj,
		Child, Space_2,
	End;

	Object->BT_Ajouter_Donnee = SimpleButton("_Ajouter");

	Object->BT_Effacer = SimpleButton("_Effacer");

	GR_Operations = GroupObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Operations:",
		MUIA_Group_Horiz, TRUE,
		Child, Object->BT_Ajouter_Donnee,
		Child, Object->BT_Effacer,
	End;

	GR_Edit_Donnee = GroupObject,
		Child, Object->RA_Type_Data,
		Child, GR_File_Donnee,
		Child, GR_Virtuel,
		Child, GR_Infos_Donnees,
		Child, GR_Operations,
	End;

	Object->WI_Param_Datas = WindowObject,
		MUIA_Window_Title, "PArametrage/Ajout de donnees ",
		MUIA_HelpNode, "WI_Param_Datas",
		MUIA_Window_ID, MAKE_ID('7', 'W', 'I', 'N'),
		WindowContents, GR_Edit_Donnee,
	End;

	Object->LV_Deselect = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Deselect = ListviewObject,
		MUIA_HelpNode, "LV_Deselect",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Deselect,
	End;

	Object->BT_Vers_Deselect = TextObject,
		ButtonFrame,
		MUIA_Weight, 324,
		MUIA_ControlChar, 'b',
		MUIA_Text_Contents, "<---",
		MUIA_Text_PreParse, "\033c",
		MUIA_Text_HiChar, 'b',
		MUIA_HelpNode, "BT_Vers_Deselect",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

	Object->BT_Vers_Select = TextObject,
		ButtonFrame,
		MUIA_Weight, 327,
		MUIA_ControlChar, 'b',
		MUIA_Text_Contents, "----->",
		MUIA_Text_PreParse, "\033c",
		MUIA_Text_HiChar, 'b',
		MUIA_HelpNode, "BT_Vers_Select",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

	GR_Vertical_Centre = GroupObject,
		MUIA_Group_VertSpacing, FALSE,
		Child, Object->LV_Deselect,
		Child, Object->BT_Vers_Deselect,
		Child, Object->BT_Vers_Select,
	End;

	Object->LV_Select = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Select = ListviewObject,
		MUIA_HelpNode, "LV_Select",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Select,
	End;

	GR_Horizontal_Gauche = GroupObject,
		MUIA_Weight, 1000,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Countries selection",
		MUIA_Group_Horiz, TRUE,
		Child, GR_Vertical_Centre,
		Child, Object->LV_Select,
	End;

	Object->BT_Continent = SimpleButton("_Continent....");

	Object->BT_Petits_Pays = SimpleButton("Small countries");

	Object->BT_Tout_Des = SimpleButton("Deselec_t all");

	Object->BT_Tout_Sel = SimpleButton("Select all");

	GR_Action = GroupObject,
		MUIA_Weight, 1000,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Action",
		MUIA_Group_Columns, 2,
		Child, Object->BT_Continent,
		Child, Object->BT_Petits_Pays,
		Child, Object->BT_Tout_Des,
		Child, Object->BT_Tout_Sel,
	End;

	GR_Vertical_Gauche = GroupObject,
		MUIA_Weight, 347,
		Child, GR_Horizontal_Gauche,
		Child, GR_Action,
	End;

	Object->RA_Type_Graph = RadioObject,
		MUIA_Weight, 0,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Graph type",
		MUIA_HelpNode, "RA_Type_Graph",
		MUIA_Radio_Entries, Object->RA_Type_GraphContent,
	End;

	Object->LV_Datas = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_Datas = ListviewObject,
		MUIA_HelpNode, "LV_Datas",
		MUIA_Weight, 941,
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_Datas,
	End;

	Object->BT_Editer = SimpleButton("_Modify...");

	Object->BT_Supprimer = SimpleButton("Delete");

	Object->BT_Ajouter = SimpleButton("Add");

	Object->BT_Ok_Datas = TextObject,
		ButtonFrame,
		MUIA_Weight, 0,
		MUIA_ControlChar, 'b',
		MUIA_Text_Contents, "OK",
		MUIA_Text_PreParse, "\033c",
		MUIA_Text_HiChar, 'b',
		MUIA_HelpNode, "BT_Ok_Datas",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
	End;

	GR_Vertical_Droite = GroupObject,
		MUIA_Weight, 0,
		Child, Object->RA_Type_Graph,
		Child, Object->LV_Datas,
		Child, Object->BT_Editer,
		Child, Object->BT_Supprimer,
		Child, Object->BT_Ajouter,
		Child, Object->BT_Ok_Datas,
	End;

	GROUP_ROOT_8 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, GR_Vertical_Gauche,
		Child, GR_Vertical_Droite,
	End;

	Object->WI_Datas = WindowObject,
		MUIA_Window_Title, "Data Display Preferences",
		MUIA_HelpNode, "WI_Datas",
		MUIA_Window_ID, MAKE_ID('8', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_8,
	End;

	Object->STR_Trajectoire_Fichier = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_Trajectoire_Fichier",
	End;

	obj_aux15 = Label2("File:");

	obj_aux14 = GroupObject,
		MUIA_Group_Columns, 2,
		Child, obj_aux15,
		Child, Object->STR_Trajectoire_Fichier,
	End;

	Object->IM_Trajectoire_Choix = ImageObject,
		MUIA_Image_Spec, 20,
		MUIA_HelpNode, "IM_Trajectoire_Choix",
		MUIA_InputMode, MUIV_InputMode_RelVerify,
		MUIA_Frame, MUIV_Frame_ImageButton,
		MUIA_Image_FreeVert, TRUE,
		MUIA_Image_FreeHoriz, TRUE,
		MUIA_FixHeight, 10,
		MUIA_FixWidth, 8,
	End;

	GR_grp_28 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, obj_aux14,
		Child, Object->IM_Trajectoire_Choix,
	End;

	Object->BT_Trajectoire_OK = SimpleButton("OK");

	Object->BT_Trajectoire_Cancel = SimpleButton("Cancel");

	GR_grp_29 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, Object->BT_Trajectoire_OK,
		Child, Object->BT_Trajectoire_Cancel,
	End;

	Object->STR_PA_chargement_trajectoire = String("", 80);

	Object->PA_chargement_trajectoire = PopButton(MUII_PopUp);

	Object->PA_chargement_trajectoire = PopaslObject,
		MUIA_HelpNode, "PA_chargement_trajectoire",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, Object->STR_PA_chargement_trajectoire,
		MUIA_Popstring_Button, Object->PA_chargement_trajectoire,
	End;

	GROUP_ROOT_13 = GroupObject,
		MUIA_Frame, MUIV_Frame_Group,
		MUIA_FrameTitle, "Select a Path",
		Child, GR_grp_28,
		Child, GR_grp_29,
		Child, Object->PA_chargement_trajectoire,
	End;

	Object->WI_Trajectoire = WindowObject,
		MUIA_Window_Title, "Path",
		MUIA_HelpNode, "WI_Trajectoire",
		MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_13,
	End;

	Object->CY_organisation = CycleObject,
		MUIA_HelpNode, "CY_organisation",
		MUIA_Cycle_Entries, Object->CY_organisationContent,
	End;

	LA_organisation_officiel = Label("offical name:");

	Object->TX_organisation_officiel = TextObject,
		MUIA_HelpNode, "TX_organisation_officiel",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_organisation_officiel,
		MUIA_Text_SetMin, TRUE,
	End;

	GR_info_organisation = GroupObject,
		MUIA_Weight, 0,
		MUIA_Group_Horiz, TRUE,
		Child, LA_organisation_officiel,
		Child, Object->TX_organisation_officiel,
	End;

	Object->LV_organisation = FloattextObject,
		MUIA_Frame, MUIV_Frame_ReadList,
	End;

	Object->LV_organisation = ListviewObject,
		MUIA_HelpNode, "LV_organisation",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_organisation,
	End;

	GROUP_ROOT_organisation = GroupObject,
		Child, Object->CY_organisation,
		Child, GR_info_organisation,
		Child, Object->LV_organisation,
	End;

	Object->WI_Organisation = WindowObject,
		MUIA_Window_Title, "Info about organizations",
		MUIA_HelpNode, "WI_Organisation",
		MUIA_Window_ID, MAKE_ID('1', '0', 'W', 'I'),
		WindowContents, GROUP_ROOT_organisation,
	End;

	Object->TX_nom_element = TextObject,
		MUIA_HelpNode, "TX_nom_element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_nom_element,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_type_element = TextObject,
		MUIA_HelpNode, "TX_type_element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_type_element,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_pays_element = TextObject,
		MUIA_HelpNode, "TX_pays_element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_pays_element,
		MUIA_Text_SetMin, TRUE,
	End;

	LA_lat = Label("Longitude:");

	Object->TX_longitude_element = TextObject,
		MUIA_HelpNode, "TX_longitude_element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_longitude_element,
		MUIA_Text_SetMin, TRUE,
	End;

	GR_grp_41 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, LA_lat,
		Child, Object->TX_longitude_element,
	End;

	LA_latitude_element = Label("Latitude:");

	Object->TX_latitude_element = TextObject,
		MUIA_HelpNode, "TX_latitude_element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_latitude_element,
		MUIA_Text_SetMin, TRUE,
	End;

	GR_grp_42 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, LA_latitude_element,
		Child, Object->TX_latitude_element,
	End;

	Object->TX_caracteristique_element = TextObject,
		MUIA_HelpNode, "TX_caracteristique_element",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_caracteristique_element,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->BT_voir_donnee_externe = SimpleButton("Watch external associeted data");

	GROUP_ROOT_INFO_ELEM = GroupObject,
		Child, Object->TX_nom_element,
		Child, Object->TX_type_element,
		Child, Object->TX_pays_element,
		Child, GR_grp_41,
		Child, GR_grp_42,
		Child, Object->TX_caracteristique_element,
		Child, Object->BT_voir_donnee_externe,
	End;

	Object->WI_Info_Element = WindowObject,
		MUIA_Window_Title, "Information about Element",
		MUIA_HelpNode, "WI_Info_Element",
		MUIA_Window_ID, MAKE_ID('1', '1', 'W', 'I'),
		WindowContents, GROUP_ROOT_INFO_ELEM,
	End;

	LA_classement = Label("sorting by:");

	Object->TX_classement = TextObject,
		MUIA_HelpNode, "TX_classement",
		MUIA_Background, MUII_BACKGROUND,
		MUIA_Text_Contents, Object->STR_TX_classement,
		MUIA_Text_SetMin, TRUE,
	End;

	GR_grp_45 = GroupObject,
		MUIA_Group_Horiz, TRUE,
		Child, LA_classement,
		Child, Object->TX_classement,
	End;

	Object->LV_classement = ListObject,
		MUIA_Frame, MUIV_Frame_InputList,
	End;

	Object->LV_classement = ListviewObject,
		MUIA_HelpNode, "LV_classement",
		MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
		MUIA_Listview_Input, TRUE,
		MUIA_Listview_List, Object->LV_classement,
	End;

	GROUP_ROOT_17 = GroupObject,
		Child, GR_grp_45,
		Child, Object->LV_classement,
	End;

	Object->WI_Classement = WindowObject,
		MUIA_Window_Title, "Show data by list",
		MUIA_HelpNode, "WI_Classement",
		MUIA_Window_ID, MAKE_ID('1', '2', 'W', 'I'),
		WindowContents, GROUP_ROOT_17,
	End;

	REC_label_0 = RectangleObject,
	End;

	GROUP_ROOT_info = GroupObject,
		Child, REC_label_0,
	End;

	Object->WI_Info = WindowObject,
		MUIA_Window_Title, "info on:",
		MUIA_Window_ID, MAKE_ID('1', '3', 'W', 'I'),
		WindowContents, GROUP_ROOT_info,
	End;

	Object->App = ApplicationObject,
		MUIA_Application_Author, "Thomas Landspurg & Olivier Collard",
		MUIA_Application_Base, "AmiGlobe",
		MUIA_Application_Title, "AmiGlobe",
		MUIA_Application_Version, "$VER: 0.99 (17-11-94)",
		MUIA_Application_Copyright, "Thomas Landspurg & Olivier Collard",
		MUIA_Application_Description, "Geographical atlas",
		SubWindow, Object->WI_Cmd,
		SubWindow, Object->WI_Param,
		SubWindow, Object->WI_Sel_Cty,
		SubWindow, Object->WI_Cmd_Panel,
		SubWindow, Object->WI_Sel_Cont,
		SubWindow, Object->WI_Chargement,
		SubWindow, Object->WI_Sel_Elem,
		SubWindow, Object->WI_Param_Datas,
		SubWindow, Object->WI_Datas,
		SubWindow, Object->WI_Trajectoire,
		SubWindow, Object->WI_Organisation,
		SubWindow, Object->WI_Info_Element,
		SubWindow, Object->WI_Classement,
		SubWindow, Object->WI_Info,
	End;


	if (!Object->App)
	{
		FreeVec(Object);
		return(NULL);
	}

	DoMethod(Object->WI_Cmd,
		MUIM_Window_SetCycleChain, Object->BT_Zoom,
		Object->BT_Zoom_Out,
		Object->BT_Redraw,
		Object->BT_Zoom_Reset,
		Object->BT_Info,
		Object->CY_Proj,
		0
		);

	DoMethod(Object->WI_Param,
		MUIM_Window_SetCycleChain, Object->CH_Lacs,
		Object->CH_Riv,
		Object->CH_LonLat,
		Object->CH_Noms,
		Object->CH_Coord,
		Object->CH_Fill_Pays,
		Object->CH_Elements,
		Object->CH_Trajectoires,
		Object->CH_Distance,
		Object->CH_Scale,
		Object->CH_Area,
		Object->SL_x,
		Object->SL_y,
		Object->SL_z,
		Object->IM_Rectangle,
		0
		);

	DoMethod(Object->WI_Sel_Cty,
		MUIM_Window_SetCycleChain, Object->LV_Pays,
		0
		);

	DoMethod(Object->WI_Cmd_Panel,
		MUIM_Window_SetCycleChain, Object->BT_Move,
		Object->BT_Show_Pt,
		Object->BT_Show_Seg,
		Object->BT_Edit,
		Object->BT_Create_Pays,
		Object->BT_New_Border,
		Object->BT_New_Point,
		Object->BT_End_Border,
		Object->STR_Nom_Border,
		Object->STR_Code,
		Object->SL_Code_Point,
		0
		);

	DoMethod(Object->WI_Sel_Cont,
		MUIM_Window_SetCycleChain, Object->LV_Continent,
		0
		);

	DoMethod(Object->WI_Chargement,
		MUIM_Window_SetCycleChain, Object->PA_chargement_fond,
		Object->BT_Redraw_Ext_Data,
		Object->BT_Effacer_Fond,
		0
		);

	DoMethod(Object->WI_Sel_Elem,
		MUIM_Window_SetCycleChain, Object->LV_Elements,
		Object->BT_Element_Origine,
		Object->BT_Element_Destination,
		0
		);

	DoMethod(Object->WI_Param_Datas,
		MUIM_Window_SetCycleChain, Object->RA_Type_Data,
		Object->STR_fichier_Donnee,
		Object->IM_Fichier_Donnee,
		Object->LV_Data1,
		Object->RA_label_4,
		Object->LV_Data2,
		Object->STR_Coef,
		Object->BT_Maj,
		Object->BT_Ajouter_Donnee,
		Object->BT_Effacer,
		0
		);

	DoMethod(Object->WI_Datas,
		MUIM_Window_SetCycleChain, Object->LV_Deselect,
		Object->BT_Vers_Deselect,
		Object->BT_Vers_Select,
		Object->LV_Select,
		Object->BT_Continent,
		Object->BT_Petits_Pays,
		Object->BT_Tout_Des,
		Object->BT_Tout_Sel,
		Object->RA_Type_Graph,
		Object->LV_Datas,
		Object->BT_Editer,
		Object->BT_Supprimer,
		Object->BT_Ajouter,
		Object->BT_Ok_Datas,
		0
		);

	DoMethod(Object->WI_Trajectoire,
		MUIM_Window_SetCycleChain, Object->STR_Trajectoire_Fichier,
		Object->IM_Trajectoire_Choix,
		Object->BT_Trajectoire_OK,
		Object->BT_Trajectoire_Cancel,
		Object->PA_chargement_trajectoire,
		0
		);

	DoMethod(Object->WI_Organisation,
		MUIM_Window_SetCycleChain, Object->CY_organisation,
		Object->LV_organisation,
		0
		);

	DoMethod(Object->WI_Info_Element,
		MUIM_Window_SetCycleChain, Object->BT_voir_donnee_externe,
		0
		);

	DoMethod(Object->WI_Classement,
		MUIM_Window_SetCycleChain, Object->LV_classement,
		0
		);

	DoMethod(Object->WI_Info,
		MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
		Object->WI_Info,
		3,
		MUIM_Set, MUIA_Window_Open, FALSE
		);

	DoMethod(Object->WI_Info,
		MUIM_Window_SetCycleChain, 0
		);


	return(Object);
}

void DisposeApp(struct ObjApp * Object)
{
	MUI_DisposeObject(Object->App);
	FreeVec(Object);
}
