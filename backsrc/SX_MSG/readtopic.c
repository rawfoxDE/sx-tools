

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>   // z.b. fuer `isalnum(x)`
#include <time.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>
#include <sx/msgstruct.h>

struct MsgPort * bbsport;
struct MsgStruct msg;
struct MsgBaseStruct base;
struct UserData * User;

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

void readmsgs(int baseindex, int listend);                //- read the msg-topic list
void myctime(long secs, char *outbuf);  //- convert the time from LONG to CHAR 

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

			readmsgs(1,1);
                        
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


void readmsgs(int baseindex, int listend) // msgtopic liste - der baseindex wird gebraucht, um die msgs in der richtigen
{                                         // MsgBase zu lesen
        BPTR fh,fhb;
        char buf[512],lines[256],timestring[32],reply[8];
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
                Seek(fhb,0,OFFSET_BEGINNING);
                while(i!=baseindex)  // solange i ungleich dem baseindex, bis current msgbase gefunden
                {
                        if(fhb)
                        {      
                                Seek(fhb,0,OFFSET_CURRENT); 
                                Read(fhb, &base, sizeof(struct MsgBaseStruct));
                                i++;
                        }
                
                }
                Close(fhb);
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
                        else                 // wenn weniger msgs da sind, und alles ins minus rutscht
                        {
                                Seek(fh,0,OFFSET_END);
                                Seek(fh,sizeof(struct MsgStruct) * -(line-9),OFFSET_CURRENT);
                        }
                }
                              
              while(Read(fh, &msg, sizeof(struct MsgStruct)))
              {
                     if(msg.replycount != 0)
                     {
                         sprintf(reply,">");
                     } else sprintf(reply," ");
                          
                     myctime(msg.msgtime,timestring); // die Zeit konvertieren
//                     sprintf(flag,"%u",msg.flags); 
                     sprintf(buf,"\033[32m(\033[35m%4ld\033[32m) \033[36m%10s \033[33m%-8s %-8s\033[31m%s\033[32m%-30s\r\n", msg.number, timestring, msg.from, msg.to,reply, msg.subj);
                     PS(buf);
                     
                     // damit der screen nich rauslaufft kommt `line` ins spiel
                     if(i>=line-8)           // line = userzeilenzahl             
                        {        
                                PS("\n\r");
                                XIMFunction(6,0,"\033[36m--[ \033[32mPause \033[33mCR\033[32m/\033[33mQ \033[36m]--> ");
                                if(themsg.String[0] == 'q') return;
                                PS("\n\r\n\r");

                                i=0;
                        }
                     i++;
               }
              Close(fh);
              PS("\n\r\033[37m No more Messages here !\n\r");
        }              
            
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
	sprintf(outbuf, "%s %s", tostr.dat_StrTime, tostr.dat_StrDate);
}
