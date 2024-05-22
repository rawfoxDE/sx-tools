/*

----------------------------------------------------
 Programming doors for System-X using EXEC-MESSAGES
----------------------------------------------------

See the botton of this source!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>

struct SXFuncStruct
{
		UWORD	id;
		char	*string;
		UWORD	extra;
		UWORD	low;
		UWORD	high;
		char	*mainarg;
		char	*execarg;
} SXFunc;

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;

void SXFunction(int fid, int extra , char *buf);
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);

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

			Door();

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

/* ============ PUT YOUR DOOR IN HERE ============== */

/*

This example sends s:startup-sequence to the user (remote login only).

*/

void Door(void)
{
  XIMFunction(5,1,"\n\r[1;33;41m NEW MAIL FOUND !! [0m [37mWould you like to read it now ? [32m([33my[32m/[33mn[32m,[33mcr[32m/[33mq[32m)[37m : ");
     if(themsg.String[0]=='y')
     {
        sprintf(buf,"%u",baseindex);
        SXFunction(73,0,buf); // join the msgbase with new mail
        PS("\n\rjoin base nr.: ");
        PS(buf);
        sprintf(buf,"%u",LRP+1);
        SXFunction(72,0,buf);  // read the new mail
        PS("\n\rread mail nr.: ");
        PS(buf);
                       
        PS("\n\r                   ");
        PutLRP(baseindex,0,msg.number);
        totalnew=totalnew-(LRP+1); //if msg was read new totalnew                              
        if(totalnew<=0) totalnew=0;
     } 
}

void SXFunction(int fid, int extra, char *buf)
{
   SXFunc.id = fid;	
	SXFunc.string = 0;
	SXFunc.extra = extra;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = buf;
	SXFunc.execarg = 0;  

   XIMFunction(1506, (long)&SXFunc, 0); // SX_FUNCTION
}