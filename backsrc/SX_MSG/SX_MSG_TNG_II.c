/*

------------------------------------------------------------------------
SX_MSG The next Generation - a MsgBase Reader done by [r]fOx            
------------------------------------------------------------------------

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>   // z.b. fuer `isalnum(x)`
#include <time.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/memory.h>
#include <sxstructs.h>
#include <sx/msgstruct.h>

static char __version[] ="\0$VER: SX_MSG V1.1 bY [r]FoX at ("__DATE__")"; 
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

struct UserData * User;
struct UserIndexStruct *Uid;
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

void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);                        //- the mainloop
void checkbases(void);         //- schaeck the installed msgbases
void SXFunction(int fid, char *buf);
void readmsgs(int baseindex, int listend, int call);                //- read the msg-topic list
void basereset(void);                   //- while leaving door() reset current msgbase to 1
void help(void);                        //- show the helpmenu
void myctime(long secs, char *outbuf);  //- convert the time from LONG to CHAR 
UWORD GetLRP(UWORD basenum, WORD which);//- get the L.ast R.ead P.ointer
void infos(void);

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

			Door();
                        basereset();  // reset the actual msg-base to base1
                        
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
        int baseindex,listend, call=1;
        int maxbase;
        char msgnum[8],selectedbase[8], buf[64],oldbase[64],basnam[64];
        
        BPTR fhbi;
        sprintf(oldbase,"EMPTY");
        maxbase=0;
        // check for maxbase now. If ya scroll the bases over the existing indexnumber        
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhbi = Open(buf, MODE_OLDFILE);
        if(fhbi)
        {
                Seek(fhbi,0,OFFSET_BEGINNING);
                while(fhbi)
                { 
                        Seek(fhbi,0,OFFSET_CURRENT); 
                        Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                        
                        sprintf(basnam,"%-10s",base.name);
                                                if(oldbase[10] == basnam[10]) 
                        {
                                break;
                        }
                        strcpy(oldbase,basnam);  // aktuelle msgbase in oldbase speichern
                        maxbase++; 
                                
                        if(maxbase==500) break;
                          
                }
                Close(fhbi); 
        }       
                
        checkbases();
         
       //test only  
       // sprintf(buf,"\n\rMAXBASE: %ld \n\r",maxbase); 
       // PS(buf); 
        baseindex=1;
 auswahl:                
            
        PS("\n\r\033[32mMsgLister Commands: \033[33m?\033[36m,\033[33mCR\033[36m,\033[33mE\033[36m,\033[33mR\033[36m,\033[33mL\033[36m,\033[33mI\033[36m,\033[33m+\033[36m,\033[33m-\033[36m,\033[33m#\033[36m,\033[32m(");
        XIMFunction(507,0,"\033[31m~MB");   // JH_MCI      
        PS("\033[32m) \033[36m:\033[33m");
        
        XIMFunction(5,5," "); 
        
        sprintf(selectedbase,"%c%c%c",themsg.String[0],themsg.String[1],themsg.String[2]);
                      
        if(atoi(selectedbase) != 0)          // check for numerical input for joinin
        {                                    // the selected msgbase
                SXFunction(73,selectedbase); //  joinbase(selectedbase);          
                readmsgs(atoi(selectedbase),1,call);         // listendflag is active here
          
                baseindex=atoi(selectedbase);
                goto auswahl;
        }        
                             
        switch(themsg.String[0])
        {
                case '+':
                {       
                        if(maxbase==1)
                        {
                                PS("\n\r\033[37m Only one Message Base in this Conference !\n\r");              
                                goto auswahl;
                                
                        } else
                        {
                                if(baseindex==maxbase)
                                {
                                        PS("\n\r\033[37m You already reach the last Message Base !\n\r");
                                        goto auswahl;
                                }
                        }             
                        baseindex++;    
                        SXFunction(74,0);  // next base
                        listend=1;
                        readmsgs(baseindex,listend,call);
                        
                        goto auswahl;
                        
                }
                case '-':
                {
                        if(maxbase==1)
                        {
                                PS("\n\r\033[37m Only one Message Base in this Conference !\n\r");
                                goto auswahl;
                        }  
                        
                        baseindex--;
                        if(baseindex==0)
                        {
                                PS("\n\r\033[37m You already reach the first Message Base !\n\r");
                                baseindex++;
                                goto auswahl;
                        } else SXFunction(75,0); // prev base

                        listend=1;

                        readmsgs(baseindex,listend,call);
   
                        goto auswahl;
                }        
                case 'i':
                {
                        checkbases();
                        //baseindex=1;          // baseindex = 1 coz first msgbase is default...maybe i do a configfile ? 
                        goto auswahl;
                }
                case 'q':
                case '\r': 
                {
                        PS("\n\r");
                        return; 
                }
                case 'r':
                {
                        sprintf(msgnum,"%c%c%c%c",themsg.String[1],themsg.String[2],themsg.String[3],themsg.String[4]);
                        if(themsg.String[1]==' ')
                        {
                                if(themsg.String[2]==' ')
                                {
                                        sprintf(msgnum,"%c",0);
                                }
                        }        
                        SXFunction(72,msgnum);  // readbase
                        goto auswahl;
                }    
                                            
                case 'l':
                {
                        listend=0;                   // listend is not active - list msg nr. 1-
                        readmsgs(baseindex,listend,call); // listend = flag for complete list, or last msgs
                        goto auswahl;
                }                
                case'#':
                {
                        listend=1;                   // listend is active
                        readmsgs(baseindex,listend,call); // see above
                        goto auswahl;
                }        
                
                case '?':
                {
                        if(themsg.String[1]=='?') infos();
                        else help();
                        goto auswahl; 
                }
                
                
                
                case 'e':
                {
                        SXFunction(70,0);     //entermsg
                        goto auswahl;
                }                
        }                  
}          



void checkbases()
{        
        char buf[512],buffer[256],pathbuf[256],oldbase[256],lrp_flag[8],newansi[8];
        int baseindex=1;       
        BPTR fhbi,fh;

                 
        sprintf(oldbase,"EMPTY");
        
        // ConferenceReader, in witch the MessageBases are
        // some Informations first...
        //PS("\n\r\033[33m===============================================================================\n\r");
        PS("\n\r\n\r\033[36mMessage Bases in ");                                                
        XIMFunction(126,1,0);                    // BB_CONFNAME
        sprintf(buffer,"\033[35m%-12s",themsg.String);
        PS(buffer);
        XIMFunction(507,0," \033[37m~DT");
        PS(" [33m| ");
        XIMFunction(507,0,"[37m~CT");
        XIMFunction(507,0," [36mLastCall: [37m~LK[0m");   // JH_MCI      

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
                                PS("\033[33m----- -------  ---------------------- --------------------------- -------------\n\r");
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


void SXFunction(int fid, char *buf)
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


void basereset(void)
{
        SXFunc.id = 73;	// change default msgbase
        SXFunc.string = 0;  // here the string for base 1..
	SXFunc.extra = 1;
	SXFunc.low = 5;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(1506, (long)&SXFunc, 0);
       
}

void readmsgs(int baseindex, int listend, int call) // msgtopic liste - der baseindex wird gebraucht, um die msgs in der richtigen
{                                         //                  MsgBase zu lesen
        BPTR fh,fhb;
        UWORD LRP;
        char buf[512],buffer[512],lines[256],timestring[32],reply[8],msgs[1024],subject[32],lrpflag[4];
        char username[32],writer[32],reciver[32];
       
        int i=0,maxmsg,x;                 // i = zaehler, um die aktuelle base zu ermitteln
        int line;                         // line = variable fuer post converted lines (char)
                     
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhb = Open(buf, MODE_OLDFILE);
        
        XIMFunction(127,1,0);              //       BB_CONFLOCAL
        sprintf(buffer,"%sMessages/Base%ld/Messages.DAT",themsg.String,baseindex); 
        fh = Open(buffer, MODE_OLDFILE);

        if(call==0)
        {
                line=19;
        } else
        {
                XIMFunction(1501,0,0);                  // ermittelt die MaxLines des Users, welche i.d. settings
                User = (struct UserData *) themsg.Data; // eingestellt sind
                sprintf(lines,"%u",User->LineLength);   // wegen der MSG-Liste, weil eben jeder User eine eigene    
                line = atoi(lines);                     // ZeilenZahl hat.
        }

        XIMFunction(1503,0,0);  // pointer to sx userindexstruct
        Uid=(struct UserIndexStruct *) themsg.Data;
        sprintf(username,"%s", Uid->handle);

        LRP = GetLRP(baseindex, 0);

        if(fh)
        {  
                Seek(fhb,0,OFFSET_BEGINNING);
                while(i!=baseindex)  // solange i ungleich dem baseindex, bis current msgbase gefunden
                {
                          Seek(fhb,0,OFFSET_CURRENT); 
                          Read(fhb, &base, sizeof(struct MsgBaseStruct));
                          i++;
                }
              Close(fhb);
              PS("\n\r\n\r\033[36m");
              PS("  Messages in \033[31m");
              sprintf(buf,"%s",base.name);
              PS(buf);
                           
              Seek(fh,sizeof(struct MsgStruct)* -1,OFFSET_END);
              Read(fh, &msg, sizeof(struct MsgStruct));
              PS("\033[36m     MaxMsg : ");
              sprintf(buf,"\033[35m%ld",msg.number);
              PS(buf);
              sprintf(msgs,"%u",msg.number);
              maxmsg=atoi(msgs);
              
              PS("\033[33m");
              PS("\n\r ------------------------------ - ---  --------------- ------------------ ----\n\r"); 
          PS(" \033[35m nR. \033[37m  dATE   \033[36m tIME   \033[33mfROM \033[35m-->\033[33m tO      \033[32m sUBJECT .. .. ... . .                 \n\r");
          PS("\033[33m----- -----  ---------------  ---------------- ---------------------- --------\n\r");
                             
                if(listend==0)               // check the Listend : 0 = Full List  1 = Short List
                {
                        Seek(fh,0,OFFSET_BEGINNING);
                }
                else
                {       
                        if(maxmsg<=(line-9))
                        {        
                                Seek(fh,0,OFFSET_END);
                                Seek(fh,sizeof(struct MsgStruct) * -maxmsg, OFFSET_CURRENT); // <- i.e. for last 15 entries...
                        }
                        else
                        {
                                Seek(fh,0,OFFSET_END);
                                Seek(fh,sizeof(struct MsgStruct) * -(line-9),OFFSET_CURRENT);
                        }
                }    
                while(Read(fh, &msg, sizeof(struct MsgStruct)))
                {
                        if(msg.replycount!=0)         // reply - flag
                        {
                             if(msg.replycount>=2)
                             {
                                 sprintf(reply,">>");
                             } else sprintf(reply,">");
                        } else sprintf(reply,"");
	                                      
                        if(msg.flags&(1<<3))           // delete - flag
                        {
                                sprintf(subject,"\033[31mDELETED");
                        }
                        else
                        { 
                                if(msg.flags&(1<<4))           // private - flag
                                {                              // check if a msg has got the PRIVATE-flag
                                        username[4]=0;         
                                        sprintf(writer,"%c%c%c%c",msg.from[0],msg.from[1],msg.from[2],msg.from[3]);
                                        sprintf(reciver,"%c%c%c%c",msg.to[0],msg.to[1],msg.to[2],msg.to[3]);                                       
                                        x=0;       
                                                                                
                                        // make the private msgs visible for writer and reciver:
                                        if(username[3]==reciver[3])
                                        {
                                                sprintf(subject,"%-30s",msg.subj); 
                                                x++;
                                        } else 
                                        {
                                                if(username[3]==writer[3])
                                                {
                                                        sprintf(subject,"%-30s",msg.subj);               
                                                        x++;
                                                } 
                                        }
                                        if(x!=1)
                                        sprintf(subject,"\033[37mPRIVATE MESSAGE"); 
                                } else sprintf(subject,"%-30s",msg.subj);
                        } 
                       
                        subject[29]=0;   // cut the subject output to 28 chars
                                                                         
                        myctime(msg.msgtime,timestring); // die Zeit konvertieren


                        //         l.r.p - last read pointer :             
	               // Seek(fh, lrp*sizeof(struct MsgStruct), OFFSET_BEGINING);
	               // Read(fh, &Msg, sizeof(struct MsgStruct));
                       if(LRP+1<=msg.number) 
                       {
                              sprintf(lrpflag,"*"); 
                       } else 
                       {
                              sprintf(lrpflag,"");
                       }
                        
                        // Print the full String including all parms to the List on Screen
sprintf(buf,"\033[32m(\033[35m%4ld\033[32m) \033[36m%10s \033[33m%-8s %-8s \033[31m%s\033[35m%s\033[32m%s\r\n", msg.number, timestring, msg.from, msg.to, lrpflag, reply, subject);
                        PS(buf);
                                                              
                        // damit der screen nich rauslaufft kommt `line` ins spiel
                        if(i>=line-8)                        
                        {        
                                PS("\n\r");
                                XIMFunction(6,0,"\033[36m--[ \033[32mPause \033[33mCR\033[32m/\033[33mQ \033[36m]--> ");
                                if(themsg.String[0] == 'q') return;

                                PS("\n\r\n\r");

                                i=0;
                        }
                        i++;
               }
               PS("\n\r\033[37m No more Messages here !\n\r");
        }              
        Close(fh);    
}

void myctime(long secs, char *outbuf)
{
	struct DateStamp *tp;
	struct DateTime tostr;
                 
	tp = __timecvt((time_t)secs);

	memcpy(&tostr, tp, 12);

	tostr.dat_Format  = FORMAT_USA;
	tostr.dat_StrDay  = "2222222222";
	tostr.dat_StrDate = "000000000";
	tostr.dat_StrTime = "111111111";
	tostr.dat_Flags   = 0;

	DateToStr(&tostr);
        tostr.dat_StrTime[5]=0;
	sprintf(outbuf, "\033[37m%s \033[36m%s", tostr.dat_StrDate, tostr.dat_StrTime);
      
}
      
void help(void)
{
PS("\n\r");        
PS("   [36m.[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m.            [34m.[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m.\n\r");
PS("   [34m| [33mCR[32m/[33mQ [32m- [37mQuit [34m`[36m-[34m-[36m-[34m-[36m-[34m-[36m.     | [33m+[32m/[33m- [32m- [37mNext Prev MsgBase  [34m`[36m-[34m-[36m-[34m-[36m.\n\r");
PS("   `[34m-[36m. [33m?? [32m- [37mInformation [34m|     | [33mnum [32m- [37mEnter MsgBase by Number [34m`[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m.\n\r");
PS("  [36m.[34m-[36m-[34m'  [33m? [32m- [37mHelp Menu   [36m`[34m-[36m-[34m-[36m-[34m-[36m'   [33mI [32m- [37mMsgBases Index in current Conf  [36m|\n\r");
PS("  [34m| [33mR num [32m- [37mRead Msg by Number    [33mL [32m- [37mFull Messages List [36m.[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m'\n\r");
PS("  [36m`[34m-[36m-[34m-[36m-[34m.[33mE [32m- [37mEdit new Msg [34m.[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m.[33m# [32m- [37mLast Messages List [34m|\n\r");
PS("       [36m`[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m'       `[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m'\n\r");

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

void infos(void)
{
        char buf[512],ii[256];
        int i;
        PS("[2J[32m");
        Delay(20);
        i=0;
        sprintf(buf," WELCOME tO SX_MSG V1.1 dONE bY [r]fOx ");
        PS("[15;20H");
                
        while(i<=39)
        {
                sprintf(ii,"%c",buf[i]);
                PS(ii);
                Delay(8);
                i++;
        }
        
        PS("\n\r");
        XIMFunction(6,0,"[15;20H[33m         -  [37mpRESS nOW [33m -                      [0;0H");
        PS("[2J");
        
        sprintf(buf,"    cONTACT: house_bbs@hotmail.com        ");
        PS("[15;20H");
        
        i=0;        
        while(i<=39)
        {
                sprintf(ii,"%c",buf[i]);
                PS(ii);
                Delay(8);
                i++;
        }
        
        PS("\n\r");
        Delay(30);
        PS("[2J");
        
}        
        
        
      
        
        
        
        
        
        
        
