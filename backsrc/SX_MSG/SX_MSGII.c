/*

----------------------------------------------------
SX_MSG - a MsgBase Reader done by [r]fOx            
----------------------------------------------------

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>
#include <sx/msgstruct.h>
#define SX_FUNCTION 1506
static char __version[] ="\0$VER: SX_MSG V0.9beta4 bY [r]FoX at ("__DATE__")"; 
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
void Door(void);
void checkbases(void);
void actualbase(int);
void nextbase(void);
void prevbase(void);
void readbase(void);
void readmsgs(int, int);
void entermsg(void);
void basereset(void);
void help(void);
void myctime(long secs, char *outbuf);

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
                        basereset();
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
        int baseindex,listend;
        char buf[64],oldbase[64];
        static int maxbase;
        BPTR fhbi;
        
        sprintf(oldbase,"EMPTY");
                
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhbi = Open(buf, MODE_OLDFILE);
        Seek(fhbi,0,OFFSET_BEGINNING);
        if(fhbi)
        {
           
                Seek(fhbi,0,OFFSET_BEGINNING);
                while(fhbi)
                { 
                    
                        Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                        sprintf(buf," \033[32m(\033[33m%3ld\033[32m) - \033[36m%s",maxbase,base.name);
                        if(oldbase[32] == buf[32]) 
                        {
                                break;
                        }
                        strcpy(oldbase,buf);  // aktuelle msgbase in oldbase speichern
                        maxbase++;          
                        if(maxbase==500) break;
                          
                }
             // baseindex=1;        
        }       
        Close(fhbi);    


 //test
 /*       PS("\n\rMAXBASE == ");
        sprintf(buf,"%ld",maxbase);
        PS(buf);
        PS("\n\r");
 */      
        checkbases();
        // maxbase = maxbase + baseindex;                   
        baseindex=1;

        
 auswahl:        
               
        PS("\n\r\033[32mMsgLister Commands: \033[33m?\033[36m,\033[33mCR\033[36m,\033[33mE\033[36m,\033[33mR\033[36m,\033[33mL\033[36m,\033[33mI\033[36m,\033[33m+\033[36m,\033[33m-\033[36m,\033[33m#\033[36m,\033[32m(");

        actualbase(baseindex);

        PS("\033[32m) \033[36m:");
        
        XIMFunction(5,4," \033[33m");              
        switch(themsg.String[0])
        {
                case '+':
                {       
                        if(maxbase==1)
                        {
                                PS("\n\r\033[37m Only one Message Base in this Conference !\n\r");              
                                goto auswahl;
                                
                        }
                        if(baseindex==maxbase)
                        {
                                PS("\n\r\033[37m You reach already the last MsgBase !\n\r");
                                //baseindex--;
                                goto auswahl;
                        }     
                        baseindex++;   
                        nextbase();
                        listend=1;
                        readmsgs(baseindex,listend);
                        
                        goto auswahl;
                        
                }
                case '-':
                {
                        if(maxbase==1)
                        {
                                PS("\n\r\033[37m Only one Message Base in this Conference !\n\r");
                                goto auswahl;
                        }  
                        prevbase();
                        baseindex--;
                        listend=1;
                        if(baseindex==0)
                        {
                                PS("\n\r\033[37m You reach already the first Message Base !\n\r");
                                baseindex++;
                                goto auswahl;
                        } 
                        readmsgs(baseindex,listend);
   
                        goto auswahl;
                }        
                case 'i':
                {
                        checkbases();
                        baseindex=1;          // baseindex = 1 coz first msgbase is default...maybe i do a configfile ? 
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
                        readbase();
                        goto auswahl;
                }                
                case 'l':
                {
                        listend=0; 
                        readmsgs(baseindex,listend); // listend = flag for complete list, or last msgs
                        goto auswahl;
                }                
                case'#':
                {
                        listend=1;
                        readmsgs(baseindex,listend); // see above
                        goto auswahl;
                }        
                
                case '?':
                {
                        help();
                        goto auswahl; 
                }
                case 'e':
                {
                        entermsg();
                        goto auswahl;
                }                
        }                  
}          
void checkbases(void)
{
        
        char buf[64],oldbase[64];
        int maxbase=1;
        BPTR fhbi;

        sprintf(oldbase,"EMPTY");
        
        // ConferenceReader, in witch the MessageBases are
        PS("\n\r\033[34m-------------------------------------------------------------------------------\n\r");
        PS("\033[36mMessage Bases in ");
        XIMFunction(127,1,0);
        sprintf(buf,"\033[32m%-12s",themsg.String);
        PS(buf);
        
        PS("\033[36m     Conference Name : ");
        XIMFunction(126,1,0);                     // BB_CONFNAME
        sprintf(buf,"\033[35m%s",themsg.String);
        PS(buf);
        PS("\n\r\033[34m-------------------------------------------------------------------------------\n\r");
           
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhbi = Open(buf, MODE_OLDFILE);
        PS("\n\r");

        Seek(fhbi,0,OFFSET_BEGINNING);
                    
        if(fhbi)
        {
           
                Seek(fhbi,0,OFFSET_BEGINNING);
                while(fhbi)
                { 
                        Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                        sprintf(buf," \033[32m(\033[33m%3ld\033[32m) - \033[36m%s",maxbase,base.name);
                        if(oldbase[32] == buf[32]) 
                        {
                                PS("\n\r\033[0m No more MsgBases in this Conference !\n\r");
                                break;
                        }
                        PS(buf);
                        PS("\n\r");
                        strcpy(oldbase,buf);  // aktuelle msgbase in oldbase speichern
                        maxbase++;          
                        if(maxbase==500) break;
                       
                }
             // baseindex=1;        
        }       
        Close(fhbi);    
}

void nextbase(void)
{
   SXFunc.id = 74;	// SX-Function 74: Next Msg Base
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
}

void prevbase(void)
{
        SXFunc.id = 75;	// Previous Msg Base
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
}

void readbase(void)   // read msgs in current msgbase
{   	
	SXFunc.id = 72;	// read msgs
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
}

void entermsg(void)   // read msgs in current msgbase
{   	
	SXFunc.id = 70;	// enter a  msgs
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 5;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
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

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
 
        
        
}

void actualbase(int baseindex)
{               
        BPTR ab;
        char buf[64],basname[64],oldbase[64]; // buf = buffer fuer aktuelle conference
        int i = 0;                            // basname = current msgbasename
        sprintf(oldbase,"EMPTY");
        
        
        XIMFunction(127,1,0);       
        sprintf(buf,"%sMsgBases.DAT",themsg.String); 
        ab = Open(buf, MODE_OLDFILE);
        
        if(ab)
        {
                Seek(ab,0,OFFSET_BEGINNING);       
                
                while(i!=baseindex)  // solange i ungleich dem baseindex, bis current msgbase gefunden
                {
                       // if(ab)
                       // {      
                                Seek(ab,0,OFFSET_CURRENT); 
                                Read(ab, &base, sizeof(struct MsgBaseStruct));
                                i++;
                                sprintf(basname,"\033[31m%s\033[0m",base.name);
                                
                                if(oldbase[32] == basname[32]) 
                                {
                                        PS("\n\r\033[0m Last Message Base in this Conference !\n\r");
                                        break;
                                }
                                strcpy(oldbase,basname);
                        // }
                
                }
        }
        PS(basname);       
        Close(ab);
}

void readmsgs(int baseindex, int listend) // msgtopic liste - der baseindex wird gebraucht, um die msgs in der richtigen
{                            // MsgBase zu lesen
        BPTR fh,fhb;
        char buf[512],lines[256],timestring[32];
        int i=0,maxmsg=0;                            // i = zaehler, um die aktuelle base zu ermitteln
        int line;                                    // line = variable fuer post converted lines (char)
        
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhb = Open(buf, MODE_OLDFILE);
        
        XIMFunction(127,1,0);                                  //         BB_CONFLOCAL
        sprintf(buf,"%sMessages/Base%ld/Messages.DAT",themsg.String,baseindex); //  i.e. for base 1, weil baseindex=1
        fh = Open(buf, MODE_OLDFILE);

        XIMFunction(1501,0,0);                  // ermittelt die MaxLines des Users, welche i.d. settings
        User = (struct UserData *) themsg.Data; // eingestellt sind
        sprintf(lines,"%u",User->LineLength);   // wegen der MSG-Liste, weil eben jeder User eine eigene    
        line = atoi(lines);                     // ZeilenZahl hat.
                                                // line = lines (char) gewandelt zu int

        if(fh)
        {  
                while(i!=baseindex)  // solange i ungleich dem baseindex, bis current msgbase gefunden
                {
                        if(fhb)
                        {      
                                Seek(fhb,0,OFFSET_CURRENT); 
                                Read(fhb, &base, sizeof(struct MsgBaseStruct));
                                i++;
                        }
                
                }

              PS("\n\r\n\r\033[36m");
              PS("  Messages in \033[31m");
              sprintf(buf,"%s",base.name);
              PS(buf);
                           
              Seek(fh,0,OFFSET_BEGINNING);
              while(Read(fh, &msg, sizeof(struct MsgStruct)))
              {
                      maxmsg++;
              }   
              PS("\033[36m     MaxMsg : ");
              sprintf(buf,"\033[35m%ld",maxmsg);
              PS(buf);

          
              PS("\033[34m");
              PS("\n\r===============================================================================\n\r"); 
          PS("\033[35m  NR.  \033[37m TIME    \033[36m DATE     \033[33mFROM \033[35m-->\033[33m TO      \033[32m SUBJECT   \n\r");
          PS("\033[34m-------------------------------------------------------------------------------\n\r");
                             
                if(listend==0)
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
                     myctime(msg.msgtime,timestring); // die Zeit konvertieren
                     sprintf(buf,"\033[32m(\033[35m%4ld\033[32m) \033[36m%10s \033[33m%-8s %-8s \033[32m%-30s\r\n", msg.number, timestring, msg.from, msg.to, msg.subj);
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
            
}

void help(void)
{
        PS("\n\r\n\r\033[34m-----[\033[37m MsgLister Helpmenu\033[34m ]----------------------------------------------------\n\r");
        PS("\033[32m [\033[33mR\033[32m] -\033[36m Read Msgs   \033[32m [\033[33m#\033[32m] -\033[36m Last Msgs    \033[32m [\033[33m?\033[32m] -\033[36m This Menu       \033[32m [\033[33mCR\033[32m] -\033[36m Quit    \n\r");
        PS("\033[32m [\033[33m+\033[32m] -\033[36m Next MsgBase\033[32m [\033[33mL\033[32m] -\033[36m Full MsgList \033[32m [\033[33mI\033[32m] -\033[36m MsgBases in Conf  \033[32m[\033[33mE\033[32m] - \033[36mEnterMsg \n\r");
        PS("\033[32m [\033[33m-\033[32m] -\033[36m Prev MsgBase\n\r");
        PS("\033[34m-------------------------------------------------------------------------------");
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
	sprintf(outbuf, "\033[37m%s \033[36m%s", tostr.dat_StrTime, tostr.dat_StrDate);
}
