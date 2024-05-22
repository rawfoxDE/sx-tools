/*

hi peter, hgere i try to do i/o with sequential files...

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <fcntl.h>          // i read i must include fcntl.h ...must i (i.e. for Open ) ?
#include <ctype.h>
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
void AReq(void);
void pause(void);
void cursormenu(BPTR fh, long textsize);

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

			AReq();

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

void AReq(void)
{
       	BPTR fh;
	
    //  buff - kleiner workbuffer, buf - grosser textbuffer, charbuf - 1 zeichen
    //  list - wahl der aktuellen liste
    //  fname/fpath - Filename/path     
        char buff[256],buf[128],charbuf[1],list[32];
        char fname[19],fpath[11];
    //  i - spaltenzaehler i.d. liste, q - Zeilenzaehler    
        int i=0,q=1;
        long textsize=0;  
    //  Listenauswahl :    
        PS("\n\r[0;33;44m AmiNet Requester - Select List to View [0m\n\r");
        PS("[33m----------------------------------------\n\r");
        PS("[32m      1 - Aminet Charts \n\r");
        PS("      2 - Aminet Recent \n\r");
        XIMFunction(6,0," ");
        if(themsg.String[0]=='1') sprintf(list,"sx:aminet/CHARTS");
        if(themsg.String[0]=='2') sprintf(list,"sx:aminet/RECENT");                  
               
	fh = Open(list, MODE_OLDFILE);
        PS("[2J");
        Seek(fh,0,OFFSET_BEGINNING);
	if(fh)
	{
                PS("\n\r");
        	while(Read(fh, buf, sizeof(charbuf)))
		{                                
                        sprintf(charbuf,"%c",buf[0]);
                        
                        textsize++;
                        if(charbuf[0]=='|')     // if a '|'-character is found - used on Aminet-Lists as began of non-files lines
                        {
                                do              // exclude the \n
                                {
                                        Read(fh,buf, sizeof(charbuf));
                                        sprintf(charbuf,"%c",buf[0]);
                                        //textsize++;
                                } while(isalpha(charbuf[0])==0 || charbuf[0]=='\n');      // detect newline      

                                do              // exclude the crap
                                {
                                        Read(fh,buf, sizeof(charbuf));
                                        sprintf(charbuf,"%c",buf[0]);
                                        //textsize++;
                                } while(isalpha(charbuf[0])==0);    


                                PS("[1A");
                        } //else PS(charbuf);
                   
                        if(charbuf[0]=='\n')           // if a new line begins
                        {               
                                PS("\n\r");
                                i=0;
                                if(q==20) 
                                { 
                                        q=0; 
                                        cursormenu(fh,textsize);
                                       // pause(); 
                                        
                                }
                                q++;
                        }  
                        
                        if(i==0 && (isalpha(charbuf[0])!=0 || isdigit(charbuf[0])!=0 || charbuf[0]=='.'))
                        {                                
                                strncat(fname,charbuf,1);
                        }
                        if(i==1 && (isalpha(charbuf[0])!=0 || charbuf[0]=='/'))
                        {
                                strncat(fpath,charbuf,1);
                                
                        }
                        
                        if(charbuf[0]==' ' && i<=3)     // if a space-character is detected in the list
                        {
                                if(i==0)
                                {
                                        sprintf(buff,"[31m%-19s",fname);  // type the filename after detect first space                 
                                        PS(buff);
                                        fname[0]=0;
                                       
                                        do              // exclude these chars
                                        {
                                                Read(fh,buff, sizeof(charbuf));
                                                sprintf(charbuf,"%c",buff[0]);
                                                textsize++;
                                        } while(charbuf[0]==' ');      // detect newline      
                                        
                                        Seek(fh,-1,OFFSET_CURRENT);    // get first pathchar
                                        
                                }                                                      
                                if(i==1)
                                {
                                        sprintf(buff,"[32m%-11s",fpath);  // type the filename after detect first space                 
                                        PS(buff);
                                        fpath[0]=0;
                                        
                                        do              // exclude these chars
                                        {
                                                Read(fh,buff, sizeof(charbuf));
                                                sprintf(charbuf,"%c",buff[0]);
                                                textsize++;
                                        } while(charbuf[0]!='\n');      // detect newline      
                                        //PS("\n\r");
                                }                                
                                i++;                                                                     
                        }                                                                                           
                }		
                Seek(fh, 0, OFFSET_END);
       		Close(fh);
	} 
}
void pause(void)
{
        XIMFunction(5,0,"\n\rPaused....");
        if(themsg.String[0]=='q') return;
        PS("[2J");
}
void cursormenu(BPTR fh,long textsize)
{
        char marktext[128],buf[128], buff[128], charbuf[1];
       // int currline=1;
        long linesize;
       
        sprintf(buff,"%ld",textsize);
        PS(buff);
                        
                
        
newline:        
        marktext[0]=0;
        linesize=0;
        Seek(fh, -textsize, OFFSET_CURRENT);
 
        PS("\n\r Enter the Loop for market text now...");
        do
        {
                Read(fh, buff, sizeof(charbuf));
                sprintf(buf,"%c",buff[0]);
                strncat(marktext,buf,1);
                linesize++;
        } while(buf[0]!='\n');
        PS("\n\rLoop passed...");
        PS("[0;0H");
        sprintf(buff,"[0;33;44m%s[0m",marktext);
        PS(buff);
        
        XIMFunction(5,1," ");
        if(themsg.String[0]=='q') return;
        if(themsg.String[0]=='y')
        {
                PS("\r[K");                       // cursor links, zeile loeschen
                Seek(fh,-linesize,OFFSET_CURRENT); // pointer 1 zeile zurueck
                Read(fh, buff, sizeof(linesize));  // lese die leile
                sprintf(marktext,"[0m%s",buff);   // zeile in buffer
                PS(marktext);
                PS("\r[1B");
                goto newline;
        }        
                
                
}
