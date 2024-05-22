/*

----------------------------------------------------
Programming doors for System-X using AEDoor.library.
----------------------------------------------------

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libraries/aedoor.h>

#include <proto/exec.h>

struct	Library	*AEDBase;

struct	DIFace	*d;
char	*strf,*res,usern[50],str[256],location[100];
long c=0;
BOOL sx;

void PS(char *str);

main(argc, argv)
int argc;
char *argv[];
{
	if((AEDBase=(struct Library *)OpenLibrary(AEDoorName,0))==NULL)
	{
		exit(10);
	}
	d=CreateComm(argv[1][0]);	/* Establish link with /X */

	strf= GetString(d);		/* Get a pointer to the JHM_String  */
					/* field. Only need to do this once */

	if(strcmp(strf,"SX")==0) sx=TRUE; else sx=FALSE; /* find out if we are under SYSTEM-X or AmiExpress */

	if(sx) {
		PS("\r\nThis door is running under System-X!\r\n\r\n");
	} else {
		PS("\r\nThis door is running under AmiExpress!\r\n\r\n");
	}

	GetDT(d,DT_NAME,0);		/* Get USER name, no string input   */
					/* here, so use 0 as last parameter */
	strcpy(usern,strf);		/* Copy result from JHM_String to   */
					/* our own string. Don't forget this*/
	GetDT(d,DT_LOCATION,0);
	strcpy(location,strf);

	PS("User name : ");		/* Write some text */
	PS(usern);
	PS("\r\nLocation  : ");
	PS(location);
	PS("\r\n");

	GetDT(d,DT_DUMP,"T:user.dump");		/* Dump user's data struct */

	if( (res=Prompt(d,80,"\nGimme some input: ")) )
	{						/* Ask some input */
		strcpy(str,res);
		PS("Entered: ");
		PS(str);
		PS("\r\n");
		if( (res=GetStr(d,3,"YES")) )
		{
			if(!strcmp(res,"YES"))
			{
				ShowFile(d,"S:User-Startup");
				ShowGFile(d,"BBS:BULL30");
			}
		}
		else PS("\r\n\r\nLOST CARRIER!\r\n");

	}
	else PS("\r\n\r\nLOST CARRIER!\r\n");

	DeleteComm(d);
	CloseLibrary(AEDBase);
}

void PS(char *str)
{
	if(sx)
		SendStrDataCmd(d, 1500, 0, (long)str);
	else
		WriteStr(d, str, NOLF);
}
