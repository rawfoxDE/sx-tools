/*

SX NUKER

*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include "sxstructs.h"

struct MsgPort *bbsport;

struct JHMessage
{
	struct 	Message Msg;
	char	String[200];
	long	Data;
	long	Command;
} themsg;

struct XferLog
{
        UWORD   user_slot;
        UBYTE   conf,
                filearea;
        char    filename[32];
        long    size,
                baud,
                cps,
                time;
        UBYTE   node;
        char    res[7];
} XferLog;


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Nuke(char *fname);
void NukeFile(char *fname, char *dirbuf, long pos, long descsize, long bufsize);

char buf[512], listname[152];


void mysprintf(char *Buffer,char *ctl, ...)
{
	RawDoFmt(ctl, (long *)(&ctl + 1), (void (*))"\x16\xc0\x4e\x75", Buffer);
}

void myctime(long secs, char *outbuf)
{
	struct DateStamp *tp;
	struct DateTime tostr;

	tp = __timecvt((time_t)secs);

	memcpy(&tostr, tp, 12);

	tostr.dat_Format  = FORMAT_DOS;
	tostr.dat_StrDay  = "DIGITAL   ";
	tostr.dat_StrDate = "CORRUPTION";
	tostr.dat_StrTime = "IN 1997!!";
	tostr.dat_Flags   = 0;

	DateToStr(&tostr);
	mysprintf(outbuf, "%s %s", tostr.dat_StrDate, tostr.dat_StrTime);
}

int main(int argc, char *argv[])
{
	if(!argv[1] || argv[1][0]==0)
		PutStr("This program requires System-X BBS Software\n");
	else {
		mysprintf(buf, "AEDoorPort%s", argv[1]);
		bbsport = FindPort(buf);
		if(bbsport)
		{
			XIMFunction(1, 0, 0); 	// function 1 = register

			PS("\r\n[0;36;44m SX-Nuke 1.05 BETA [0m\r\n");

			XIMFunction(1507, 0, 0);	// SX_ARG

			Nuke((char *)themsg.Data);

			XIMFunction(2, 0, 0); 	// function 2 = shutdown
		}
	}
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

long FindFile(char *dirbuf, long size, char *fname)
{
	char *po;
	long i = 0;

	goto checkfile;

	while(i < size)
	{
		if(dirbuf[i] == 10)
		{
			i++;
checkfile:
			memcpy(buf, &dirbuf[i], 32);
			po = strchr(buf, ' ');
			if(po)
			{
				po[0] = 0;
				if(stricmp(fname, buf)==0)
					return(i);
			}
		}
		i++;
	}
	return(-1);
}

long DescSize(char *desc)
{
	register long i = 1;

	while(1)
	{
		if(desc[i] == 10)
		{
			i++;
			if(desc[i] != ' ')
			{
				long a, b;

				a = (long) desc;
				b = (long) &desc[i];

				return(b - a);
			}
		}
		if(desc[i] == 0) return(0);
		i++;
	}
}

void Nuke(char *arg)
{
	BOOL found = FALSE;
	BOOL done = FALSE;
	char fname[32], *dirbuf;
	UWORD dir = 0;
	BPTR fh;
	long size, pos, descsize;

	if(arg[0] == 0)
	{
		XIMFunction(5, 30, "\r\n[32mEnter filename[33m: [0m");
		PS("\r\n");
		strcpy(fname, themsg.String);
	} else
		strcpy(fname, arg);

	if(fname[0] == 0) return;

	XIMFunction(127, 1, 0);		// BB_CONFLOCAL

	while(!done)
	{
		dir++;
		mysprintf(listname, "%sDir%ld", themsg.String, dir);

		fh = Open(listname, MODE_OLDFILE);
		if(fh)
		{
			Seek(fh, 0, OFFSET_END);
			size = Seek(fh, 0, OFFSET_BEGINNING);

			dirbuf = AllocMem(size+2, MEMF_PUBLIC|MEMF_CLEAR);
			if(dirbuf)
			{
				Read(fh, dirbuf, size);
				pos = FindFile(dirbuf, size, fname);

				if(pos != -1)
				{
					descsize = DescSize(&dirbuf[pos]);
					Close(fh);
					fh = 0;
					if(descsize)
					{
						NukeFile(fname, dirbuf, pos, descsize, size);
						found = TRUE;
					}
					done = TRUE;
				}

				FreeMem(dirbuf, size+2);
			}
			if(fh) Close(fh);
		} else
			done = TRUE;
	}

	if(!found) PS("\r\nThat file cannot be found in this conference.\r\n\r\n");
}

void RemoveFile(char *fname, char *dirbuf, long pos, long descsize, long bufsize)
{
	BPTR fh = Open(listname, MODE_NEWFILE);
	if(fh)
	{
		long after;

		if(pos) Write(fh, dirbuf, pos);	// DATA BEFORE FILE

		after = pos + descsize;

		if(after != bufsize) Write(fh, &dirbuf[after], bufsize - after);	// DATA AFTER FILE

		Close(fh);

		PS("Filelist.... Altered.\r\n\r\n");

		XIMFunction(1511, 0, fname);	// SX_FINDFILE

		if(themsg.String[0])
		{
			strcpy(buf, themsg.String);
			strcat(buf, fname);
			DeleteFile(buf);
			PS("File........ Deleted.");
		} else
			PS("File........ Not Found.");
	} else
		PS("[0;31m\r\nCannot write to list - it might be in use.[0m\r\n");
}

void TakeCredits(char *fname, char *dirbuf, long pos, long descsize, long bufsize, struct XferLog *Xfer)
{
	char *comment;
	char tbuf[128];
	struct UserData User;
	struct UserData *ThisUser;

	BPTR ffh, fh = Open("SX:User.Data", MODE_OLDFILE);
	if(fh)
	{
		Seek(fh, (Xfer->user_slot-1)*sizeof(struct UserData), OFFSET_BEGINNING);
		Read(fh, &User, sizeof(struct UserData));
		Seek(fh, (Xfer->user_slot-1)*sizeof(struct UserData), OFFSET_BEGINNING);

		mysprintf(buf,	"\r\n\r\n"
				"[32mUploader      :[0m %s\r\n"
				"[32mUploaded Files:[0m %ld\r\n"
				"[32mUploaded Bytes:[0m %ld\r\n\r\n", User.Name, User.Uploads, User.Bytes_Upload);
		PS(buf);

		XIMFunction(5, 40, "Take how many times credits (eg 1, 2): ");

		if(themsg.String[0])
		{
			UWORD factor = atoi(themsg.String);
			mysprintf(buf,	"\r\n"
					"[32mUploaded Files: [0m%-30ld [32mUploaded Bytes: [0m%ld\r\n"
					"-    [32mDeduction: [0m%-30ld -    [32mDeduction: [0m%ld\r\n"
					"[35m---------------------------                    ----------------------------[0m\r\n"
					"     [32mNew Value: [0m%-30ld      [32mNew Value: [0m%ld\r\n\r\n",
						User.Uploads,
						User.Bytes_Upload,
						factor,
						Xfer->size * factor,
						User.Uploads - factor,
						User.Bytes_Upload - (Xfer->size * factor));
			PS(buf);
			strcpy(themsg.String, "[36mIs this correct[32m([33mY[32m/[33mn[32m)[36m? [0m");
getkey:
			XIMFunction(6, 0, 0);		// 6 = JH_HK

			switch(themsg.String[0])
			{
				case '\r':
				case '\n':
				case 'y':
				case 'Y':
					PS("Yes");
askcom:
					PS("\r\n\r\n[35m([36m1[35m)[0m Old\r\n"
						"[35m([36m2[35m)[0m Fake\r\n"
						"[35m([36m3[35m)[0m Shit\r\n"
						"[35m([36m4[35m)[0m Other\r\n"
						"[35m([36m5[35m)[0m No Comment\r\n\r\n"
						"Select: ");

					themsg.String[0] = 0;
					XIMFunction(6, 0, 0);	// 6 = JH_HK

					switch(themsg.String[0])
					{
						case '1':
							comment = "OLD!";
							break;
						case '2':
							comment = "FAKE!";
							break;
						case '3':
							comment = "SHIT!";
							break;
						case '4':
							XIMFunction(5, 20, "\r\n\r\nEnter comment: ");
							if(themsg.String[0])
							{
								strcpy(tbuf, themsg.String);
								comment = tbuf;
							} else
								comment = 0;
							themsg.String[0] = ' ';
							break;
						case '5':
							comment = 0;
							break;
						default:
							goto askcom;
					}

					themsg.String[1] = '\r';
					themsg.String[2] = '\n';
					themsg.String[3] = '\r';
					themsg.String[4] = '\n';
					themsg.String[5] = 0;
					PS(themsg.String);

					ffh = Open(listname, MODE_NEWFILE);
					if(ffh)
					{
						long after = pos + descsize;

						dirbuf[pos+13] = ' ';
						dirbuf[pos+14] = 'D';
						dirbuf[pos+15] = 'E';
						dirbuf[pos+16] = 'L';
						dirbuf[pos+17] = 'E';
						dirbuf[pos+18] = 'T';
						dirbuf[pos+19] = 'E';
						dirbuf[pos+20] = 'D';

						if(pos) Write(ffh, dirbuf, after);// DATA BEFORE FILE + FILE

						XIMFunction(1501, 0, 0);	// SX_USERPO
						ThisUser = (struct UserData *) themsg.Data;

						if(comment)
						{
							mysprintf(buf, "                                 [32m%s[34m: [35m%s[0m\n", ThisUser->Name, comment);
							Write(ffh, buf, strlen(buf));
						}

						mysprintf(buf, "                                             [32m%ld* [31mCredits Taken[0m\n", factor);
						Write(ffh, buf, strlen(buf));

						if(after != bufsize) Write(ffh, &dirbuf[after], bufsize - after);	// DATA AFTER FILE

						Close(ffh);

						PS("Filelist.... Altered.\r\n\r\n");

						XIMFunction(1511, 0, fname);	// SX_FINDFILE

						if(themsg.String[0])
						{
							strcpy(buf, themsg.String);
							strcat(buf, fname);
							DeleteFile(buf);
							PS("File........ Deleted.");
						} else
							PS("File........ Not Found.");

						User.Uploads -= factor;
						User.Bytes_Upload -= Xfer->size * factor;

						Write(fh, &User, sizeof(struct UserData));
						PS("\r\n\r\nAccount..... Saved.");
					} else
						PS("[0;31mCannot write to list - it might be in use.[0m\r\n");
					break;
				case 'n':
				case 'N':
					PS("No");
					break;
				default:
					themsg.String[0] = 0;
					goto getkey;
			}
		}
		Close(fh);
	}
}

void Reward(char *fname, char *dirbuf, long pos, long descsize, long bufsize, struct XferLog *Xfer)
{
	struct UserData User;

	BPTR ffh, fh = Open("SX:User.Data", MODE_OLDFILE);
	if(fh)
	{
		Seek(fh, (Xfer->user_slot-1)*sizeof(struct UserData), OFFSET_BEGINNING);
		Read(fh, &User, sizeof(struct UserData));
		Seek(fh, (Xfer->user_slot-1)*sizeof(struct UserData), OFFSET_BEGINNING);

		mysprintf(buf,	"\r\n\r\n"
				"[32mUploader      :[0m %s\r\n"
				"[32mUploaded Files:[0m %ld\r\n"
				"[32mUploaded Bytes:[0m %ld\r\n\r\n", User.Name, User.Uploads, User.Bytes_Upload);
		PS(buf);

		XIMFunction(5, 40, "Reward how many times credits (eg 1, 2): ");

		if(themsg.String[0])
		{
			UWORD factor = atoi(themsg.String);
			mysprintf(buf,	"\r\n"
					"[32mUploaded Files: [0m%-30ld [32mUploaded Bytes: [0m%ld\r\n"
					"+       [32mReward: [0m%-30ld +    [32m   Reward: [0m%ld\r\n"
					"[35m---------------------------                    ----------------------------[0m\r\n"
					"     [32mNew Value: [0m%-30ld      [32mNew Value: [0m%ld\r\n\r\n",
						User.Uploads,
						User.Bytes_Upload,
						factor,
						Xfer->size * factor,
						User.Uploads + factor,
						User.Bytes_Upload + (Xfer->size * factor));
			PS(buf);
			strcpy(themsg.String, "[36mIs this correct[32m([33mY[32m/[33mn[32m)[36m? [0m");
getkey:
			XIMFunction(6, 0, 0);		// 6 = JH_HK

			switch(themsg.String[0])
			{
				case '\r':
				case '\n':
				case 'y':
				case 'Y':
					PS("Yes\r\n\r\n");

					ffh = Open(listname, MODE_NEWFILE);
					if(ffh)
					{
						long after = pos + descsize;

						if(pos) Write(ffh, dirbuf, after);// DATA BEFORE FILE + FILE

						mysprintf(buf, "                                             [32m%ld* [33mCredits Given[0m\n", factor);
						Write(ffh, buf, strlen(buf));

						if(after != bufsize) Write(ffh, &dirbuf[after], bufsize - after);	// DATA AFTER FILE

						Close(ffh);

						PS("Filelist.... Altered.");

						User.Uploads += factor;
						User.Bytes_Upload += Xfer->size * factor;

						Write(fh, &User, sizeof(struct UserData));
						PS("\r\n\r\nAccount..... Saved.");
					} else
						PS("[0;31mCannot write to list - it might be in use.[0m\r\n");
					break;
				case 'n':
				case 'N':
					PS("No");
					break;
				default:
					themsg.String[0] = 0;
					goto getkey;
			}
		}
		Close(fh);
	}
}

void NukeFile(char *fname, char *dirbuf, long pos, long descsize, long bufsize)
{
	struct UserData User;
	struct XferLog Xfer;
	BPTR fh;
	WORD i, j = 0;
	char timestr[32];
	BOOL menu;

	PS("[0m\r\n");

	for(i=0 ; i<descsize ; i++)
	{
		if(dirbuf[pos+i] == 10)
		{
			buf[j] = 13;
			j++;
			buf[j] = 10;
			j++;
			buf[j] = 0;
			PS(buf);
			j = -1;
		} else
			buf[j] = dirbuf[pos+i];

		j++;
	}

	PS("\r\n[32mSearching for uploader...[0m[A\r\n");

	fh = Open("SX:LogFiles/Upload.Log", MODE_OLDFILE);
	if(fh)
	{
		while( Read(fh, &Xfer, sizeof(struct XferLog)) )
		{
			if(stricmp(fname, Xfer.filename) == 0)
			{
				Close(fh);
				goto find;
			}
		}
		Close(fh);
	}
	PS("Uploader could not be found. File was Upped by the Sysop !\n\r");
	Xfer.user_slot = 1;

find:
	if(Xfer.user_slot != 0)
	{
		fh = Open("SX:User.Data", MODE_OLDFILE);
		if(fh)
		{
			Seek(fh, (Xfer.user_slot-1)*sizeof(struct UserData), OFFSET_BEGINNING);
			Read(fh, &User, sizeof(struct UserData));
			Close(fh);

			myctime(Xfer.time, timestr);

			mysprintf(buf, "[32mUploader:[0m %s  [32mUpload Time:[0m %s  [32mCPS Rate:[0m %ld\r\n\r\n", User.Name, timestr, Xfer.cps);
			PS(buf);
		}
	}
showmenu:
	PS(	"[35m([36m1[35m)[0m Nuke file and take credits\r\n"
		"[35m([36m5[35m)[0m Remove file from filelist and harddisk\r\n"
		"[35m([36m9[35m)[0m Reward Uploader\r\n\r\n"
		"[35m([36mQ[35m)[0m Quit and Abort\r\n"
		"\r\n[36mSelect: [35m[[32m-[35m][0m[2D");

	do
	{
		menu = FALSE;
		themsg.String[0] = 0;
		XIMFunction(6, 0, 0);		// 6 = JH_HK

		if(themsg.Data == -1) break;	// LOST CARRIER

		PS(themsg.String);

		switch(themsg.String[0])
		{
		case '1':
			if(Xfer.user_slot == 0)
			{
				menu = TRUE;
				PS("\r\n\r\nUploader unknown - Cannot take credits.\r\n\r\n");
				goto showmenu;
			} else
				TakeCredits(fname, dirbuf, pos, descsize, bufsize, &Xfer);
			break;
		case '5':
			RemoveFile(fname, dirbuf, pos, descsize, bufsize);
			break;
		case '9':
			if(Xfer.user_slot == 0)
			{
				menu = TRUE;
				PS("\r\n\r\nUploader unknown - Cannot reward.\r\n\r\n");
				goto showmenu;
			} else
				Reward(fname, dirbuf, pos, descsize, bufsize, &Xfer);
			break;
		case 'q':
		case 'Q':
			break;
		default:
			PS("\b \b");
			menu = TRUE;
		}
	}
	while(menu);

	PS("\r\n\r\n");
}
