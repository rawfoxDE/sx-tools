/*
Last Downloads
*/

static const char __version[] = "\0$VER: Last_Upps 1.0 ("__DATE__")";

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>

struct MsgPort *bbsport;
struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
  int NodeID;
  int LineNum;
  unsigned long signal;
  struct Process *task;
  APTR *Semi;
} themsg;

void ShutDown(void);
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void TopFile(void);
void myctime(long secs, char *outbuf);

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

			TopFile();

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void ShutDown(void)
{
	XIMFunction(2, 0, 0);
	exit(0);
}

void PS(char * str)
{
	XIMFunction(1500, (long)str, 0);
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

void TopFile(void)
{
	char buf[256], timestr[32];
        int i=0;
	BPTR fhdn, fh, fhc;
        // ************************** some structs here ***********************************
	struct XferLog
	{
		UWORD	user_slot;
		UBYTE	conf,
		        filearea;
		char	filename[32];
		long	size,
			baud,
			cps,
			time;
		UBYTE	node;
		char	res[7];
	} Xfer;
	
	struct UserIndexStruct
	{
		char handle[31];
		char realname[31];
		UWORD misc;
	} Uid;

        struct ConfStruct
        {
	        char name[45];
	        char path[55];
	        char pass[16];
	        char filepath[52];
	        UWORD fileareas;
        	UWORD uploadarea;
        	UBYTE flf;
        	UBYTE flags;
        	char reserve[82];
        } Conf;
        // *******************************************************************************

	PS("[0m\r\nSystem-X Last Uploads [36m1.0 [0mby [r]FoX\r\n");

	fhdn = Open("SX:LogFiles/Upload.LOG", MODE_OLDFILE);
        
        fh = Open("SX:User.Index", MODE_OLDFILE);
        
        fhc = Open("SX:Prefs/Confs.DAT",MODE_OLDFILE);
        

        //Seek(fhdn,sizeof(struct XferLog) *-20,OFFSET_END); // last 20 entrys
	Seek(fhdn,sizeof(struct XferLog)*-1,OFFSET_END);
        if(fhdn)
	{
                
                                
                PS("\n\r User     Conference        FName        Size     Time          CPS  Baud\n\r");
		while(fhdn)
                {
                                                
		        Read(fhdn, &Xfer, sizeof(struct XferLog));
			                        
                        Seek(fh, (Xfer.user_slot-1)*sizeof(struct UserIndexStruct), OFFSET_BEGINING);
			Read(fh, &Uid, sizeof(struct UserIndexStruct));

                        Seek(fhc, (Xfer.conf-1) *sizeof(struct ConfStruct), OFFSET_BEGINNING);
                        Read(fhc, &Conf, sizeof(struct ConfStruct));
 
                        myctime(Xfer.time,timestr);
                        
                        // cut the strings here ..
                        Uid.handle[8]=0;
                        Conf.name[17]=0;
                        Xfer.filename[13]=0;                
                        //timestr[19]=0;
                        
sprintf(buf,"[32m%-8s [33m%-17s [31m%-13s [35m%-7ld %18s[0;37m %-4ld [0;33;44m%-5ld[0m\n\r", Uid.handle, Conf.name,  Xfer.filename, Xfer.size, timestr, Xfer.cps, Xfer.baud);   
                        PS(buf);
                        i++;
			if(i>=20) 
                        { 
                                XIMFunction(5,1,"\n\r [Q,CR] ...Pause :");
                                switch(themsg.String[0])
                                {
                                        case 'q':
                                        {
                                                Close(fhdn); 
                                                Close(fh); 
                                                Close(fhc); 
                                                ShutDown();       
                                        }        
                                }              

                                i=0; 
                                
                        }                                
                        Seek(fhdn, sizeof(struct XferLog) *-2, OFFSET_CURRENT);
                }
                Close(fhdn);
                Close(fh);
                Close(fhc);  
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
        sprintf(outbuf, "[0;31;43m%s [34m%s", tostr.dat_StrTime, tostr.dat_StrDate);
}

 
        
