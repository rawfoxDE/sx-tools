/********************************/
/* AmiGlobe                     */
/* T.Landspurg                  */
/* O. Collard                   */
/********************************/

/* INCLUDES ********************************************************** */

#include <exec/types.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

/* Prototypes */
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>


#include "amiglobe_types.h"
#include "database_types.h"
#include "map_function_protos.h"
#include "read_bin.h"
#include "main_protos.h"

struct Window *OpenWindow();

#define MOVE_PT 0
#define ADD_SEGMENT 1


int commande=MOVE_PT;

extern  PREFERENCE Pref;        /* Préférences              */
extern  int max_country;        /* Nombre max de pays       */

extern  struct Screen *sC;
extern  struct Window *wG;      /* we fetch the RastPort pointer from here */
extern  struct RastPort *rpG;
extern  int signal_wG;

int tab_vecteur_temp[100];
int num_vect=0;

struct Window *edit_wG;
struct Window *inf_wG;
struct RastPort *edit_rpG,*inf_rpG;

extern  BORDER  *tab_vect;
extern  COUNTRY *tab_country;

int signal_edit,signal_edit_wG;

int start_bord=0;
int flg_quit_inf;

void cmd_edit_pays(void);
void cmd_ok(void);
void aff_borders(int,int);
void cmd_add_bord(void);

void cmd_move_pt(void)
{
    printf("cmd move pt!\n");
    commande=MOVE_PT;
}

void cmd_create_pays(void)
{
    printf("Create pays!\n");
    commande=ADD_SEGMENT;
}

void cmd_show_bord(void)
{
    map_show_bords(Pref.country_sel,start_bord,rpG);
}

void cmd_show_seg(void)
{
    map_show_borders(rpG);
}

void cmd_newname(void)
{
    printf("cmd new name!\n");
}

void cmd_del_bord(void)
{
    int i,num_bord;
    int *p_b;
    COUNTRY *p_c;

    p_c = &(tab_country[Pref.country_sel]);
    num_bord = (p_c->p_borders)[start_bord];
    printf ("cmd del bord de %d\n",num_bord);
    if (p_c->nb_borders>1)
    {
        p_c->nb_borders--;
        p_b=p_c->p_borders;
        for (i=start_bord;i<p_c->nb_borders;i++)
        {
            p_b[i]=p_b[i+1];
        }
        aff_borders(Pref.country_sel,start_bord);
        redraw_map(-1,rpG);
    }
}

void cmd_up(void)
{
    if(start_bord>0)start_bord--;
    aff_borders(Pref.country_sel,start_bord);
}

void cmd_down(void)
{
    if(start_bord<(tab_country[Pref.country_sel].nb_borders-1)){
        start_bord++;
        aff_borders(Pref.country_sel,start_bord);
    }
}

void cmd_show_pt(void)
{
    printf("cmd show pt !\n");
    map_show_pt(rpG);
}
/*************************************************************************/

/*************************************************************************/


UBYTE infpinfpnaSIBuff[20];
struct StringInfo infpinfpnaSInfo = {
    infpinfpnaSIBuff,   /* buffer where text will be edited */
    NULL,   /* optional undo buffer */
    0,  /* character position in buffer */
    20, /* maximum number of characters to allow */
    0,  /* first displayed character buffer position */
    0,0,0,0,0,  /* Intuition initialized and maintained variables */
    0,  /* Rastport of gadget */
    0,  /* initial value for integer gadgets */
    NULL    /* alternate keymap (fill in if you set the flag) */
};

SHORT infpBorderVectors1[] = {
    0,0,
    132,0,
    132,9,
    0,9,
    0,0
};
struct Border infpBorder1 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors1, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct IntuiText infpIText1 = {
    1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
    -44,0,  /* XY origin relative to container TopLeft */
    NULL,   /* font pointer or NULL for default */
    "Name:",    /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct Gadget infpna = {
    NULL,   /* next gadget */
    81,18,  /* origin XY of hit box relative to window TopLeft */
    131,8,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    STRGADGET,  /* gadget type flags */
    (APTR)&infpBorder1, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    &infpIText1,    /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    (APTR)&infpinfpnaSInfo, /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

SHORT infpBorderVectors2[] = {
    0,0,
    55,0,
    55,16,
    0,16,
    0,0
};
struct Border infpBorder2 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors2, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct IntuiText infpIText2 = {
    3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
    10,3,   /* XY origin relative to container TopLeft */
    NULL,   /* font pointer or NULL for default */
    "Add",  /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct Gadget infps3 = {
    &infpna,    /* next gadget */
    167,84, /* origin XY of hit box relative to window TopLeft */
    54,15,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    BOOLGADGET, /* gadget type flags */
    (APTR)&infpBorder2, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    &infpIText2,    /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    NULL,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

SHORT infpBorderVectors3[] = {
    0,0,
    55,0,
    55,16,
    0,16,
    0,0
};
struct Border infpBorder3 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors3, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct IntuiText infpIText3 = {
    3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
    11,3,   /* XY origin relative to container TopLeft */
    NULL,   /* font pointer or NULL for default */
    "Del",  /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct Gadget infps2 = {
    &infps3,    /* next gadget */
    167,66, /* origin XY of hit box relative to window TopLeft */
    54,15,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    BOOLGADGET, /* gadget type flags */
    (APTR)&infpBorder3, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    &infpIText3,    /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    NULL,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

SHORT infpBorderVectors4[] = {
    0,0,
    55,0,
    55,16,
    0,16,
    0,0
};
struct Border infpBorder4 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors4, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct IntuiText infpIText4 = {
    3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
    6,3,    /* XY origin relative to container TopLeft */
    NULL,   /* font pointer or NULL for default */
    "Show", /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct Gadget infps1 = {
    &infps2,    /* next gadget */
    167,48, /* origin XY of hit box relative to window TopLeft */
    54,15,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    BOOLGADGET, /* gadget type flags */
    (APTR)&infpBorder4, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    &infpIText4,    /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    NULL,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

UBYTE infpinfpbord_numSIBuff[4];
struct StringInfo infpinfpbord_numSInfo = {
    infpinfpbord_numSIBuff, /* buffer where text will be edited */
    NULL,   /* optional undo buffer */
    0,  /* character position in buffer */
    4,  /* maximum number of characters to allow */
    0,  /* first displayed character buffer position */
    0,0,0,0,0,  /* Intuition initialized and maintained variables */
    0,  /* Rastport of gadget */
    0,  /* initial value for integer gadgets */
    NULL    /* alternate keymap (fill in if you set the flag) */
};

SHORT infpBorderVectors5[] = {
    0,0,
    68,0,
    68,9,
    0,9,
    0,0
};
struct Border infpBorder5 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors5, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct Gadget infpbord_num = {
    &infps1,    /* next gadget */
    158,36, /* origin XY of hit box relative to window TopLeft */
    67,8,   /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    STRGADGET,  /* gadget type flags */
    (APTR)&infpBorder5, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    NULL,   /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    (APTR)&infpinfpbord_numSInfo,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

SHORT infpBorderVectors6[] = {
    0,0,
    33,0,
    33,12,
    0,12,
    0,0
};
struct Border infpBorder6 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors6, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct Gadget infpd2 = {
    &infpbord_num,  /* next gadget */
    128,77, /* origin XY of hit box relative to window TopLeft */
    32,11,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    BOOLGADGET, /* gadget type flags */
    (APTR)&infpBorder6, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    NULL,   /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    NULL,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

SHORT infpBorderVectors7[] = {
    0,0,
    33,0,
    33,12,
    0,12,
    0,0
};
struct Border infpBorder7 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors7, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct Gadget infpu1 = {
    &infpd2,    /* next gadget */
    129,48, /* origin XY of hit box relative to window TopLeft */
    32,11,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    BOOLGADGET, /* gadget type flags */
    (APTR)&infpBorder7, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    NULL,   /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    NULL,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

SHORT infpBorderVectors8[] = {
    0,0,
    77,0,
    77,16,
    0,16,
    0,0
};
struct Border infpBorder8 = {
    -1,-1,  /* XY origin relative to container TopLeft */
    3,0,JAM1,   /* front pen, back pen and drawmode */
    5,  /* number of XY vectors */
    infpBorderVectors8, /* pointer to XY vectors */
    NULL    /* next border in list */
};

struct IntuiText infpIText5 = {
    1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
    20,4,   /* XY origin relative to container TopLeft */
    NULL,   /* font pointer or NULL for default */
    "Quit", /* pointer to text */
    NULL    /* next IntuiText structure */
};

struct Gadget infpok1 = {
    &infpu1,    /* next gadget */
    102,108,    /* origin XY of hit box relative to window TopLeft */
    76,15,  /* hit box width and height */
    NULL,   /* gadget flags */
    RELVERIFY,  /* activation flags */
    BOOLGADGET, /* gadget type flags */
    (APTR)&infpBorder8, /* gadget border or image to be rendered */
    NULL,   /* alternate imagery for selection */
    &infpIText5,    /* first IntuiText structure */
    NULL,   /* gadget mutual-exclude long word */
    NULL,   /* SpecialInfo structure */
    NULL,   /* user-definable data */
    NULL    /* pointer to user-definable data */
};

#define infpGadgetList1 infpok1

struct NewWindow infpNewWindowStructure1 = {
    12,33,  /* window XY origin relative to TopLeft of screen */
    258,128,    /* window width and height */
    0,1,    /* detail and block pens */
    GADGETDOWN+GADGETUP+CLOSEWINDOW,    /* IDCMP flags */
    WINDOWSIZING+WINDOWDRAG+WINDOWDEPTH+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH, /* other window flags */
    &infpok1,   /* first gadget in gadget list */
    NULL,   /* custom CHECKMARK imagery */
    "Info pays",    /* window title */
    NULL,   /* custom screen pointer */
    NULL,   /* custom bitmap */
    5,5,    /* minimum width and height */
    65535,65535,    /* maximum width and height */
    CUSTOMSCREEN    /* destination screen type */
};


void HandleEvent3(APTR object)
{
  if (object == (APTR)&infpok1) { cmd_ok(); return; }
  if (object == (APTR)&infpu1) { cmd_up(); return; }
  if (object == (APTR)&infpd2) { cmd_down(); return; }
  if (object == (APTR)&infps1) { cmd_show_bord(); return; }
  if (object == (APTR)&infps2) { cmd_del_bord(); return; }
  if (object == (APTR)&infps3) { cmd_add_bord(); return; }
  if (object == (APTR)&infpna) { cmd_newname(); return; }
}
#define HANDLEEVENT HandleEvent

/* end of PowerWindows source generation */
/*


#include "win_info_pays.h"

/*************************************************************************/

void cmd_add_bord(void)
{
    int i,num_bord,num_add;
    int *p_b;
    COUNTRY *p_c;

    num_add = atoi(infpinfpbord_numSIBuff);
    printf ("cmd add bord de %d\n",num_add);
    p_c = &(tab_country[Pref.country_sel]);
    num_bord = (p_c->p_borders)[start_bord];

    p_b = p_c->p_borders;
    for (i=p_c->nb_borders;i>=start_bord;i--)
    {
        p_b[i+1]=p_b[i];
    }
    p_b[start_bord] = num_add;
    p_c->nb_borders++;
    aff_borders(Pref.country_sel,start_bord);
    redraw_map(-1,rpG);
}

/*************************************************************************/

void    inf_loop(void)
{
    UWORD code;
    ULONG class;
    APTR object;
    struct IntuiMessage *message;
    int x,y;

    flg_quit_inf=FALSE;
    do
    {
        WaitPort(inf_wG->UserPort);
        while( (message = (struct IntuiMessage *)
                GetMsg(inf_wG->UserPort) ) != NULL)
        {
            code = message->Code;  /* MENUNUM */
            class = message->Class;
            object = message->IAddress;  /* Gadget */
            x= message->MouseX;
            y= message->MouseY;
            ReplyMsg((struct Message *)message);
            if ( class == CLOSEWINDOW ) (flg_quit_inf = TRUE);
            if (( class == GADGETUP ) ||    /* Gagdets */
                ( class == GADGETDOWN ))
            {

                HandleEvent3(object);
            }
        }
    }
      while(flg_quit_inf==FALSE);
}


/*************************************************************************/
void    aff_borders(
    int num,
    int start
    )
    
{
    int i,*p_bord;
    char    buf[100];
    i=0;

    p_bord=tab_country[num].p_borders;
    while ((i<5)&&(i+start)<tab_country[num].nb_borders)
    {
        sprintf(buf,"%04d  ",p_bord[i+start]);
        IPrint(inf_rpG,10,55+10*i,buf,-1);
        i++;
    }
    if (i<5)
    {
        IPrint(inf_rpG,10,55+10*i,"    ",-1);
    }
}

/*************************************************************************/
void init_window(int num)
{
    aff_borders(num,0);
    start_bord=0;
}
/*************************************************************************/

void cmd_edit_pays(void)
{
    printf ("cmd edit pays!\n");
    infpNewWindowStructure1.Screen = sC;
    strcpy (infpinfpnaSIBuff,tab_country[Pref.country_sel].name);
    inf_wG = OpenWindow(&infpNewWindowStructure1);
    if (inf_wG==NULL)
    {
        printf("Erreur ouverture window!\n");
    }
    else
    {
        inf_rpG=inf_wG->RPort;
        init_window(Pref.country_sel);
        inf_loop();
        CloseWindow(inf_wG);
    }
}

void cmd_ok(void)
{
    printf("cmd ok!\n");
    flg_quit_inf=TRUE;
}



/*************************************************************************/

void    edit_loop(  struct IntuiMessage *message)
{
    UWORD code;
    ULONG class;
    APTR object;
    static  int flg_press=FALSE;
    static  VECT    *v,*tv;
    static  int num_pt,num_bord,num_pt_avant,num_pt_apres;
    static  int flg_found;
    static  ULONG   seconds,micros;
    static  ULONG   n_seconds,n_micros,ds,dm;
    static  int old_seg =-1;
    static  int x,y,old_x,old_y,x2,y2;

    code = message->Code;  /* MENUNUM */
    class = message->Class;
    object = message->IAddress;  /* Gadget */
    x= message->MouseX;
    y= message->MouseY;
    seconds=message->Seconds;
    micros =message->Micros;

    switch (class)
    {
        case VANILLAKEY:
        if ((commande==ADD_SEGMENT) && (code==' '))
        {
            char    Buffer[10];
            int i,num;
            int mx,my,MX,MY;
            char    c;

            printf("Fin pays!\n Ce pays contient les vecteurs:\n");

            for (i=0;i<num_vect;i++)
            {
                printf("[%2d]:%04d\n",i,tab_vecteur_temp[i]);
            }
            printf ("Correct?(o/n)\n");
            do
            {
                c =getchar();
                printf("c=%c\n",c);
            }
             while ((c!='o') && (c!='n'));
            if (c=='o')
            {
                printf("Ok....\nQuel est le nom du pays?");
                scanf("%s",Buffer);
                printf("Va pour %s\n",Buffer);

                mx=100000;
                my=100000;
                MX=-100000;
                MY=-100000;

                tab_country[max_country].name=(char *)malloc(strlen(Buffer)+1);
                strcpy(tab_country[max_country].name,Buffer);

                tab_country[max_country].p_borders=(int *)malloc((10+num_vect)*sizeof(int));
                for (i=0;i<num_vect;i++)
                {
                    num=tab_vecteur_temp[i];
                    tab_country[max_country].p_borders[i]=num;
                    tstpt(  tab_vect[i].b_clip.minx,
                        tab_vect[i].b_clip.maxy,
                        &mx,&my,&MX,&MY);
                    tstpt(  tab_vect[i].b_clip.maxx,
                        tab_vect[i].b_clip.maxy,
                        &mx,&my,&MX,&MY);
                }

                tab_country[max_country].nb_borders=num_vect;
                tab_country[max_country].pnb1=0;
                tab_country[max_country].pnb2=0;
                tab_country[max_country].pnb3=0;
                tab_vect[num_bord].b_clip.minx=mx;
                tab_vect[num_bord].b_clip.miny=my;
                tab_vect[num_bord].b_clip.maxx=MX;
                tab_vect[num_bord].b_clip.maxy=MY;
                /* un pays est ajouté */
                max_country++;
            }
            num_vect=0;
        }
        if ( code =='s')
        {
            save_zones("Out_Temp");
        }
        if ( code =='r')
        {
            redraw_map_complete(rpG);
        }
        if ((code>='1')&&(code<='5'))
        {
            map_set_prof(code-'0');
            redraw_map_complete(rpG);
        }
        break;
        case MOUSEBUTTONS:
        if (code==SELECTDOWN)
        {
          printf("Sel\n");
          switch(commande)
          {

            case MOVE_PT:
            flg_press=TRUE;
            flg_found=map_find_pt(rpG,x,y,&v,&num_pt,&num_bord);
            if(flg_found==TRUE)
            {
                tv=tab_vect[num_bord].p_vect;
                if (num_pt==0)
                {
                    num_pt_avant=num_pt;
                    num_pt_apres=1;
                }
                else 
                if(num_pt==tab_vect[num_bord].nb_vect-1)
                {
                    num_pt_avant=num_pt-1;
                    num_pt_apres=num_pt;
                }
                else
                {
                    num_pt_avant=num_pt-1;
                    num_pt_apres=num_pt+1;
                }
                SetDrMd(rpG,COMPLEMENT);
/*
                DrawSeg(rpG,tv,num_pt_avant,num_pt,num_pt_apres);
*/
            }
            break;
            case ADD_SEGMENT:
            printf("Add segment:%d\n",old_seg);
            tab_vecteur_temp[num_vect]=old_seg;
            num_vect++;
            {
                char    Buffer[10];
                int i;

                for(i=0;i<num_vect;i++)
                {
                    sprintf(Buffer,"%04d",tab_vecteur_temp[i]);
                    IPrint(rpG,30,40+8*i,Buffer,-1);
                }
            }

            break;
            default:
            break;
           }
        }
        if (code==SELECTUP)
        {
            printf("Sel up\n");
            flg_press=FALSE;
           switch (commande)
           {
              case MOVE_PT:
            if(flg_found==TRUE)
            {
                flg_found=FALSE;
                redraw_map(-1,rpG);
            }
            break;
             case ADD_SEGMENT:
            old_seg=-1;
            break;
            }
        }
        break;
        case MOUSEMOVE:
        CurrentTime (&n_seconds,&n_micros);
        ds=n_seconds-seconds;
        dm=n_micros -micros;
        if ((ds==0)&&(dm<50000))
        {
           switch (commande)
           {
            case MOVE_PT:
                if ((flg_press==TRUE)&&(flg_found==TRUE))
                {
                    SetDrMd(rpG,COMPLEMENT);
                    DrawSeg(rpG,tv,num_pt_avant,num_pt,num_pt_apres);
                    x2=x;
                    y2=y;
                    conv_inv_xy(&x2,&y2);
                    v->x=x;
                    v->y=y;
                    DrawSeg(rpG,tv,num_pt_avant,num_pt,num_pt_apres);
                }
                break;
             case ADD_SEGMENT:
            {
                int num_seg;

                num_seg=map_find_segment(x,y);
                if(num_seg!=old_seg){
                    if(old_seg!=-1){
                        map_draw_segment(rpG,old_seg,2);
                    }
                    if(num_seg!=-1){
                        char    buffer[10];
                        map_draw_segment(rpG,num_seg,9);
                        sprintf(buffer,"%04d",num_seg);
                        IPrint(rpG,20,20,buffer,-1);
                    }
                    old_seg=num_seg;
                }
            }
            break;
            default:
            break;
           }
           old_x=x;
           old_y=y;
        }
        break;
        }

}

