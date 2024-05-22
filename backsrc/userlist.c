/*

-------------------------------------------------------
   UserList  - a UsrsLister for SystemX                
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

//static char __version[] ="\0$VER: Userlist V1.0 bY [r]FoX at ("__DATE__")";

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;
     
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void UserList(void);

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

			UserList();

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

void UserList(void)
{
     struct UserData *You;
	struct UserData User;
	struct UserIndexStruct Index;
                
	BPTR fhi,fhu = Open("SX:User.Data", MODE_OLDFILE);
	
     BOOL listfin = FALSE;
	
     UWORD lco = 1;
	
     char buf[128],tbuf[128], newflag[8];
     int line,usernr=0;
                
	sprintf(newflag,"      ");
	if(fhu)
	{
                
		fhi = Open("SX:User.Index", MODE_OLDFILE);
		if(fhi)
		{
               XIMFunction(1501,0,0);
               You = (struct UserData *) themsg.Data;
               sprintf(buf,"%u",You->LineLength);
               line = atoi(buf);
         
			while(!listfin)
			{                
                                                                                                
                    if(!(FRead(fhu, &User, sizeof(struct UserData), 1)))
				{
                         PS("[1B");
					listfin = TRUE;
				} else 
                                {
					if(!(FRead(fhi, &Index, sizeof(struct UserIndexStruct), 1)))
					{
						listfin = TRUE;
					} else 
                         {                                          
                                  usernr++;                          
						    if(!(User.Flags&(1<<6)))  // Deleted ?
						    {
  sprintf(tbuf,"\n\r[%4ld] %12s  %3ld  %26s  %4ld" , usernr, Index.handle, User.Sec_Status, User.Location, User.Times_Called);
						    } 
						
                                                    sprintf(newflag,"      ");
	                                            lco++;
							
                                                    if(lco >= line)
						    {
                    					       XIMFunction(5,1,"\n\rWhat now?");
								if(themsg.String[0]=='q') break;
                                                        lco=1;
                                                    }
						    if(!listfin) PS(tbuf); 
                                                   					
					}
				}
			}
               Close(fhi);
          }
		Close(fhu);
	}
}
