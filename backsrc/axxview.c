/*

-------------------------------------------------------
  AccessView - shows the current installed AccessLevels
               by [r]FoX                               
-------------------------------------------------------

See the botton of this source!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>

static char __version[] ="\0$VER: AxxView V1.01 bY [r]FoX at ("__DATE__")";

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;

struct AccessStruct Axx;
     
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void AxxView(void);

BOOL sx;


int main(int argc, char *argv[])
{
	char portname[16];

	if(argv[1][0]==0)
	{
		PutStr("This program requires System-X BBS Software\n");
	} else {
		sprintf(portname, "AEDoorPort%s", argv[1]);
		bbsport = FindPort(portname);
		if(bbsport)
		{
			XIMFunction(1, 0, 0); 	/* function 1 = register */

			/* find out if we are under SYSTEM-X or AmiExpress */

			if(strcmp(themsg.String,"SX")==0) sx=TRUE; else sx=FALSE;

			AxxView();

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void PS(char * str)
{
	if(sx) XIMFunction(1500, (long)str, 0); else XIMFunction(4, 0, str);
}

void XIMFunction(int func, long data, char * str)
{
	struct MsgPort *replyport;

	replyport = CreateMsgPort();
	if(replyport)
	{
		themsg.Msg.mn_Length	= sizeof(struct JHMessage);
		themsg.Msg.mn_ReplyPort	= replyport;
		themsg.Data 		= data;
		themsg.Command 		= func;
		if(str && str[0]!=0) strcpy(themsg.String, str);
		PutMsg(bbsport, (struct Message *)&themsg);
		WaitPort(replyport);
		DeleteMsgPort(replyport);
	}
}

void AxxView(void)
{ 
        char access[256];
       
        BPTR fhax;
        fhax = Open("SX:Prefs/Access.DAT",MODE_OLDFILE);
        Seek(fhax,0,OFFSET_BEGINNING);        
        if(fhax)
        {                        
                // some gfx here ...
                PS("\n\r                            [34m.--[36m[ [37maXXvIEw[36m ][34m--.");                      
                PS("\n\r    [34m.-----------------------'     [37mv1.o1     [34m`-------------------------.");    
                PS("\n\r    [34m|[0;36;44m   aXX    aXXnAME    tIMElIMIT  fILEs[33m/[36mbYTEsfREe  fILE[33m&[36mbYTErATIO  [0m[34m|");
                         
                while(Read(fhax,&Axx,sizeof(struct AccessStruct)))
                {
                        
                        sprintf(access,"\n\r    [34m|  [32m[[31m%3ld[32m]  [33m%-12s [33m%4ld [32mmin.   [33m%3ld [32m/ [33m%-9ld    [33m1[32m:[33m%ld [32m/ [33m1[32m:[33m%ld   [34m|[0m",Axx.access, Axx.name, Axx.Time_Limit/60, Axx.freefiles, Axx.freebytes, Axx.fileratio, Axx.byteratio);
                        PS(access);
                }
                PS("\n\r    [34m|[36;44m              gREETz tO zED aND dIGITAL^cORRUPTIOn               [0m[34m|");
                PS("\n\r    `----------------------.                 .------------------------'"); 
                PS("\n\r                           [34m`--[36m[[37m bY [r]FoX[36m ][34m--'"); 
        }                                        
        Close(fhax);

        PS("\n\r\n\r");
        
}
