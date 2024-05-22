
/*
 * FAMETrapDoor v1.2
 * ~~~~~~~~~~~~~~~~~
 *
 * $VER: FAMETrapDoor.c v1.2
 *
 * Code by: David 'Strider/tRSi' Wettig
 * E-Mail:  strider@trsi.de
 * URL:     http://www.trsi.de/inno/strider/index.html
 *
 * Used tab size: 2
 */

/*
 * Includes:
 */

#include <stdio.h>
#include <strings.h>
#include <proto/exec.h>
#include <FAME/FAME.h>

/*
 * Disable Lattice CTRL-C handling:
 */

#ifdef LATTICE
int CXBRK(void) {return(0);}
int chkabort(void) {return(0);}
#endif

/*
 * Define our version string for the "version" command:
 */

char *VerStr = {"$VER: FAMETrapDoor v1.2 "__AMIGADATE__};

/*
 * Prototypes:
 */

void SPrintf( char *Buffer, char *ctl, ... );
void ShutDown( long );

/*
 * No CON window please, quit immediately.
 */

void __autoopenfail(void) { _XCEXIT(0);}

/*
 * Variables for ReadArgs and Template.
 */

char 		ArgStr[]		= "NODENR/K/N/A,DTERATE/K/N/A,CONNECTMSG/F/A";
long			ArgArray[]	= {0L,0L,0L};

/*
 * Global variables:
 */

char	ConnectMsg[256], WorkBuffer[256], WorkStr[256];
long NodeNr, DTERate;

struct FAMELibrary		*FAMEBase					= NULL;
struct MsgPort				*FAME_ServerPort	= NULL;
struct MsgPort				*ServerReply			= NULL;
struct FAMEServerMsg	*FAME_ServerMsg		= NULL;

/*
 * void main( void );
 *
 * main() entry point:
 */

void main( void )
{
	struct  RDArgs *rda	= NULL;

	Printf("\nFAMETrapDoor v1.2 "__AMIGADATE__" by David 'Strider' Wettig\n\nFAMETrapDoor starts/calls a FAME Node via TrapDoor.\n\n");

	if( ! (FAMEBase = (struct FAMELibrary *)	OpenLibrary(FAMENAME,4L)))
	{
		ShutDown( 1 );
	}

	/*
	 * Use ReadArgs() for arguments:
	 */

	if(rda = ReadArgs(ArgStr,ArgArray,NULL))
	{
		/*
		 * Read the Node number:
		 */

		if(ArgArray[0])
		{
			long *LDummy = (long *)ArgArray[0];
			NodeNr = (*LDummy);
		}
		else
		{
			FreeArgs( rda );
			ShutDown( 2 );
		}

		/*
		 * DTE Rate:
		 */

		if(ArgArray[1])
		{
			long *LDummy = (long *)ArgArray[1];
			DTERate = (*LDummy);
		}
		else
		{
			FreeArgs( rda );
			ShutDown( 2 );
		}

		/* 
		 * Connect message:
		 */

		if(ArgArray[2])
		{
			strcpy(ConnectMsg,(STRPTR)ArgArray[2]);
		}
		else
		{
			SPrintf(ConnectMsg,"CONNECT %ld",DTERate);
		}

		FreeArgs(rda);
	}
	else
	{
		ShutDown( 2 );
	}	

	/*
	 * Call the Server and tell him of the TrapDoor call:
	 */

	/*
	 * First create the reply port:
	 */

	if( ! (ServerReply = CreateMsgPort()))
	{
		ShutDown( 3 );
	}

	/*
	 * Allocate the message structure:
	 */

	if( ! (FAME_ServerMsg = (struct FAMEServerMsg *)AllocVec(sizeof(struct FAMEServerMsg),MEMF_PUBLIC | MEMF_CLEAR)))
	{
		ShutDown( 4 );
	}

	/* 
	 * Set the structure elements.
	 */

     FAME_ServerMsg -> Msg.mn_Length	= sizeof(struct FAMEServerMsg);
     FAME_ServerMsg -> Msg.mn_ReplyPort	= ServerReply;
	FAME_ServerMsg -> BaudCPS		= DTERate;
	FAME_ServerMsg -> Command		= SERVER_TrapDoor;
	FAME_ServerMsg -> Node			= -1;
	FAME_ServerMsg -> Data			= NodeNr;
	FAME_ServerMsg -> InStr			= ConnectMsg;
	FAME_ServerMsg -> StructDummy1	= FindTask(NULL);

	/* 
	 * Find the Server and place the message:
	 */

	Printf("Ok, FAMETrapDoor informes the Server!\n");

	Forbid();

	if(FAME_ServerPort = FindPort("FAME_Server"))
	{
  	PutMsg(FAME_ServerPort,(struct Message *)FAME_ServerMsg);
  }

	Permit();

	/*
	 * Was Port available?
	 */

	if( ! FAME_ServerPort)
	{
		ShutDown(5);
	}

	/*
	 * Wait for the reply:
	 */

  if(WaitPort(ServerReply))
	{
		if(FAME_ServerMsg = (struct FAMEServerMsg *)GetMsg((struct MsgPort *)ServerReply))
		{
			FreeVec(FAME_ServerMsg);
		}

		FAME_ServerMsg = NULL;
	}

	Printf("FAMETrapDoor waits for a CTRL_C to end!\n\n");

	/*
	 * Wait till someone sends a CTRL_C to end us (SIGBREAKF_CTRL_C):
	 */

	Wait(SIGBREAKF_CTRL_C);

	/*
	 * Finito:
	 */

	ShutDown( 0 );

} // void main( void );

/*
 * void SPrintf( char *Buffer, char *ctl, ... );
 *
 * SPrintf via RawDoFmt:
 *
 * Note that you have to uses long aligned values means instead of only
 * using %d you have to use %ld! Also %lc ...
 */

void SPrintf( char *Buffer, char *ctl, ... )
{
	RawDoFmt(ctl, (long *)(&ctl + 1), (void (*))"\x16\xc0\x4e\x75",Buffer);

} // void SPrintf( char *Buffer, char *ctl, ... );

/*
 * void ShutDown( long ErrCode );
 *
 * Clean up code.
 */

void ShutDown( long ErrCode )
{
	long TheIoErr = 0L;

	if(FAME_ServerMsg)
	{
		FreeVec(FAME_ServerMsg);
	}

	if(ServerReply)
	{
		while(FAME_ServerMsg = (struct FAMEServerMsg *)GetMsg((struct MsgPort *)ServerReply))
		{
			FreeVec(FAME_ServerMsg);
		}

		DeleteMsgPort((struct MsgPort *)ServerReply);
	}

	if(FAMEBase)
	{
		CloseLibrary((struct Library *)FAMEBase);
	}

	switch( ErrCode )
	{
		case 1:
			Printf("Can't open FAME.library 4!\n");

			TheIoErr	= ERROR_INVALID_RESIDENT_LIBRARY ;
			ErrCode		= RETURN_FAIL;
		break;
		case 2:
			Printf("Argument error!\n");

			TheIoErr	= ERROR_REQUIRED_ARG_MISSING ;
			ErrCode		= RETURN_ERROR;
		break;
		case 3:
			Printf("Failed to create reply port!\n");

			TheIoErr	= ERROR_NO_FREE_STORE;
			ErrCode		= RETURN_ERROR;
		break;
		case 4:
			Printf("Failed on allocating message structure!\n");

			TheIoErr	= ERROR_NO_FREE_STORE;
			ErrCode		= RETURN_ERROR;
		break;
		case 5:
			Printf("FAME Server not found!\n");

			TheIoErr	= ERROR_OBJECT_NOT_FOUND;
			ErrCode		= RETURN_WARN;
		break;
		default:
			Printf("FAMETrapDoor ended!\n");

			TheIoErr	= 0L;
			ErrCode		= RETURN_OK;
		break;
	}

	Printf("\n");

	SetIoErr( TheIoErr );

	PrintFault(IoErr(),"FAMETrapDoor");

	exit( ErrCode );

} // void ShutDown( long ErrCode );

