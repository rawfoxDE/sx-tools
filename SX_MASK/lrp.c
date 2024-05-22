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
#include <sx/msgstruct.h>

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;
struct MsgBaseStruct base;
struct MsgStruct msg;

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

void Door(void)
{
        //char buf[256];
       	PS("Hello! Welcome to a test door\r\n\r\n");
	        
        PS("\r\n\r\nExiting...\r\n\r\n");
        
/* get the L.ast R.ead P.ointer and jump to that position */

	lrp = GetLRP(var.thisconf, var.thismsgarea, 0);
	Seek(fh, lrp*sizeof(struct MsgStruct), OFFSET_BEGINING);
	Read(fh, &Msg, sizeof(struct MsgStruct));

/* GetLRP(....)

confnum = conference number
basenum = msg base number
which   = which LRP  0 = normal one
		     1 = menu one
*/

UWORD GetLRP(UWORD confnum, UWORD basenum, WORD which)
{
	BPTR fh;
	UWORD LRP;
	void *freemem;

	mysprintf(buf, "%sMessages/Base%ld/LRP.DAT", Structs->Conf[confnum]->path, basenum);
	fh = Open(buf, MODE_OLDFILE);
	if(!fh)
	{
		fh = Open(buf, MODE_NEWFILE);
		if(fh)
		{
			freemem = AllocMem(8192, MEMF_PUBLIC|MEMF_CLEAR);
			if(!freemem) { Close(fh); return(1); }
			Write(fh, freemem, 8192);
			FreeMem(freemem, 8192);
		}
		Close(fh);
		if(which) return(1); else return(0);
	}
	if(which==0) Seek(fh, (Structs->User.Slot_Number-1)*4, OFFSET_BEGINING);
	else Seek(fh, ((Structs->User.Slot_Number-1)*4)+2, OFFSET_BEGINING);

	Read(fh, &LRP, 2);
	Close(fh);
	if(LRP==0 && which) LRP=1;
	return(LRP);
}

void PutLRP(UWORD confnum, UWORD basenum, WORD which, UWORD LRP)
{
	BPTR fh;

	mysprintf(buf, "%sMessages/Base%ld/LRP.DAT", Structs->Conf[confnum]->path, basenum);
	fh = Open(buf, MODE_OLDFILE);
	if(fh)
	{
		if(which==0)
			Seek(fh, (Structs->User.Slot_Number-1)*4, OFFSET_BEGINING);
		else
			Seek(fh, ((Structs->User.Slot_Number-1)*4)+2, OFFSET_BEGINING);

		Write(fh, &LRP, 2);
		Close(fh);
	}
}
