/*

----------------------------------------------------
SX_MSG - a MsgBase Reader done by [r]fOx            
----------------------------------------------------

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>   // z.b. fuer `isalnum(x)`
#include <time.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>
#include <sx/msgstruct.h>
#include <exec/memory.h>

struct MsgPort * bbsport;
struct MsgStruct msg;
struct MsgBaseStruct base;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;
struct UserData * User;


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);                        //- the mainloop
void checkbases(void);                  //- chaeck the installed msgbases
UWORD GetLRP(UWORD basenum, WORD which);

BOOL sx;

int main(register int argc, register char *argv[])
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

			checkbases();
                    
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


void checkbases(void)
{        
        char buf[512],buffer[256],pathbuf[256],oldbase[256],lrp_flag[8],newansi[8];
        int baseindex=1;        
        BPTR fhbi,fh;
                
        sprintf(oldbase,"EMPTY");
        
        // ConferenceReader, in witch the MessageBases are
        //PS("\n\r\033[33m===============================================================================\n\r");
        PS("\n\r\n\r\033[36m      Message Bases in ");                                                
        XIMFunction(126,1,0);                    // BB_CONFNAME
        sprintf(buffer,"\033[35m%-12s",themsg.String);
        PS(buffer);
        PS("\033[36m     Conference Path : ");
        XIMFunction(127,1,0);                     // BB_CONFLocal
        sprintf(buf,"\033[33m%s",themsg.String);
        PS(buf);
        PS("\n\r\033[33m---------------------- ----------- -------------- ------------------- ---------");
           
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhbi = Open(buf, MODE_OLDFILE);   
              
        PS("\n\r");
                                
        Seek(fhbi,0,OFFSET_BEGINNING);
        if(fhbi)            // double listing mit `q` - wenn 2 bases getyped dann return next line            
        {                                  
                int q=0;
                
                char name[64],maxmsg[8];
                Seek(fhbi,0,OFFSET_BEGINNING);
                
                                               
                while(fhbi)
                { 
                        Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                        sprintf(name,"%-10s",base.name);
                        
                        XIMFunction(127,1,0);              //       BB_CONFLOCAL
                        sprintf(pathbuf,"%sMessages/Base%ld/Messages.DAT",themsg.String,baseindex); 
                        fh = Open(pathbuf, MODE_OLDFILE);
                                                                                              
                        if(fh) // check for maxmsg in msgbases
                        {      
                                Seek(fh,sizeof(struct MsgStruct) * -1, OFFSET_END);
                                Read(fh, &msg, sizeof(struct MsgStruct));
                                Close(fh);                                                            
                        }
                        sprintf(maxmsg,"%ld",msg.number);
                        sprintf(buf,"%u",msg.number-GetLRP(baseindex,0));   // L.R.P.
                        if(buf[0]=='0') 
                        {
                                sprintf(lrp_flag,"[37m - "); 
                                sprintf(newansi,"");

                        } else 
                        {
                                sprintf(lrp_flag,"[33m%3s",buf);
                                sprintf(newansi,"[0;44m");
                        }        

                        // print the complete MsgBase information string
sprintf(buf,"%s\033[32m(\033[33m%3ld\033[32m) - \033[36m%-19s\033[32m[\033[31m%4s\033[32m/%s\033[32m]",newansi,baseindex,name,maxmsg,lrp_flag);
                        
                        if(oldbase[10]==name[10]) 
                        {     
                                PS("\n\r\033[33m----- -------  ---------------------- --------------------------- -------------\n\r");
                                PS("\n\r\033[0m No more MsgBases in this Conference !\n\r");
                                                                         
                                Close(fhbi);
                                return;               
                        }                       
                        PS(buf);
                        PS("[0m");
                        q++;
                        if(q==2)
                        {
                                PS("\n\r");
                                q=0;
                        } else PS("[33m  |  ");       
                        strcpy(oldbase,name);  // aktuelle msgbase in oldbase speichern
                        baseindex++;          
                        if(baseindex>=256) break;
                       
                }
        } 
}

UWORD GetLRP(UWORD basenum, WORD which)
{
	BPTR fhl;
	UWORD LRP;
	void *freemem;
        char buf[256];
        
        XIMFunction(1501,0,0);
        User = (struct UserData *) themsg.Data;

        XIMFunction(127,1,0);
        sprintf(buf, "%sMessages/Base%ld/LRP.DAT",themsg.String, basenum);
	fhl = Open(buf, MODE_OLDFILE);
	if(!fhl)
	{
		fhl = Open(buf, MODE_NEWFILE);
		if(fhl)
		{
			freemem = AllocMem(8192, MEMF_PUBLIC|MEMF_CLEAR);
			if(!freemem) { Close(fhl); return(1); }
			Write(fhl, freemem, 8192);
			FreeMem(freemem, 8192);
		}
		Close(fhl);
		if(which) return(1); else return(0);
	}
	if(which==0) Seek(fhl, (User->Slot_Number-1)*4, OFFSET_BEGINING);
	else Seek(fhl, ((User->Slot_Number-1)*4)+2, OFFSET_BEGINING);

	Read(fhl, &LRP, 2);
	Close(fhl);
	if(LRP==0 && which) LRP=1;
	return(LRP);
}

