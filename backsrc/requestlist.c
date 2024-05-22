/*

hi peter, hgere i try to do i/o with sequential files...

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <fcntl.h>          // i read i must include fcntl.h ...must i (i.e. for Open ) ?
#include <sxstructs.h>

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
void Req(void);


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

			Req();

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void PS(char * str)
{
	XIMFunction(1500, (long)str, 0);  /* XIM Function to send some text to SX */
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

void Req(void)
{
       	BPTR fh;

	char addline[40],newline[40],charbuf[1],readme[7];
        long lenght;
	fh = Open("doors:areq/FTPGetList", MODE_OLDFILE);
        sprintf(readme,".readme");

reqfile:                
        XIMFunction(5,40,"\r[32mEnter Path/Name you like to get (V,CR): [31m");
        sprintf(newline,"%s\n",themsg.String);
        
        lenght = strlen(themsg.String)-4;                
        themsg.String[lenght]=0;
        sprintf(addline,"%s%s\n",themsg.String,readme);

        if(themsg.String[0]==0) 
        {                       
                 return;
        }        
        else 
        {
                if(themsg.String[0]=='v') goto listview;
        again: 
                if(fh)
                {
                        Seek(fh,0,OFFSET_END);
                        Write(fh, newline, strlen(newline)); // write the filepath
                        Write(fh, addline, strlen(addline));
                        Close(fh);                  
                        return;
                } else 
                {
                        fh = Open("doors:areq/FTPGetList", MODE_NEWFILE);
                        goto again;
                }     
                
        }
        
        
        
        
listview:              
        PS("\n\r[32mList of AminetRequests for the next Netcall :");
        PS("\n\r---------------------------------------------[36m\n\r");
jumpin:  
        Seek(fh,0,OFFSET_BEGINNING);
	if(fh)
	{
		while(Read(fh, charbuf, sizeof(charbuf)))
		{
                       if(charbuf[0]=='\n') PS("\r"); 	                      
                       PS(charbuf);
                }
		
              		Close(fh);
	} else 
        {
                fh = Open("doors:areq/FTPGetList", MODE_NEWFILE);
                goto jumpin;
        }        

}

