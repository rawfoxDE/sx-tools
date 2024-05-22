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
struct ConfStruct Conf;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
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

struct JHMessage themsg;
struct UserData * User;


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void set_unread(void);  
UWORD GetLRP(UWORD basenum, WORD which);
void PutLRP(UWORD basenum, WORD which, UWORD LRP);
void SX_Function(int fid, char *buf);

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

                        set_unread();
                    
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


void set_unread(void)
{
        char buf[512],pathbuf[256],oldbase[256];
        char confname[45],oldconfname[45];                           
	int baseindex=1,totalmsgs=0;       
                                           
        BPTR fhbi,fh,fhc;                          
                          
        sprintf(oldbase,"EMPTY");                  
        sprintf(oldconfname,"EMPTY");              
                                     
        fhc = Open("sx:prefs/confs.dat",MODE_OLDFILE);  
        Seek(fhc,0,OFFSET_BEGINNING);                   
        while(fhc)               
 	     {                                             
                Seek(fhc,0,OFFSET_CURRENT);
                Read(fhc,&Conf,sizeof(struct ConfStruct));
                strcpy(confname,Conf.name);
                                
                if(strcmpi(confname,oldconfname)==NULL)
                {
                        PS("\n\r[37mAll Messages set to unread now !\n\r");
                        sprintf(buf,"\n\rTotal Messages : %ld  ",totalmsgs);
                        PS(buf);
                        Close(fhc);
                        return;
                } 
                
                sprintf(buf,"%sMsgBases.DAT", Conf.path);
                fhbi = Open(buf, MODE_OLDFILE);
                                  
                Seek(fhbi,0,OFFSET_BEGINNING);
                if(fhbi)            // double listing mit `q` - wenn 2 bases getyped dann return next line            
                {                                  
                        char name[64];
                        Seek(fhbi,0,OFFSET_BEGINNING);
                        while(fhbi)
                        { 
                                Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                                sprintf(name,"%-10s",base.name);
                                                        
                                sprintf(pathbuf,"%sMessages/Base%ld/Messages.DAT",Conf.path,baseindex); 
                                fh = Open(pathbuf, MODE_OLDFILE);
                                                                                              
                                if(fh) // check for maxmsg in msgbases
                                {      
                                        Seek(fh,sizeof(struct MsgStruct) * -1, OFFSET_END);
                                        Read(fh, &msg, sizeof(struct MsgStruct));
                                        Close(fh);                                                            
                                }

                                PutLRP(baseindex,1,msg.number);                                  
                
                                if(strcmpi(oldbase,name)==NULL) 
                                {                                           
                                        baseindex=1;                                                                       
                                        Close(fhbi);
                                        break;               
                                }                       
                                                              
                                strcpy(oldbase,name);  // aktuelle msgbase in oldbase speichern
                                baseindex++;          
                                if(baseindex>=256) break;							
                       
                        }
                strcpy(oldconfname,confname);
                
                }
        }
        Close(fhc);
  }

UWORD GetLRP(UWORD basenum, WORD which)
{
	BPTR fhl;
	UWORD LRP;
	void *freemem;
        char buf[256];
        
        XIMFunction(1501,0,0);
        User = (struct UserData *) themsg.Data;

        
        sprintf(buf, "%sMessages/Base%ld/LRP.DAT",Conf.path, basenum);
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
void PutLRP(UWORD basenum, WORD which, UWORD LRP)
{
        char buf[256];
	BPTR fh;

        XIMFunction(1501,0,0);
        User = (struct UserData *) themsg.Data; 

        
	sprintf(buf, "%sMessages/Base%ld/LRP.DAT", Conf.path, basenum);
	fh = Open(buf, MODE_OLDFILE);
	if(fh)
	{
		if(which==0) Seek(fh, (User->Slot_Number-1)*4, OFFSET_BEGINING);
		else Seek(fh, ((User->Slot_Number-1)*4)+2, OFFSET_BEGINING);

		Write(fh, &LRP, 2);
		Close(fh);
	}
}
void SX_Function(int fid, char *buf)
{
   SXFunc.id = fid;	// SX-Function 74: Next Msg Base
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = buf;
	SXFunc.execarg = 0;  

   XIMFunction(1506, (long)&SXFunc, 0); // SX_FUNCTION
}
