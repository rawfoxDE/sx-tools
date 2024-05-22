/*

----------------------------------------------------
      SXUserStatus 0.5 by [r]fox
----------------------------------------------------
3.12.97

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>

static char __version[] ="\0$VER: SXUserStatus V0.5beta3 ("__DATE__")";
struct MsgPort *bbsport;
struct SXUserStruct * SXUser;
struct UserIndexStruct * Uid;
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
void UStats(void);
void Flicker(int i);
void Flicker1(int x);


BOOL sx;

int main(int argc, char *argv[])
{
	char portname[16];

	if(argv[1][0]==0)
	{
		PutStr("This program requires System-X BBS Software\n\r\033[33mSupport Site http://sx.denet.co.jp\033[0m");
	} else {
		sprintf(portname, "AEDoorPort%s", argv[1]);
		bbsport = FindPort(portname);
		if(bbsport)
		{
			XIMFunction(1, 0, 0); 	/* function 1 = register */

			/* find out if we are under SYSTEM-X or AmiExpress */

			if(strcmp(themsg.String,"SX")==0) sx=TRUE; else sx=FALSE;

			UStats();
                        
                        PS("\033[17;51H"); 
                                                                    
                        XIMFunction(6,0," ");
                        if(themsg.String[0] == '?')
                        {
                                PS("\033[16;48H");
                                PS("\033[31m(\033[35mA\033[31m)\033[37m :\033[36m aBOUT");
                                PS("\033[17;48H");
                                PS("\033[31m(\033[35mG\033[31m)\033[37m :\033[36m gREETZ");
                                
                                XIMFunction(6,0," ");
                                if(themsg.String[0] == 'a')
                                {
                                                PS("\033[16;0H\033[K\033[1B\033[K\033[1A");
                                                PS("   \033[1;33;41m SXUserStatus v0.5beta3\033[36m done by [r]FoX house_bbs@hotmail.com \n\r\033[3C");
                                                         PS(" for SystemX by zED^dC  supported on http://sx.denet.co.jp   ");                                                                               
                                }
                                if(themsg.String[0] == 'g')      
                                {      
                                                 PS("\033[16;0H\033[K\033[1B\033[K\033[1A");
                                                 PS("   \033[1;36;41m gREETz sURELy tO zED fOR hIS pOWERFULL hELP, jOINIn C++ !   \n\r\033[3C");
                                                 PS(" eLSE for jUNGLe, uBIT, tRANTOr aND aLL dUDES on #elitecafe  ");                                                
                                              
                                }        
                                                                                         
                                   
                         }  

                         PS("\033[25;0H"); 
                         
                        
                        
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

void UStats(void)
{
        char buf[256];               // erstmal einige variabeln definieren
        
        static int x,i;
             
        PS("\033[2J");
	PS("\033[34m-============================-\033[36m[ \033[0;32;44mSXUserStatus 0.5\033[0m\033[36m ]\033[34m-==========================-\n\r");
        PS("\033[34m-\033[36m[ \033[33mcODE bY [r]FoX\033[36m ]\033[34m--------------------------------------------\033[36m[\033[33m aND zED^dC\033[36m ]\033[34m-\n\r");
        PS("\033[34m-============================================================================-\n\r");

        XIMFunction(11,0,0);
        sprintf(buf,"\033[36m     bOARDs nAME\033[36m :\033[31m %s\033[34m", themsg.String);
        PS(buf);
         
        XIMFunction(12,0,0);
        sprintf(buf,"\033[4;44H\033[36moP`s nAME\033[36m :\033[31m %s\n\r", themsg.String);
        PS(buf);
                       
        XIMFunction(1503,0,0);
        Uid = (struct UserIndexStruct *) themsg.Data;
        sprintf(buf,"          \033[32mhANDLE\033[37m : \033[33m%s\n\r", Uid -> handle);
        PS(buf);
        sprintf(buf,"        \033[32mrEALnAME\033[37m : \033[33m%s\n\r", Uid -> realname);
        PS(buf);
    
        XIMFunction(102,1,0);
        sprintf(buf,"\033[32m    gROUP/lOCATe\033[37m :\033[33m %s\n\r", themsg.String);
        PS(buf);
 
        XIMFunction(105,1,0);
        sprintf(buf,"      \033[5;37H\033[32m       aXx lEVEL\033[37m :\033[33m %s\n\r", themsg.String);
        PS(buf);
      
        XIMFunction(146,1,0);
        sprintf(buf,"\033[6;37H\033[32m  cONFERENCE aXx\033[37m :\033[33m %s\n\r", themsg.String);
        PS(buf);
    
        PS("\033[15;0H\033[34m-----------------------------------------------------------------------------\n\r");

        PS("\033[7;39H\033[32m     bYTErATIO\033[37m : \033[37m");
        PS("\033[8;39H\033[32m     fILErATIO\033[37m : \033[37m");

        XIMFunction(1501,0,0);
        User = (struct UserData *) themsg.Data;
        sprintf(buf,"\033[9;39H\033[32m  bYTES pUMPED\033[37m : \033[33m%ld", User -> Bytes_Upload);
        PS(buf);
        sprintf(buf,"\033[10;39H\033[32m  fILES pUMPED\033[37m : \033[33m%ld", User -> Uploads);
        PS(buf);
        sprintf(buf,"\033[11;39H\033[32m bYTES lEECH`d\033[37m : \033[33m%ld", User -> Bytes_Download);
        PS(buf);
        sprintf(buf,"\033[12;39H\033[32m fILES lEECH`d\033[37m : \033[33m%ld", User -> Downloads);
        PS(buf);
        sprintf(buf,"\033[9;0H\033[32m     tOTAL cALLS\033[37m :\033[33m %u", User -> Times_Called);
        PS(buf);
 
        XIMFunction(143,0,0);
        sprintf(buf,"\033[17;0H\033[32m         lAST oN\033[37m : \033[33m%s", themsg.String);
        PS(buf);
        sprintf(buf,"\033[10;0H\033[32m   tIME uSED nOW\033[37m : \033[33m%u \033[32mMins.", User -> Time_Used/60);
        PS(buf);
        sprintf(buf,"\033[11;0H\033[32m      tIME lIMIT\033[37m : \033[33m%u \033[32mMins.", User -> Time_Limit/60);
        PS(buf);
        sprintf(buf,"\033[12;0H\033[32m       tIME lEFT\033[37m : \033[33m%u \033[32mMins.", User -> Time_Left/60);
        PS(buf);
                
        PS("\033[14;7H\033[33mfREE bYTES\033[37m : ");
        PS("\033[14;39H\033[33m    fREE fILES\033[37m : ");

   
	PS("\033[13;0H\033[34m-----------------------------------------------------------------------------\n\r");

        PS("\033[18;0H\033[34m-=========================================================[\033[33m?\033[34m]===============-\n\r");

        XIMFunction(1502,0,0);
        SXUser = (struct SXUserStruct *) themsg.Data;
      
        sprintf(buf,"\033[8;0H\033[32m      yA mACHINE\033[37m : \033[33m%s", SXUser -> computer);
        PS(buf);
        sprintf(buf,"\033[16;0H\033[32m     sENTbY lINE\033[37m : \033[33m%s", SXUser -> sentbyline);
        PS(buf);

//----------------------------------------check-byteratio--------------------------------

        sprintf(buf,"%u", SXUser -> byteratio);
        if(SXUser -> byteratio != 0)               //if byteratio NOT 0 coz full normal..-
         {
           PS("\033[7;56H\033[35m1:");                      
           PS(buf);
                        
           sprintf(buf,"\033[14;19H \033[35m%u", SXUser -> freebytes);
           PS(buf);
           if(SXUser -> fileratio == 0) Flicker1(x);  
         
         }      
         
// byteratio ends ------------------------------------------------------------------------
//----------------------------------------check-fileratio --------------------------------
      
        sprintf(buf,"%u", SXUser -> fileratio);
        if(SXUser -> fileratio != 0)
         {
          PS("\033[8;56H\033[35m1:");
          PS(buf);
      
          sprintf(buf,"\033[14;57H\033[35m%u", SXUser -> freefiles);
          PS(buf);
         } 
                             
// fileratio ends -------------------------------------------------------------------------
   
         if(SXUser -> byteratio == 0) Flicker(i);
         
         else if(SXUser -> fileratio ==0) Flicker1(x);
 
}        
// Programm end ========================================================================== 
//=========================================================================================

void Flicker(int i)                   
{                        
                        
                        if(SXUser -> fileratio == 0)
                        {         
                                i=0; 
                                while(i<4)
                                        {                                     
                                        PS("\033[8;56H\033[0;33;41mdISABLEd !");
                                        PS("\033[14;56H\033[0;33;41muNLIMITeD !");
                                        Delay(2);
                                        PS("\033[8;56H\033[0;37mdISABLEd !");
                                        PS("\033[14;56H\033[0;37muNLIMITeD !");
                                        Delay(2);
                                        PS("\033[7;56H\033[0;33;41mdISABLEd !");
                                        PS("\033[14;20H\033[0;33;41mfREE LeECH !");
                                        Delay(2);
                                        PS("\033[7;56H\033[0;37mdISABLEd !");
                                        PS("\033[14;20H\033[0;37mfREE LeECH !");
                                        Delay(2);
                                        i++;
                                        }
                        } else                
                        {
                                i=0;
                                while(i<4)
                                {
                                        PS("\033[7;56H\033[0;33;41mdISABLEd !");
                                        PS("\033[14;20H\033[0;33;41mfREE LeECH !");
                                        Delay(2);
                                        PS("\033[7;56H\033[0;37mdISABLEd !");
                                        PS("\033[14;20H\033[0;37mfREE LeLCH !");
                                        Delay(2);
                                        i++;
                                }
                        }
       // free leech geflacker ende !
}

void Flicker1(int x)
{              
                x=0;     
                while(x<4)
                {       
                        PS("\033[8;56H\033[0;33;41mdiSABLEd !");
                        PS("\033[14;56H\033[0;33;41muNLIMITeD !");
                        Delay(2);
                        PS("\033[8;56H\033[0;37mdISABLEd !");
                        PS("\033[14;56H\033[0;37muNLIMITeD !");
                        Delay(2);
                        x++;
                }
                return;
                
}

           
           
           
           
           
           
        
        

