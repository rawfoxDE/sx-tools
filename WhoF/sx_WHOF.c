/*

SX_WHOF (Who Transfered)



static const char __version[] = "\0$VER: SX_WHOF 1.0 ("__DATE__")";
*/

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
void WhoF(char *arg);
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
               
               PS("[0m\r\nSystem-X WHOF [36m1.0 [0mBy Zed/DC\r\n\r\n");
               XIMFunction(1507, 0, 0);
			WhoF((char*)themsg.Data);

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

void WhoF(char *arg)
{
	char buf[256], timestr[32], filename[36];
	BPTR fh, fhup, fhdn;
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
	} Index;
	BOOL readfin = FALSE, found = FALSE;

	if(arg[0]!=0)
	{
		strcpy(filename, arg);
	} else {
		XIMFunction(5, 34, "[36mEnter filename to search[34m: [0m");
		PS("\r\n");
		if(themsg.Data==-1) ShutDown();
		if(themsg.String[0]==0) return;
		strcpy(filename, themsg.String);
	}

	fh = Open("SX:User.Index", MODE_OLDFILE);

	PS("\r\nSearching for the uploader...\r\n\r\n");

	fhup = Open("SX:LogFiles/Upload.LOG", MODE_OLDFILE);
	if(fhup)
	{
		while(!readfin)
		{
			if(Read(fhup, &Xfer, sizeof(struct XferLog)))
			{
				if(stricmp(Xfer.filename, filename)==0)
				{
					readfin = TRUE;
					found = TRUE;
				}
			} else {
				readfin = TRUE;
			}
		}
		Close(fhup);
		if(found)
		{
			Seek(fh, (Xfer.user_slot-1)*sizeof(struct UserIndexStruct), OFFSET_BEGINING);
			Read(fh, &Index, sizeof(struct UserIndexStruct));

			myctime(Xfer.time, timestr);
			sprintf(buf, "[33m%-28s [32m%-16s [36m%9ld [35m%s[0m\r\n", Index.handle, Xfer.filename, Xfer.size, timestr);
			PS(buf);
		} else {
			PS("[31mNo Uploader found...[0m\r\n");
		}
	} else {
		PS("[31mNo Uploader found...[0m\r\n");
	}


	PS("\r\nSearching for downloaders...\r\n\r\n");

	fhdn = Open("SX:LogFiles/Download.LOG", MODE_OLDFILE);
	if(fhdn)
	{
		readfin = FALSE;
		found = FALSE;
		while(!readfin)
		{
			if(Read(fhdn, &Xfer, sizeof(struct XferLog)))
			{
				if(stricmp(Xfer.filename, filename)==0)
				{
					found = TRUE;

					Seek(fh, (Xfer.user_slot-1)*sizeof(struct UserIndexStruct), OFFSET_BEGINING);
					Read(fh, &Index, sizeof(struct UserIndexStruct));

					myctime(Xfer.time, timestr);
					sprintf(buf, "[33m%-28s [32m%-16s [36m%9ld [35m%s[0m\r\n", Index.handle, Xfer.filename, Xfer.size, timestr);
					PS(buf);
				}
			} else {
				readfin = TRUE;
			}
		}
		Close(fhdn);
		if(!found) PS("[31mNo downloaders found...[0m");
	} else {
		PS("[31mNo downloaders found...[0m");
	}

	Close(fh);

	PS("\r\n\r\n");
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
