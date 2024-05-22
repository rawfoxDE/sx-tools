/*

-------------------------------------------------------
   NEWvAIi  -  a NEW USER Validator                    
               by [r]FoX                               
-------------------------------------------------------

See the botton of this source!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>
#include <time.h>

static char __version[] ="\0$VER: NEWvALi V1.0b bY [r]FoX at ("__DATE__")";

struct MsgPort *bbsport;

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
void newrep(void);
void myctime(long secs, char *outbuf);
void BPS(char * str, char * p);
void header(char * p);
void noheader(char * p);
void footer(char * p);
void nofooter(char * p);

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

			newrep();

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void PS(char * str)
{
	if(sx) XIMFunction(1500, (long)str, 0); else XIMFunction(4, 0, str);
}

void BPS(char * str, char * p)
{	
        BPTR fh;        
                            
        fh = Open(p, MODE_OLDFILE);  
                         
  newin:
        if(!fh) 
        {  
                fh = Open(p, MODE_NEWFILE); 
                goto newin;
        } else
        {        
                 Seek(fh,0,OFFSET_END);
                 Write(fh, str, strlen(str));
                 Close(fh);
        }    
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

void newrep(void)
{
	struct UserData User;                                         // fhc  - configfile 
	struct UserIndexStruct Index;                                 // fhax - access.DAT 
        struct AccessStruct Axx;                                      // fhi  - UserIndex
                                                                      // fhu  - User.Data 
	BPTR fhc, fhax, fhi, fhu = Open("SX:User.Data", MODE_OLDFILE);//  p   - bull outputpath
  	                                                              // newbies - space for 10 new users
        BOOL listfin = FALSE;                                         //           with 256= 2.56k 
	 	
        char p[128], buf[128], charbuf[1], timestr[32], newbies[10][256];          //  x   - newbie read counter
        int  x=0,y=0;                                                 //  y   - newbie write counter
        long firstaxx;
        
        // config checks the bulletinpath
        fhc = Open("sx:doors/NewRep/nr.config",MODE_OLDFILE);
        if(!fhc)     
        {
                sprintf(p,"sx:bulletins/NewUser.ans"); // default path               
        }
        else                                       
        {       
                p[0]=0;                         
                Seek(fhc,0,OFFSET_BEGINNING);          // if nv.config exists  
                while(Read(fhc, buf, sizeof(charbuf)))
                {
                        sprintf(charbuf,"%c",buf[0]);
                        if(charbuf[0]=='\n') break;   
                        strncat(p, charbuf, 1);          // copy targetpath
                        
                }        
                Close(fhc);                                  
        }
        
        if(p)
        {
                sprintf(buf,"c:delete %s",p);          // delete the old ansi bevor writing a newone
                Execute(buf,0,0);
        }
        
	if(fhu)                                        // looking for newbies now
	{                                
                Axx = sizeof(struct AccessStruct);
                fhax = Open("sx:prefs/access.dat", MODE_OLDFILE);
                if(!fhax) return;
                else
                {
                        Read(fhax,&Axx,sizeof(struct AccessStruct));
                        Close(fhax);
                }
                firstaxx = Axx.access;                 // check NewUser - access level
                              
		fhi = Open("SX:User.Index", MODE_OLDFILE);
		if(fhi)
		{                        
			while(!listfin)
			{                                                                                              
                                if(!(FRead(fhu, &User, sizeof(struct UserData), 1)))
				{
					listfin = TRUE;
				} else 
                                {
					if(!(FRead(fhi, &Index, sizeof(struct UserIndexStruct), 1)))
					{
						listfin = TRUE;
					} else 
                                        {                                                
					        if(!(User.Flags&(1<<6)))  // Deleted ?
						{
                                                        if(User.Sec_Status!=firstaxx);
                                                        else
                                                        {        
                                                           myctime(User.Time_Last_On,timestr); 
                                                           sprintf(newbies[x],"      [31m|    [31m%-12s [33m%-14s %-14s     [33m%5ld        [31m|\r",User.Name, User.Location, timestr, User.Times_Called); 
                                                           x++;
                                                        }                                                                                    
				                }                                           
	                                }                                                    
               			}
			}
			Close(fhi);
		}
		Close(fhu);
               
                if(!x) { noheader(p); nofooter(p); }  // if no newbies are found
                else
                {       
                        header(p);                    // otherwise
                        do
                        {                                       
                                BPS(&newbies[y][0],p);
                                y++;
                                
                        } while(y==(x-1));        
                        footer(p);
                }
        BPS("\n\r",p);
        return;
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
        tostr.dat_StrTime[5]=0;
	sprintf(outbuf, "\033[37m%s[32m/[37m%s", tostr.dat_StrDate, tostr.dat_StrTime);
      
}

void header(char * p)
{
BPS("                      [31m__________________________________\r",p);
BPS("                      [0;34;43m Newbie Reporter V 1.0a by [r]FoX [0m\r",p); 
BPS("      [31m.--------------/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯[31m\\--------------.\r",p);
BPS("      |                [32mNEW USERS FOUND ON THIS SYSTEM !                [31m|\r",p);
BPS("      |               ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯               [31m|\r",p);
BPS("      |    [37mName        Location        FirstLogin       TotalLogins    [31m|\r",p);
BPS("      |[0;36;44m>--------------------------------------------------------------<[0m[31m|\r",p);

}

void noheader(char * p)
{
BPS("                      [31m__________________________________\r",p);
BPS("                      [0;34;43m Newbie Reporter V 1.0a by [r]FoX [0m\r",p); 
BPS("      [31m.--------------/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯[31m\\--------------.\r",p);
BPS("      |[0;33;44m             NO NEW USERS FOUND HERE AT THE MOMENT !            [0m[31m|\r",p);

}

void footer(char * p)
{
        BPS("      |[0;36;44m>--------------------------------------------------------------<[0m[31m|\r",p);
        BPS("      |   [32mIf you know any of this New Users above, so please leave a   [31m|\r",p);
        BPS("      |            [32mComment to the Sysop or try to Page him.            [31m|\r",p);
        BPS("      `----------------------------------------------------------------'\r",p);
}

void nofooter(char * p)
{
        BPS("      `----------------------------------------------------------------'\r",p);
}

