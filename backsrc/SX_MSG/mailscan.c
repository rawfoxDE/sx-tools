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
void mailscan(int scanflag);  
UWORD GetLRP(UWORD basenum, WORD which);
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

                        XIMFunction(5,1,"0 - Scan NEW Msg   1 - Scan UNREADED Msg : ");
                        if(themsg.String[0]=='0') mailscan(0); 
                        else mailscan(1);
                    
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


void mailscan(int scanflag)
{
        char buf[512],buffer[256],pathbuf[256],oldbase[256],lrp_flag[8],newansi[8];
        char confname[45],oldconfname[45];                           
	int baseindex=1,totalmsgs=0,totalnew=0;         // buf         - any buffer 512 Bytes
        UWORD LRP;                                      // buffer      - any buffer 256 Bytes
        BPTR fhbi,fh,fhc;                               // pathbuf     - buffer for Confpath
    // switch the gfx                                   // oldbase     - needed to stop reading msgbases in conf   
  /*      if(scanflag==0) ms_header();                    // lrp_flag    - shows how many new msgs as num.
        else nm_header();                               // newansi     - colored background for bases with new msgs                      
  */      sprintf(oldbase,"EMPTY");                       // confname    - current conferencename
        sprintf(oldconfname,"EMPTY");                   // oldconfname - needed to srop reading the confs
                                                        // baseindex   - current msgbase in actual conf
        fhc = Open("sx:prefs/confs.dat",MODE_OLDFILE);  // LRP         - Last Read Pointer
        Seek(fhc,0,OFFSET_BEGINNING);                   // fhbi        - filehandle baseindex
        while(fhc)                                      // fh          - filehandle msgbases
 	{                                               // fhc         - filehandle conferences
                Seek(fhc,0,OFFSET_CURRENT);
                Read(fhc,&Conf,sizeof(struct ConfStruct));
                strcpy(confname,Conf.name);
                                
                if(confname[0]==oldconfname[0]&&confname[5]==oldconfname[5]&&confname[10]==oldconfname[10])
                {
                        PS("\n\r[37mnothing more to Scan...\n\r");
                        sprintf(buf,"\n\rTotal Messages : %ld  ",totalmsgs);
                        PS(buf);
                        Close(fhc);
                        return;
                } 
                                                                            
                sprintf(buffer,"\n\r\033[31m%-18s ",Conf.name);
                PS(buffer);
                                              
                sprintf(buf,"%sMsgBases.DAT", Conf.path);
                fhbi = Open(buf, MODE_OLDFILE);              
                                
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
                                                        
                                sprintf(pathbuf,"%sMessages/Base%ld/Messages.DAT",Conf.path,baseindex); 
                                fh = Open(pathbuf, MODE_OLDFILE);
                                                                                              
                                if(fh) // check for maxmsg in msgbases
                                {      
                                        Seek(fh,sizeof(struct MsgStruct) * -1, OFFSET_END);
                                        Read(fh, &msg, sizeof(struct MsgStruct));
                                        Close(fh);                                                            
                                }
                                sprintf(maxmsg,"%ld",msg.number);
                                totalmsgs=(totalmsgs + atoi(maxmsg));
                                
                                sprintf(buf,"%u",msg.number-GetLRP(baseindex,scanflag));   // L.R.P.                                    
                              
                                
                                if(buf[0]=='0') 
                                {
                                        sprintf(lrp_flag,"[37m - "); 
                                        sprintf(newansi,"");

                                } else 
                                {
                                        
                                        sprintf(lrp_flag,"[31m%3s",buf);
                                        sprintf(newansi,"[0;44m");
                                 }        

                                // print the complete MsgBase information string
                                sprintf(buf,"%s \033[36m%-22s\033[32m[\033[\033[32m%s\033[32m] ",newansi,name,lrp_flag);
                        
                                if(oldbase[10]==name[10]) 
                                {                                           
                                        baseindex=1;                                                                       
                                        Close(fhbi);
                                        break;               
                                }                       
                                PS(buf);
                                PS("[0m");
                      
                                if(scanflag==0)
                                {    
                                        LRP = GetLRP(baseindex,scanflag);
                                        if(LRP!=msg.number)                 // new mail found !
                                        {
                                                XIMFunction(5,1,"\n\r[1;33mNEW MAIL FOUND !  [37mWould you like to read it now ? (y/n) : ");
                                                if(themsg.String[0]=='y')
                                                {
                                                        sprintf(buf,"%ld",baseindex);
                                                        SX_Function(73,buf); // join the msgbase with new mail
                                               
                                                        sprintf(buf,"%ld",LRP+1);
                                                        SX_Function(72,buf);  // read the new mail
                                                        PS("\n\r                   ");

                                                } else 
                                                {
                                                        if(q==2)
                                                        {
                                                                PS("\r[1A[K[1A");  // clear the questionstring on screen
                                                        } else PS("\r[1A[K[1A[48C"); // but keep the gfx
                                                
                                                }                                                        
                                        }    
                                }
                              
                                q++;
                                if(q==2)   // if 2 bases are typed...
                                {
                                        PS("\n\r                   ");
                                        q=0;
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
