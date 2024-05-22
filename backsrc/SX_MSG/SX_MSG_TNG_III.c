/*

------------------------------------------------------------------------
SX_MSG The next Generation - a MsgBase Reader done by [r]fOx  
29 may 1998                                                   
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

static char __version[] ="\0$VER: SX_MSG V1.2pre bY [r]FoX at ("__DATE__")"; 
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
struct ConfStruct Conf;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;
// Prototypes for all the functions                                                            PROTOTYPS
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void MessageLister(void);                               //- the mainloop
void checkbases(void);                                  //- schaeck the installed msgbases
void SXFunction(int fid, int extra , char *buf);        //- SXFunction
void readmsgs(int baseindex, int listend, int call);    //- read the msg-topic list
void basereset(void);                                   //- while leaving door() reset current msgbase to 1
void help(void);                                        //- show the small helpmenu
void myctime(long secs, char *outbuf);                  //- convert the time from LONG to CHAR 
UWORD GetLRP(UWORD basenum, WORD which);                //- get the L.ast R.ead P.ointer
void PutLRP(UWORD basenum, WORD which, UWORD LRP);      //- put that
void set_unread(void);                                  //- set all msgs in all confs/bases to old (lrp(1))
void infos(void);                                       //- main information page, screen length related
void mailscan(int scanflag);                            // scanflag 0 for NEW, 1 for UNREADED
void ms_header(void);                                   // header for NewMail Scan or unread MailScan 
void nm_header(void);
void prompt(void);
BOOL sx;
// Main communication                                                                          MAIN    
int main(register int argc, register char *argv[])
{
	char portname[16], *arg;
        
	if(argv[1][0]==0)
	{
                PutStr("This program requires System-X BBS Software");
	        exit(0);
        }  else 
        {               
	        sprintf(portname, "AEDoorPort%s", argv[1]);
	        bbsport = FindPort(portname);
	        if(bbsport)
	        {
		        XIMFunction(1, 0, 0); 	/* function 1 = register */

		        /* find out if we are under SYSTEM-X or AmiExpress */

		        if(strcmp(themsg.String,"SX")==0) sx=TRUE; else sx=FALSE;

                        XIMFunction(1507, 0, 0);          // called the door online with argument for mailscan only ?
	                arg = (char *)themsg.Data;
	                if(arg[0]=='s' || arg[0]=='S' || arg[1]=='s' || arg[1]=='S')
	                {
		                mailscan(0);
                                XIMFunction(2,0,0);
                                exit(0);             

	                } else
                        {
               			MessageLister();
                                basereset();  // reset the actual msg-base to base1
                        }
                        
			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}
// PrintString                                                                            PRINT STRING 
void PS(char * str)
{
	if(sx) XIMFunction(1500, (long)str, 0); else XIMFunction(4, 0, str);
}
// ximfunction                                                                            XIMFUNCTIONS 
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

// MessageLister                                                                MESSAGELISTER MAINLOOP 
void MessageLister(void)
{
        int maxbase,baseindex,listend, call=1;
       
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
                        if(strcmpi(oldbase,basnam)==NULL || maxbase>=500) break; 
                                                
                        strcpy(oldbase,basnam);  // aktuelle msgbase in oldbase speichern
                        maxbase++; 
                                              
                }
                Close(fhbi); 
        }       
                
        checkbases(); // start with bases index here
        baseindex=1;  // and set default while startup

auswahl:                
        prompt();    
        XIMFunction(5,5," "); // await the users input
        
        sprintf(selectedbase,"%c%c%c",themsg.String[0],themsg.String[1],themsg.String[2]);
                      
        if(atoi(selectedbase)!=0)          // check for numerical input for joinin
        {                                    // the selected msgbase
                if(atoi(selectedbase)>=maxbase+1)
                {
                        PS("\r[1A[K[37m No such MessageBase here !");
                        Delay(40);
                        PS("[1A\r");
                        goto auswahl;
                }
                SXFunction(73,0,selectedbase);        //  joinbase(selectedbase);          
                readmsgs(atoi(selectedbase),1,call);  // listendflag is active here
          
                baseindex=atoi(selectedbase);
                goto auswahl;
        }        
                             
        switch(themsg.String[0])
        {
                case '+':
                {       
                        if(maxbase<=2)
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
                        SXFunction(74,0,0);  // next base
                        listend=1;
                        readmsgs(baseindex,listend,call);
                        
                        goto auswahl;
                        
                }
                
                case '-':
                {
                        if(maxbase<=2)
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
                        } else SXFunction(75,0,0); // prev base

                        listend=1;

                        readmsgs(baseindex,listend,call);
   
                        goto auswahl;
                }        
                
                case '*':
                case 'I':
                case 'i':
                {
                        checkbases();
                        //baseindex=1;          // baseindex = 1 coz first msgbase is default...maybe i do a configfile ? 
                        goto auswahl;
                }
                
                case 'Q':
                case 'q':
                case '\r': 
                {
                        PS("\n\r");
                        return; 
                }
                
                case 'R':
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
                        SXFunction(72,0,msgnum);  // readbase
                        goto auswahl;
                }    
                
                case 'L':                            
                case 'l':
                {
                        sprintf(msgnum,"%c%c%c%c",themsg.String[1],themsg.String[2],themsg.String[3],themsg.String[4]);
                        if(themsg.String[1]==0) sprintf(msgnum,"1");
                        listend=atoi(msgnum)-1;             // listend is not active - list msg nr. 1-
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
                        if(themsg.String[1]=='?') { infos(); checkbases(); }
                        else help();
                        goto auswahl; 
                }
                         
                case 'E':               
                case 'e':
                {
                        SXFunction(70,0,0);     //entermsg
                        checkbases();
                        goto auswahl;
                }
                 
                case 's':                     // unreadet mail index 
                {                             // or reset all LRPs(1)
                        if(themsg.String[1]=='o' || themsg.String[2]=='o')
                        {
                                XIMFunction(5,2,"\n\r[37mDo you really like to set all Msgs to [31mOLD[37m ? [33m");
                                if(themsg.String[0]==0 || themsg.String[0]=='y' ) 
                                {
                                        set_unread(); // set all unreaded msgs in all confs/masgbases to read/old
                                        goto auswahl;
                                } else if(themsg.String[0]=='n' || themsg.String[0]=='q') goto auswahl;      
                        }                    
                        mailscan(1);                        
                        goto auswahl;
                }
                
                case 'S':                      // scan for newmail
                {
                        themsg.String[0]=0;
                        sprintf(buf,"%u",baseindex);
                        mailscan(0);                        
                        SXFunction(73,0,buf);        //  joinbase(selectedbase);  
                        //basereset();
                        goto auswahl;        
                }                       
        }                  
}          


// check the msgbases for num, etc                                                           CHECKBASES
void checkbases(void)
{        
        char buf[512],buffer[256],pathbuf[256],oldbase[256],lrp_flag[8],newansi[8];
        int baseindex=1;       
        BPTR fhbi,fh;
                         
        sprintf(oldbase,"EMPTY");
        
        // ConferenceReader, in witch the MessageBases are
        // some Informations first...
        
        PS("\n\r\n\r\033[36mMessage Bases in ");                                                
        XIMFunction(126,1,0);     // BB_CONFNAME
        themsg.String[12]=0;      // cut the output for correct gfx
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
                        sprintf(buf,"%u",msg.number-GetLRP(baseindex,2));   // L.R.P.
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
                        
                        if(strcmpi(oldbase,name)==NULL) 
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

// execute some SXFunctions                                                             SX_FUNCTION    
void SXFunction(int fid, int extra, char *buf)
{
        SXFunc.id = fid;	// SX-Function 74: Next Msg Base
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = extra;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = buf;
	SXFunc.execarg = 0;  

   XIMFunction(1506, (long)&SXFunc, 0); // SX_FUNCTION
}

// while leaving the door set actual base to 1                                             BASERESET   
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
// Read the List of Msgs                                                                   READMSGS    
void readmsgs(int baseindex, int listend, int call) // msgtopic liste - der baseindex wird gebraucht, um die 
{                                                   // Messages in der richtigen MsgBase zu lesen
        BPTR fh,fhb;
        //UWORD LRP;
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

        XIMFunction(1501,0,0);  // pointer to sx userindexstruct
        User=(struct UserData *) themsg.Data;
        sprintf(username,"%s", User->Name);

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
                {       if(listend==1)
                        {
                                if(maxmsg<=(line-9))
                                {        
                                        Seek(fh,0,OFFSET_END);
                                        Seek(fh,sizeof(struct MsgStruct) * -maxmsg, OFFSET_CURRENT); // <- i.e. for last 15 entries...
                                } else
                                {
                                        Seek(fh,0,OFFSET_END);
                                        Seek(fh,sizeof(struct MsgStruct) * -(line-9),OFFSET_CURRENT);
                                }
                        } else if(listend>=1)
                        {
                                Seek(fh, listend * sizeof(struct MsgStruct), OFFSET_BEGINNING);                                                                
                        }        
                }    

                // Read the Msgs now ................................
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
                                        strcpy(writer,msg.from);
                                        strcpy(reciver,msg.to);                                       
                                        x=0;       
                                                                                
                                        // make the private msgs visible for writer and reciver:
                                        if(strcmpi(username,reciver)==NULL)
                                        {
                                                sprintf(subject,"%-30s",msg.subj); 
                                                x++;
                                        } else 
                                        {
                                                if(strcmpi(username,writer)==NULL)
                                                {
                                                        sprintf(subject,"%-30s",msg.subj);               
                                                        x++;
                                                } 
                                        }
                                        if(x!=1)
                                        sprintf(subject,"\033[37mPRIVATE MESSAGE"); 
                                } else sprintf(subject,"%-30s",msg.subj);
                        } 
                       
                        subject[29]=0;   // cut the subject output to 29 chars
                                                                         
                        myctime(msg.msgtime,timestring); // die Zeit konvertieren
                             
                        if(GetLRP(baseindex,2)+1<=msg.number) 
                        {
                               sprintf(lrpflag,"*");     // LRP-Flag for the list
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
               PS("\n\r[37m No more Messages here !\n\r");
        }              
        Close(fh);    
}
// Timeconverter                                                                               MYCTIME 
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
	sprintf(outbuf, "[37m%s [36m%s", tostr.dat_StrDate, tostr.dat_StrTime);
      
}
// Mailscan/Newscan                                                                           MAILSCAN 


void mailscan(int scanflag)
{
        char buf[512],buffer[256],pathbuf[256],oldbase[256],lrp_flag[8],newansi[8];
        char confname[45],oldconfname[45],newbuf[256],username[32];                           
	int baseindex=1,totalmsgs=0,totalnew=0,foryou=0;// buf         - any buffer 512 Bytes
        int confnumba=0,end=0;
        UWORD LRP;                                      // buffer      - any buffer 256 Bytes
        BPTR fhbi,fh,fhc;                               // pathbuf     - buffer for Confpath
        // switch the gfx                               // oldbase     - needed to stop reading msgbases in conf   
        if(scanflag==0) ms_header();                    // lrp_flag    - shows how many new msgs as num.
        else nm_header();                               // newansi     - colored background for bases with new msgs                      
        sprintf(oldbase,"EMPTY");                       // confname    - current conferencename
        sprintf(oldconfname,"EMPTY");                   // oldconfname - needed to srop reading the confs
                
        XIMFunction(1501,0,0);                          // pointer to sx userindexstruct
        User=(struct UserData *) themsg.Data;
        sprintf(username,"%s", User->Name);
                                                        // baseindex   - current msgbase in actual conf
        fhc = Open("sx:prefs/confs.dat",MODE_OLDFILE);  // LRP         - Last Read Pointer
        Seek(fhc,0,OFFSET_BEGINNING);                   // fhbi        - filehandle baseindex
        while(fhc)                                      // fh          - filehandle msgbases
 	{                                               // fhc         - filehandle conferences
                Read(fhc,&Conf,sizeof(struct ConfStruct));
                strcpy(confname,Conf.name);
                
                confnumba++;                            // ceck the users conferences access level here
                XIMFunction(614, confnumba, 0);         // JH_CONFACCESS #614 - undocumented
                if(!themsg.Data) end++;
                                
                if(strcmpi(confname,oldconfname)==NULL || end!=0) // quit the loop if no more confs
                {       // has user no axx only to this but not to next conf ?
                        if(end!=0 && strcmpi(confname,oldconfname)!=NULL) { end=0; goto Ignore; }      
                        if(scanflag==0)    
                        {
                                XIMFunction(109,1,0); // DT_MESSAGESPOSTED
                                sprintf(newbuf,"%s",themsg.String);
                                PS("\n\r\n\r[33m---- ---------- ------ - ------------------------- ----------------------- ----");
                                sprintf(buf,"[1;31;44m\n\r  Messages visited : [37m%-5ld  [31mMsgs written : [37m%-4s  [31mTotalNew : [37m%-4ld [31mForYou : [37m%-3ld  [0m", totalmsgs, newbuf, totalnew, foryou);                    
                                PS(buf);
                                PS("\n\r[33m--------- ------------------------- ------ ---------------------------- -------");
                                PS("\n\r[37mnothing more to Scan...\n\r");                        
                       
                                Close(fhc);
                                return;
                       } else
                       {
                                XIMFunction(109,1,0); // DT_MESSAGESPOSTED
                                sprintf(newbuf,"%s",themsg.String);
                                PS("\n\r\n\r[33m---- ---------- ------ - ------------------------- ----------------------- ----");
                                sprintf(buf,"[1;31;44m\n\r  Messages visited : [37m%-5ld  [31mMessages written : [37m%-4s    [31mTotal unreaded : [37m%-4ld   [0m", totalmsgs, newbuf, totalnew);                    
                                PS(buf);
                                PS("\n\r[33m--------- ------------------------- ------ ---------------------------- -------");
                                PS("\n\r[37mnothing more to Scan...\n\r");                        
                       
                                Close(fhc);
                                return;                       
                       } 
                        
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
                                sprintf(maxmsg,"%ld",msg.number);            // count the total messages in
                                totalmsgs=(totalmsgs + atoi(maxmsg));        // all bases
                                                                           
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
                        
                                if(strcmpi(oldbase,name)==NULL) 
                                {                                           
                                        baseindex=1;                                                                       
                                        Close(fhbi);
                                        break;               
                                }                       
                                PS(buf);
                                PS("[0m");
                      
                                if(scanflag==0)
                                {                                            
                                        LRP = GetLRP(baseindex,0);
                                        if(LRP!=msg.number)                 // new mail found !
                                        {     
                                                totalnew++;
                                                sprintf(pathbuf,"%sMessages/Base%ld/Messages.DAT",Conf.path,baseindex); 
                                                fh = Open(pathbuf, MODE_OLDFILE);

                                                if(fh) // check if da msg is really for you 
                                                {                                                              
                                                        Seek(fh,(LRP) * sizeof(struct MsgStruct), OFFSET_BEGINNING);
                                                        while(Read(fh, &msg, sizeof(struct MsgStruct)))
                                                        {                                                                                                                        
                                                           if(strcmpi(username,msg.to)==NULL || strcmpi(msg.to,"eall")==NULL || strcmpi(msg.to,"all")==NULL)
                                                           {                                                                                                                                
                                                                foryou++;
                                                                
                                                                PS("\n\r[31m-------- --------- -- - ------- --------- ---------------  ---------- ---------\n\r"); 
                                                                PS("\n\r");
                                                                PS("[31m-------- ----------- - ---------------- --------------- ---------  ------------\n\r"); 
                                                                PS("[3A");                                                               
                                                                PS("\n\r      [1;34;42m NEW MAIL FOUND !! [0m [37mWould you like to read it ? [1;34;42m ([31mQ[34m,[31mN[34m,[31mCR[34m/[31mY[34m) [0m: ");
                                                                XIMFunction(6,0,0);
                                                                if(themsg.String[0]=='\r' || themsg.String[0]=='y' || themsg.String[0]=='Y')
                                                                {                              
                                                                        sprintf(buf,"%u",baseindex);
                                                                        SXFunction(73,0,buf); // join the msgbase with new mail
                                                                                                                                        
                                                                        sprintf(buf,"%u",msg.number);
                                                                        SXFunction(72,0,buf);  // read the new mail
                                                                                                
                                                                        PS("\n\r                   ");
                                                                        
                                                                        // here the Lrp is set to last msg, coz SXFunction 
                                                                        // read msg does not end after 1 msg ....logo
                                                                                                                                               
                                                                        Seek(fh, -sizeof(struct MsgStruct), OFFSET_END);
                                                                        Read(fh, &msg, sizeof(struct MsgStruct));
                                                                        PutLRP(baseindex,0,msg.number);
                                                                        
                                                                        foryou--;
                                                                        totalnew=totalnew-(LRP+1); //if msg was read new totalnew                              
                                                                        if(totalnew<=0) totalnew=0;
                                                                                             
                                                                 } else 
                                                                 {
                                                                        totalnew=totalnew+atoi(lrp_flag);
                                                                        if(themsg.String[0]=='q') 
                                                                        {
                                                                                PS("\n\r");
                                                                                Close(fh);
                                                                                Close(fhbi);
                                                                                Close(fhc);
                                                                                return;
                                                                        }        
                                                                 } 
                                                                         
                                                                 if(q==2)
                                                                 {
                                                                        PS("\r[1A[K[1B[K[1B[K[3A");  // clear the questionstring on screen
                                                                 } else PS("\r[1A[K[1B[K[1B[K[3A[48C"); // but keep the gfx

                                                           } else PutLRP(baseindex,0,msg.number);
     
                                                           if(LRP==msg.number || msg.number<=atoi(maxmsg))
                                                           {
                                                                 Close(fh);
                                                                 break;
                                                           }
                                                        }    
                                                }                               
                                        }                                 
                                }
                                // count the new or unreaded Msgs like scanflag is set
                                sprintf(newbuf,"%u",msg.number-GetLRP(baseindex,scanflag));
                                if(newbuf[0]!='0') totalnew=totalnew+atoi(newbuf);
                                     
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
// jump here if user has no axx   \/    
        Ignore:                
                strcpy(oldconfname,confname);                
                }
        }
        Close(fhc);
        PS("[0m");
        return;
}       
// get the LRP from `which`                                                                    GET_LRP 
UWORD GetLRP(UWORD basenum, WORD which)
{
	BPTR fhl;
	UWORD LRP;
	void *freemem;
        char buf[256];
        
        XIMFunction(1501,0,0);
        User = (struct UserData *) themsg.Data;
        
        if(which==2) 
        {
                XIMFunction(127,1,0);
                sprintf(buf, "%sMessages/Base%ld/LRP.DAT",themsg.String,basenum);
                which=1;
        } else sprintf(buf,"%sMessages/Base%ld/LRP.DAT",Conf.path, basenum);
        
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
// put the LRP ..only needed for MailScan                                                       PUT_LRP
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

void set_unread(void)
{
        char buf[512],pathbuf[256],oldbase[256];
        char confname[45],oldconfname[45];                           
	int baseindex=1;       
                                           
        BPTR fhbi,fh,fhc;                          
        
        PS("[37mstay cewl a moment please, reset all Msgs to [31mOLD [37m..."); 
                          
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
                        PS("\n\r[37mAll Messages set to [31mOLD [37mnow !\n\r");                        
                        Close(fhc);
                        return;
                }                 
                sprintf(buf,"%sMsgBases.DAT", Conf.path);
                fhbi = Open(buf, MODE_OLDFILE);
                                  
                Seek(fhbi,0,OFFSET_BEGINNING);
                if(fhbi)               
                {                                  
                        char name[64];
                        Seek(fhbi,0,OFFSET_BEGINNING);
                        while(fhbi)
                        { 
                                Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                                sprintf(name,"%s",base.name);
                                                        
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
                                if(baseindex>=500) break;                     
                        }
                strcpy(oldconfname,confname);                
                }
        }
        Close(fhc);
}

// Commands Help                                                                              HELP MENU
void help(void)
{
PS("\n\r");
PS("    [36m.[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m.            [34m.[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m.\n\r");
PS("    [34m| [33mCR[32m/[33mQ [32m- [37mQuit [34m`[36m-[34m-[36m-[34m-[36m-[34m-[36m.     | [33m+[32m/[33m- [32m- [37mNext Prev MsgBase  [34m`[36m-[34m-[36m-[34m-[36m.\n\r");
PS("    `[34m-[36m. [33m?? [32m- [37mInformation [34m|     | [33mnum [32m- [37mEnter MsgBase by Number [34m`[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m.\n\r");
PS("   [36m.[34m-[36m-[34m'  [33m? [32m- [37mHelp Menu   [36m`[34m-[36m-[34m-[36m-[34m-[36m'   [33mI [32m- [37mMsgBases Index in current Conf  [36m|\n\r");
PS("   [34m|     [33mS [32m- [37mNew Msg Scanner       [33ms [32m- [37mUnreaded Msg Index `[33ms o[37m` set [31mOLD[34m|\n\r");
PS("   [36m| [33mR num [32m- [37mRead Msg by Num.  [33mL num [32m- [37mFull Messages List [34m.[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m'\n\r");
PS("   [34m`[36m-[34m-[36m-[34m-[36m.[33mE [32m- [37mEdit new Msg [36m.[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m.[33m# [32m- [37mLast Messages List [36m|\n\r");
PS("        [34m`[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m'       `[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m-[36m-[34m'\n\r");

}
// Info`s                                                                                      INFO    
void infos(void)
{
        char buf[128],ii[128];
        int i;
               
        PS("[2J[33m");
        Delay(20);
        i=0;
        sprintf(buf,"%u",User->LineLength);
                  
PS("[34m.---------------------------------------------.------[1m [22m{[1m[43m SX_MSG - Help [22m[40m}[1m [22m------.\n\r"); 
PS("|  [31m?[32m/[31m??[37m..[36mview a small [33mHelp[36mMenu[32m/[36mshow this page [34m|                               |\n\r"); 
PS("|>-------------------------------------------<|                               |\n\r"); 
PS("|     [31mE[37m..[33mEnter [36ma Msg in normal way            [34m|                               |\n\r"); 
PS("|>--------------------------------------------`------------------------------>|\n\r"); 
PS("| [31mR num[37m..[33mRead [36ma Msg [33mwith [36mgiven [33mnumber[36m. Only the Msgs in the current MsgBase   [34m|\n\r"); 
PS("|         [36mcan be visited. Its not needed to give an argument                  [34m|\n\r"); 
PS("|>---------------------------------------------------------------------------<|\n\r"); 
PS("| [31mL num[37m..[33mList [36mthe Msgs. If no numerical argument is given, the list will be   [34m|\n\r"); 
PS("|         [36mreaded from Msg #1-EOF, else the Msgs will listed from Nr. to EOF   [34m|\n\r"); 
PS("|>---------------------------------------------------------------------------<|\n\r"); 
PS("|     [31m#[37m..[33mList [36mlast Msgs. Your ScreenLength ( Usersettings) is relevant here.  [34m|\n\r"); 
PS("|>---------------------------------------------------------------------------<|\n\r"); 
PS("|     [31mI[37m..[33mIndex [36mof the MsgBases in the current Conference                      [34m|\n\r"); 
PS("|         [36mConferencename, actual Date/Time and last Login are on the top.     [34m|\n\r"); 
PS("|         [36mThe Bases list shows MsgBase number, total Msgs and global New ones [34m|\n\r"); 
PS("|        [37mEnter a number at the Command Prompt let you join the Base with Nr.  [34m|\n\r");
PS("|>---------------------------------------------------------------------------<|\n\r"); 

if(atoi(buf)<=27)
{
        PS("`-----------------------------------------------------------------------------'\n\r"); 
        sprintf(buf," You have less then 28 Lines ");
        PS("[2;49H[37m");
        while(i<=29)
        {
                sprintf(ii,"%c",buf[i]);
                PS(ii);
                Delay(1);
                i++;
        }
        PS("\n\r");
                 
        sprintf(buf," in your settings.  For more ");
        PS("[3;49H");
        
        i=0;        
        while(i<=29)
        {
                sprintf(ii,"%c",buf[i]);
                PS(ii);
                Delay(1);
                i++;
        }
        PS("[4;49H[1;33;44m Press Return or `q` to quit![0m");
        XIMFunction(5,0," ");
        if(themsg.String[0]==0) 
        {
                i=0;
                PS("[2J");
                 
                PS("[34m.---------------------------------------------.------[1m [22m{[1m[43m SX_MSG - Help [22m[40m}[1m [22m------.\n\r"); 
                PS("|      [31mgREETz :[35m Digital^Corruption[36m, [35mZed[36m,      [34m|                               |\n\r"); 
                PS("|[35m      Raped[36m, [35mTrantor[36m, [35mUbit[36m, [35mJgL[36m, [35mRasta[36m,      [34m|                               |\n\r"); 
                PS("| [35m     #elitecafe[36m, [35mMerlin[36m, [35mWizzler[32m([33mgetSX[32m)     [34m|                               |\n\r"); 
                PS("|>--------------------------------------------`------------------------------>|\n\r"); 
        } else if(themsg.String[0]=='q') { PS("[2J"); return; }
        
}
PS("|     [31mS[37m..[33mScan [36mfor new mails. [33mUppercase [36mCommand [33m! [36mIf Newmail is found, a small [34m|\n\r"); 
PS("|         [36mCommandprompt let you choose: quit scan, read new mail, do nothing  [34m|\n\r"); 
PS("|>---------------------------------------------------------------------------<|\n\r"); 
PS("|     [31ms[37m..[33mscan [36mfor unreaded mails. [33mLowercase [36mCommand [33m! [36mConfused [33m? [36mIts good to  [34m|\n\r"); 
PS("|         [36mknow, that SX has two Pointers for Mail: 1 for Mailscan and         [34m|\n\r"); 
PS("|         [36m1 for unreaded Mails. If you like to reset all unreaded Msgs,       [34m|\n\r"); 
PS("|         [36myou may enter `[33mso[36m` or `[33ms o[36m`.                                        [34m|\n\r"); 
PS("|>---------------------------------------------------------------------------<|\n\r"); 
PS("|   [31m+[32m/[31m-[37m..[33mup[32m/[33mdown [36ma MsgBase. You get notice if there is`nt any more MsgBase    [34m|\n\r"); 
PS("`-----------------------------------------------------------------------------'[32m"); 
 
        sprintf(buf," -[ SX_MSG V1.2 bY [r]FoX ]- ");
        PS("[2;49H");
                
        while(i<=29)
        {
                sprintf(ii,"%c",buf[i]);
                PS(ii);
                Delay(2);
                i++;
        }
        
        PS("\n\r");
        Delay(50);
        //PS("[2J");
        
        sprintf(buf," -[ house_bbs@hotmail.com ]- ");
        PS("[3;49H");
        
        i=0;        
        while(i<=29)
        {
                sprintf(ii,"%c",buf[i]);
                PS(ii);
                Delay(2);
                i++;
        }
        
        //PS("\n\r");
        PS("[4;49H[1;33;44m amichurch.home.ml.org  [37m<[31mCR[37m> [0m");
        XIMFunction(5,0," ");
        if(themsg.String[0]==0) PS("[2J");
       
        
}        
// Header for Mailscan                                                                 HEADER MAILSCAN
void ms_header(void)
{
       PS("[2J");
       PS("                        [36m-[[32m NEW MAIL Scan in progress[36m ]-\n\r");
       PS("[33m-------- ----------- - ---------------- --------------------------- -----------\n\r");         
       PS("[0;35;44m cONFERENCEs         mESSAGE bASEs [32m[ [33mnEW mSGs[32m ]                                [0m\n\r");
       PS("[33m -- - --------------- - -------------------- ---------- ------------------ ----\n\r"); 
        
}
// Header for New Mails                                                                HEADER NEWMAIL 
void nm_header(void)        
{
       PS("[2J");
       PS("                        [36m-[[32m UNREADED Message Index[36m ]-\n\r");
       PS("[33m-------- ----------- - ---------------- ----------------------------- ---------\n\r");         
       PS("[0;35;44m cONFERENCEs         mESSAGE bASEs [32m[ [33muNrEAD mSGs[32m ]                             [0m\n\r");
       PS("[33m -- - -------------- - --------------------- ---------- ----- ------- ---------\n\r"); 
        
}
void prompt(void)
{        
        PS("\n\r\033[32mMsgLister Commands: \033[33m?\033[36m,\033[33mCR\033[36m,\033[33mE\033[36m,\033[33mR\033[36m,\033[33mL\033[36m,\033[33mI\033[36m,[33mS[36m,[33ms[36m,\033[33m+\033[36m,\033[33m-\033[36m,\033[33m#\033[36m,\033[32m(");
        XIMFunction(507,0,"\033[31m~MB");   // JH_MCI      ******************************************
        PS("\033[32m) \033[36m:\033[33m");  //             * here is the bug but maybe not the prob *
                                            //             ******************************************
}
