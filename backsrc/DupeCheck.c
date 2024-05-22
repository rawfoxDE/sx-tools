/*

System-X DupeCheck Door (designed for the Conference Based Version!)


*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include "sxstructs.h"
static char __version[] ="\0$VER: DupeCheck V1.0 bY [r]FoX at ("__DATE__")";
struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
} themsg;


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void FileCheck(char *fname);
void gfx(void);
void chome(void);
void ausruf_gr(void);
void ausruf_rt(void);


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

			FileCheck(argv[2]);

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void PS(char * str)
{
	XIMFunction(1500, (long)str, 0);
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

void FileCheck(char *fname)
{
	char buf[256];
	BPTR lck, fh;

	/* == FIRST, LETS CHECK IF THE FILE EXISTS == */

	lck = Lock(fname, SHARED_LOCK);
	if(!lck) return;	/* == IF IT DOESN'T, EXIT QUIETLY == */
	UnLock(lck);



	/* == PRINT SOME CREDITS == */
        PS("[2J");
        gfx();
        chome();
	/* == PRINT THE FILE INFORMATION */

	sprintf(buf, "\033[8;20H\033[37m[1m[4m          %-26s[0m", fname);
	PS(buf);
        chome();


	/* == LETS DO A SIMPLE CONFERENCE DUPE CHECK == */

	fh = Open("SX:Prefs/Confs.DAT", MODE_OLDFILE);

	if(fh)
	{
		struct ConfStruct Conf;
                
		while(1)
		{
                        if(Read(fh, &Conf, sizeof(struct ConfStruct)))
			{

				/* == LET'S CHECK THIS CONFERENCE == */
                                                               
			        sprintf(buf,"\033[13;24H\033[1m[4;37;44m\033[33m   %-25s[0m", Conf.name);
				PS(buf);
                                chome();
                                
				if(Conf.filepath[0] == 0) lck = 0;
				else 
                                {
					sprintf(buf, "%s%s", Conf.filepath, fname);
					lck = Lock(buf, SHARED_LOCK);
				}
				if(lck)
				{
					UnLock(lck);
/* == FILE IS DUP!! == */		PS("\033[18;18H\033[1;31;43mDuplicate Found! File has been deleted![0m");
                                        PS("[6;12H"); 
                                        ausruf_rt();
                                        PS("[6;61H");
                                        ausruf_rt();
                                        chome();
                                        Delay(25);
                                        DeleteFile(fname);
					Close(fh);
					return;
				} 
/* == FILE IS OK! == */			
			} else
				break;	/* == REECHED END OF FILE == */

		}

		Close(fh);

	}
        PS("\033[18;18H\033[0;35;43mFile is OK ! Pumped to the Conference :)[0m");
        PS("[6;12H"); 
        ausruf_gr();
        PS("[6;61H");
        ausruf_gr();
        chome();
}

void gfx(void)
{       
PS("          [31m______________________________________________________[0m\n\r");
PS("          [4m[31m[43m       [42m  DupeCheck V1.0 by [r]FoX for SystemX  [43m       [30m[40m[0m\n\r");
PS("[0m\n\r");
PS("            [31m-[35m*[31m-[37m¡[34m÷[37m![31m-[35m*[31m-[37m<>[36m<[37m-[36m>[34m÷[36m<[37m-[36m>[37m<>[31m-[35m*[31m-[37m<[36m::[37m>[31m-[35m*[31m-[37m<>[36m<[37m-[36m>[34m÷[36m<[37m-[36m>[37m<>[31m-[35m*[31m-[37m![34m÷[37m¡[31m-[35m*[31m-[0m\n\r");
PS("                [33m__________________________________________[0m\n\r");
PS("                [41m [37mFILENAME                        [30mFILENAME [0m\n\r");
PS("[40m                [33m¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯[0m\n\r");
PS("[0m\n\r");
PS("[24m[40m              [36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[0m\n\r");
PS("                [33m__________________________________________[0m\n\r");
PS("                [41m [37mCONFERENCE                    [30mCONFERENCE [0m\n\r");
PS("[40m                [33m¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯[0m\n\r");
PS("[0m\n\r");
PS("[24m[40m              [36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[34m__[36m_[0m\n\r");
PS("                [33m__________________________________________[0m\n\r");
PS("                [41m [37mFILESTATUS                    [30mFILESTATUS [0m\n\r");
PS("[40m                [33m¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯[0m\n\r");
PS("[0m\n\r");
PS("[24m[40m [0m\n\r");
PS("            [31m-[35m*[31m-[37m¡[34m÷[37m![31m-[35m*[31m-[37m<>[36m<[37m-[36m>[34m÷[36m<[37m-[36m>[37m<>[31m-[35m*[31m-[37m<[36m::[37m>[31m-[35m*[31m-[37m<>[36m<[37m-[36m>[34m÷[36m<[37m-[36m>[37m<>[31m-[35m*[31m-[37m![34m÷[37m¡[31m-[35m*[31m-[0m\n\r");
PS("          [31m______________________________________________________[0m\n\r");
PS("          [4m[31m[43m       [42m -[ Dedicated to dIGITAL^.cURRUPTIOn ]- [43m       [30m[40m[0m\n\r");


}

void chome(void)
{
        PS("\033[25;0H");
}

void ausruf_gr(void)
{
PS("[32m/##[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("##/[3D[1B");
PS("[1B[1B");
PS("/##[3D[1B");
PS("###[3D[1B");
PS("##/[3D[1B");
PS("[0m");
}
void ausruf_rt(void)
{
PS("[31m/##[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("###[3D[1B");
PS("##/[3D[1B");
PS("[1B[1B");
PS("/##[3D[1B");
PS("###[3D[1B");
PS("##/[3D[1B");
PS("[0m");
}

