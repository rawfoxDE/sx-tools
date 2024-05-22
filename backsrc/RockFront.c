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

void SXFunction(int fid,char *dsf, int extra , char *buf, char *exe);
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
     char buf[256],buff[128];
     
     
     PS("\n\rSelect ScreenMode:");   
     PS("\n\r 1 - ANSI");
     PS("\n\r 2 - ASCII");
     PS("\n\r 3 - RIP");
     PS("\n\r 4 - TCP/IP");

     themsg.String[0]=0;
     XIMFunction(6,0,"");   
        
     switch(themsg.String[0])
     {
        case '1':
        {
                sprintf(buf,"%c",'1'); 
                SXFunction(9,0,0,buf,0);
                return;
        }
        case '2':
        {
                sprintf(buf,"%c",'2');
                SXFunction(9,0,0,buf,0);
                return;
        }                
        case '3':
        {
                sprintf(buf,"%c",'3');
                SXFunction(9,0,0,buf,0);
                return;
        }         
        case '4':
        {                   
                PS("\n\rPumping up the TCP-Server, please have patient ...\n\r");                    
                sprintf(buf,"%s","c:execute sx:batch/tcp-prelogin.bat");
                Execute(buf,0,0);
                
                SXFunction(125,"amitcp:bin/login",0,0,0);

                sprintf(buf,"%s","c:execute sx:batch/tcp-postlogin.bat");
                Execute(buf,0,0);
              
                PS("\n\r");
                return;
        }

     }          
     
                
}

void menu(void)
{
        
        int y=0;
        XIMFunction(501,0,0);
        XIMFunction(6,0,0);
        switch(themsg.String[0])
        {
                
                case '4':
                {
                        if(y!=0)
                        {
                              
                        }
                }        
        }                
}                        

void SXFunction(int fid, char *dsf, int extra, char *buf, char *exe)
{
        SXFunc.id = fid;	        // Functions ID
	SXFunc.string = dsf;            // Door/String/File
	SXFunc.extra = extra;           // Extra Number
	SXFunc.low = 0;                 // low axx
	SXFunc.high = 255;              // high axx
	SXFunc.mainarg= buf;            // Mainline Argument
	SXFunc.execarg = exe;           // Execute Argument

   XIMFunction(1506, (long)&SXFunc, 0); // SX_FUNCTION
}
