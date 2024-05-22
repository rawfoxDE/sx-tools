/*

----------------------------------------------------
SX_MSG - a MsgBase Reader done by [r]fOx
----------------------------------------------------

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>
#include <sx/msgstruct.h>
static char __version[] ="\0$VER: SX_MSG V0.9beta3 ("__DATE__")";
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
void nextbase(void);
void prevbase(void);
void readbase(void);
void readmsgs(int);
void help(void);


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
        int baseindex=1;
        
        checkbases();                   

 auswahl:        
               
        XIMFunction(5,4,"\n\r\033[32mMsgLister Commands: \033[33m?\033[36m,\033[33mCR\033[36m,\033[33mR\033[36m,\033[33mI\033[36m,\033[33m+\033[36m,\033[33m-\033[36m,\033[33m#\033[36m : ");
                      
        switch(themsg.String[0])
        {
                case '+':
                {
                        nextbase();
                    //    Seek(fhb,0,OFFSET_CURRENT+1);
                        baseindex++;
                        readmsgs(baseindex);
                        goto auswahl;
                        
                }
                case '-':
                {
                      //  Seek(fhb,0,OFFSET_CURRENT-1);
                        prevbase();
                        baseindex--;
                        readmsgs(baseindex);
                        goto auswahl;
                }        
                case '#':
                {
                        checkbases();
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
                case 'i':
                {
                        readmsgs(baseindex);
                        goto auswahl;
                }                
                case '?':
                {
                        help();
                        goto auswahl; 
                }        
        }
        
        //Close(fh);
        
}          
void checkbases(void)
{
        int baseindex = 1;
        char buf[64];
        char oldbase[64];
        // static char oldbase[64];
        BPTR fhbi;

        sprintf(oldbase,"EMPTY");
        
        // ConferenceReader, in witch the MessageBases are
        PS("\n\r\033[34m-------------------------------------------------------------------------------\n\r");
        PS("\033[36mMessage Bases in ");
        XIMFunction(127,1,0);
        sprintf(buf,"\033[32m%s",themsg.String);
        PS(buf);
        PS("\n\r\033[34m-------------------------------------------------------------------------------\n\r");
           
   
        XIMFunction(127,1,0);
        sprintf(buf,"%sMsgBases.DAT", themsg.String);
        fhbi = Open(buf, MODE_OLDFILE);
        PS("\n\r");
        
        if(fhbi)
        {
                //XIMFunction(6,0,"Joinin IF(FHB)...");
                Seek(fhbi,0,OFFSET_BEGINNING);

                while(fhbi)
                { 
                //XIMFunction(6,0,"Now Joinin WHILE(FHB)...");
                // Seek(fhb,0,OFFSET_CURRENT);
               
                Read(fhbi, &base, sizeof(struct MsgBaseStruct));
                //XIMFunction(6,0,"Read FHB...");
                                                
                sprintf(buf," \033[32m(\033[33m%3ld\033[32m) - \033[36m%s",baseindex,base.name);
                
                //XIMFunction(6,0,"now if the base exists in buf...");
                if(oldbase[32] == buf[32]) 
                {
                        PS("\n\r\033[0m No more MsgBases in this Conference !");
                        
                        break;
                }
                                
                PS(buf);
                PS("\n\r");
                
                //XIMFunction(6,0,"now copy to oldbase...");
                strcpy(oldbase,buf);
                baseindex++;       
                if(baseindex==10) break;
                }
        }
       //XIMFunction(6,0,"Closing FHBI...");
       
       Close(fhbi);    
}

void nextbase(void)
{
        #define SX_FUNCTION 1506
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
	
	SXFunc.id = 74;	// Next Msg Base
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
        #define SX_FUNCTION 1506
        
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
        #define SX_FUNCTION 1506
        
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
	
	SXFunc.id = 72;	// read msgs
	SXFunc.string = 0;  // here the string for args..
	SXFunc.extra = 0;
	SXFunc.low = 0;
	SXFunc.high = 255;
	SXFunc.mainarg = 0;
	SXFunc.execarg = 0;

	XIMFunction(SX_FUNCTION, (long)&SXFunc, 0);
}

void readmsgs(int baseindex) // msgtopic liste
{
        BPTR fh,fhb;
        char buf[512];
        int i=0;       // i = zaehler, um die aktuelle base zu ermitteln
        char lines[256];
        int line;
        
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
                           
           //   PS("    Baseindex = ");
           //   sprintf(buf,"%ld",baseindex);
           //   PS(buf);          
           
              PS("\033[34m");
              PS("\n\r===============================================================================\n\r"); 
                   

              Seek(fh,0,OFFSET_CURRENT);
              while(Read(fh, &msg, sizeof(struct MsgStruct)))
              {
                     sprintf(buf,"\033[32m(\033[35m%4ld\033[32m)\033[36m  From: \033[33m%8s  \033[36mTo:\033[33m%8s  \033[36m Subject: \033[33m%s\r\n", msg.number, msg.from, msg.to, msg.subj);
                     PS(buf);
                     
                     // damit der screen nich rauslaufft kommt `line` ins spiel
                     if(i>=line-7)                        
                        {
                                XIMFunction(6,0,"\n\r\033[36m--[ \033[32mPause \033[33mCR\033[32m/\033[33mQ \033[36m]--> ");
                                if(themsg.String[0] == 'q') break;
                                PS("\n\r");
                                i=0;
                        }
                     i++;

              }
        }              
            
}

void help(void)
{
        PS("\n\r\n\r\033[34m-----[\033[37m MsgLister Helpmenu\033[34m ]----------------------------------------------------\n\r");
        PS("\033[32m [\033[33mR\033[32m] -\033[36m Read Msgs   \033[32m [\033[33mI\033[32m] -\033[36m Msg List     \033[32m [\033[33m?\033[32m] -\033[36m This Menu       \033[32m [\033[33mCR\033[32m] -\033[36m Quit    \n\r");
        PS("\033[32m [\033[33m+\033[32m] -\033[36m Next MsgBase\033[32m [\033[33m-\033[32m] -\033[36m Prev. MsgBase\033[32m [\033[33m#\033[32m] -\033[36m MsgBases in Conf                \n\r");
        PS("\033[34m-------------------------------------------------------------------------------");
}

