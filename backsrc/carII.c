/*

----------------------------------------------------
This code was written by [r]FoX                     
cars for SystemX by zED^dC  Bulletin Board System   
----------------------------------------------------

last cghange: 24.4.98

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>

struct MsgPort *bbsport;     // der zeiger fier die von SX zur verfuegung gestellten Functionen

struct JHMessage             // die Message-Structure wird fuer die Daten gebraucht, die zwischen
{                            // dem User, der BBS (SX only!) und der Door-Sofware hin und her gehen
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;     // Zeiger auf themsg
struct UserData *User;
//struct FileHandle *fhpl;  // was war das noch ?
struct PlayerStruct
{
        char name[32];
         int races;
             
} Player;


void PS(char * str); // die Typer-Function fuer SX
void XIMFunction(int func, long data, char * str); // und fuer den XIM-Port von SX
void Door(int); // das Game
void Race(int); // raceroutine
void Auto1(void);// die Autos 
void Auto2(void);
void Auto3(void);
void Auto4(void);
void Auto5(void);
void Auto6(void);
void eins(void); // die countdownflagge vor dem Start
void zwei(void);
void drei(void);
void killnum(void);
void Selection(int);// die Carselection Page
void read_data(char *player);
void write_data(char *player, int races);
void topten(void);
int rnd(int);  // random routine
void cara(int); // die MOVE-Routinen...
void carb(int);
void carc(int);
void card(int);
void care(int);
void carf(int);

BOOL sx;

int main(int argc, char *argv[])
{
        int carflag;       
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
                        
                        carflag=1;
			Door(carflag);

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

                        /* ============ CARS MainDoor ============== 
                                                                                                     
                                                                                                     
                                                                                                   */

// This is the Mainloop, with all importand calls...                                                
//                                                   
void Door(int carflag)
{      
        char buf[128],player[32];         // player, playnum, win, lost, money
        int races;
               
        XIMFunction(1501,0,0);
        User=(struct UserData *) themsg.Data;
        sprintf(player,"%s",User->Name);       
                       
        read_data(player);    
         
                
   
keypress:
        PS("\033[2J");
	XIMFunction(8,0,"hd1:sx/developer/cars/car_mainmenu.ans");
        
        PS("\033[8;34H");   // type actual car at mainscreen
        switch(carflag)
        {
         case 1: { Auto1(); break; }
         case 2: { Auto2(); break; }
         case 3: { Auto3(); break; }
         case 4: { Auto4(); break; }
         case 5: { Auto5(); break; }
         case 6: { Auto6(); break; }
        }        
 
        PS("[11;33H");
        PS(player);
        
        races=Player.races; 
        PS("[12;33H");
        sprintf(buf,"%ld",races);
        PS(buf);
        
        //PS("[13;33H");
        //sprintf(buf,"%ld",Player.won);
        //PS(buf);

        //PS("[14;33H");
        //sprintf(buf,"%ld",Player.points);
        //PS(buf);

 
        PS("\033[21;2H");    
        XIMFunction(6,0,"\n\rYour Choose : "); // JH_HK
       // sprintf(buf,"%s",themsg.String);
   
        switch(themsg.String[0])
        {
                case 'a':
                case 'A':
                {
                        PS("\033[2J");
                        XIMFunction(8,0,"hd1:sx/developer/cars/car_doc.ans");
                        XIMFunction(6,0,"\n\rReturn to go back...: ");
                        goto keypress;
                }
                case 'c':
                case 'C':
                {
                        Selection(carflag);
                        
                       // Door(carflag);
                }
                case 'r':
                case 'R':
                case '\r':
                {
                        
                        PS("\033[2J");
                        Race(carflag);
                        races++;
                        write_data(player,races);
                        goto keypress;
                }
                case 't':
                case 'T':
                {
                        topten();
                                                
                        XIMFunction(6,0,"\n\rPress Return...");
                        goto keypress;
                }        
                case 'q':
                {                                        
                        return;
                }        
                                      
        }
        
        PS("\r\n\r\nEOF-->Exiting...\r\n\r\n");
        return;
}

/* Die Raceroutine regelt das Rennen selbst
   - die Autos         
   - den Sieg          
   - das gewaehlte Auto
                       
   ...bis jetzt ;)     
   
*/

void Race(int carflag)
{
        int camov=0,cbmov=0,ccmov=0;                  // cXmove = Carmoveindex -> wieviele Schritte schon ?
        int cdmov=0,cemov=0,cfmov=0;                  // t      = random start
        int t,z,won=0;                                // buf    = naja, um alles moegliche auszutypen eben..
                                                      // won    = 1 wenn ein Auto gewonnen hat
        char buf[256];                                // y      = counter for mainloop        
                                                      // z      = zufallszahl -> welches auto bewegen    
        PS("\033[2J");                                            
        XIMFunction(8,0,"sx:developer/cars/car_race.ans");    //   gfx
         
         PS("\033[1;61H"); 
        
        switch(carflag)
        {
         case 1: { Auto1(); break; }
         case 2: { Auto2(); break; }
         case 3: { Auto3(); break; }
         case 4: { Auto4(); break; }
         case 5: { Auto5(); break; }
         case 6: { Auto6(); break; }
        }        
 
         
        PS("\033[5;5H");                                      //  austypen der Autos vor dem Start
        Auto1();
        PS("\033[8;5H");
        Auto2();
        PS("\033[11;5H");
        Auto3();
        PS("\033[14;5H");
        Auto4();
        PS("\033[17;5H");
        Auto5();
        PS("\033[20;5H");
        Auto6();
        PS("\033[2B\r");   
                                    
	XIMFunction(6,0,"\n\rpress to R A C E ! ...(ret/q) \n\r");
        if(themsg.String[0]=='q') return;
                                        
        PS("\033[35m");
        PS("\033[10;30H");
        drei();
        Delay(35);
        PS("\033[10;30H");
        zwei();
        Delay(45);
        PS("\033[10;30H");
        eins();
        Delay(40);
        PS("G O O ooo..");
        PS("\033[10;30H");
        killnum();
        
        t=80;
        while(t)                                // der Block fuer die Startberechnung
        {
               XIMFunction(17,0,0);  
               if(themsg.Command == '\r') break;
               sprintf(buf,"\r%ld",t);
               PS(buf);
               t--;
               Delay(0);	
        } 
                                              // die Autos fahren los...
        
        while(won<1)                           //...mer fahren bis zum Sieg !
        {
                if(t==0) // Sicherheits Abfrage, sonst endlos Loop
                {
                        PS(" LOOOSER !! zu lahm ...");
                        Delay(50);
                        return;
                        
                }
                           
                z=rnd(t);
             
                if(z==1)                                                                        // AUTO 1   
                     
                {
                         camov++;
                         cara(camov);    
                         if(camov==30) won++; // camov schon 20 ?..gewonnwn ?
                }         
                                              
                if(z==2)                                                                         // AUTO 2  
                {
                         cbmov++;
                         carb(cbmov);
                         if(cbmov==30) won++;
                }
                
                if(z==3)                                                                        // AUTO 3  
                {
                         ccmov++;
                         carc(ccmov);
                         if(ccmov==30) won++;
                }
        
                if(z==4)                                                                        // AUTO 4  
                {
                        cdmov++;
                        card(cdmov);
                        if(cdmov==30) won++;
                }
        
                if(z==5)                                                                        // auTO 5  
                {
                        cemov++;
                        care(cemov);
                        if(cemov==30) won++;
                }
                
                if(z==6)                                                                        // AUTO 6  
                {
                        cfmov++;
                        carf(cfmov);
                        if(cfmov==30) won++;
                }        
        
        }
        PS("[0p"); 
        XIMFunction(6,0,"\n\rA Car has Won !!!...press a key..."); 
        
               return;
        //PS("\033[20;0H\r\nExiting...\r\n\r\n");
}

void Selection(int carflag)
{
       // char buf[16];
       // int carflag;   

        PS("\033[2J");
        
        XIMFunction(8,0,"sx:developer/cars/car_selection.ans");    //   gfx
         
        PS("\033[10;12H");       // cars auf der Select Seite austypen                                      
        Auto1();
        PS("\033[14;12H");
        Auto2();
        PS("\033[18;12H");
        Auto3();
        PS("\033[10;37H");
        Auto4();
        PS("\033[14;37H");
        Auto5();
        PS("\033[18;37H");
        Auto6();
Auswahl:        
        PS("\033[23;6H"); 
        
        switch(carflag)
        {
                case 1: { PS("\033[6;38H "); Auto1(); break; }
                case 2: { PS("\033[6;38H "); Auto2(); break; }
                case 3: { PS("\033[6;38H "); Auto3(); break; }
                case 4: { PS("\033[6;38H "); Auto4(); break; }
                case 5: { PS("\033[6;38H "); Auto5(); break; }
                case 6: { PS("\033[6;38H "); Auto6(); break; }
        }        
                                          
	XIMFunction(5,1,"\n\rSelect your Car : ");
  
        switch(themsg.String[0])
        {
                case 'a': { PS("\033[6;38H "); Auto1(); carflag=1; break; }
                case 'b': { PS("\033[6;38H "); Auto2(); carflag=2; break; }
                case 'c': { PS("\033[6;38H "); Auto3(); carflag=3; break; }
                case 'd': { PS("\033[6;38H "); Auto4(); carflag=4; break; }
                case 'e': { PS("\033[6;38H "); Auto5(); carflag=5; break; }
                case 'f': { PS("\033[6;38H "); Auto6(); carflag=6; break; }
        }                                                 
       
       PS("\033[25;1H\033[34m");
       XIMFunction(5,1,"ok, press M for Menu, get to (r)ace or select (a)gain...");
       
       switch(themsg.String[0])
       {
        case 'r': { Race(carflag); break; }
        case 'a': { goto Auswahl; }
        case '\r': { Door(carflag); }        
       }         
}        


void read_data(char *player)
{       
        BPTR fhpl;        
        char buf[256];
         int i=0;
                             
        fhpl = Open("sx:developer/cars/car_log", MODE_OLDFILE);
        if(fhpl==NULL) 
        {
                fhpl = Open("sx:developer/cars/car_log", MODE_NEWFILE);
        
                PS("\n\redit first Entry...");
                sprintf(Player.name,"%s",player);
                Player.races=0;

                Seek(fhpl, 0, OFFSET_BEGINNING);                                             
                Write(fhpl, &Player, sizeof(struct PlayerStruct));
                Close(fhpl);
        } else if(fhpl)
        {
                Seek(fhpl,0,OFFSET_BEGINNING);
                while(Read(fhpl, &Player, sizeof(struct PlayerStruct)))
                {
                        i++;
                        if(strcmpi(Player.name,player)==NULL || i>=10) { Close(fhpl); break; } 
                } 
                sprintf(buf,"\n\rPlayer num.: %ld \n\rname: %s \n\rraces: %ld",i,Player.name,Player.races);
                PS(buf);      
                if(strcmpi(Player.name,player) != NULL)
                {
                        PS("\n\rPlayername is not Player !");
                        if(fhpl)
                        {
                                                                
                                sprintf(buf,"\n\redit first Entry for %s ...",player);
                                PS(buf);
                                
                                sprintf(Player.name,"%s",player);
                                
                                Player.races=0;
                                
                                Seek(fhpl,sizeof(struct PlayerStruct),OFFSET_END);
                                Write(fhpl, &Player, sizeof(struct PlayerStruct));
                                Close(fhpl); 
                        }        
                }                  
         }                      
         PS("\r\nExiting reading...");
}

void write_data(char *player,int races)
{       
        BPTR fhpl;        
        
        char buf[256];         // player, playnum, win, lost, money
        int i=0;
        
        XIMFunction(6,0,"\n\rwrite data now ...");
                             
        fhpl = Open("sx:developer/cars/car_log", MODE_OLDFILE);
        if(fhpl)
        {                                
                Seek(fhpl,0,OFFSET_BEGINNING);
                do
                {
                        Read(fhpl, &Player, sizeof(struct PlayerStruct));
                        i++;
                        
                } while(strcmpi(Player.name,player)!=NULL || i>=10); 
                
                if(strcmpi(Player.name,player) != NULL)                 
                {
                        PS("\n\rPlayer not found ! maybe the Datafile is corrupt ?");
                        return;
                }        
                              
                sprintf(Player.name,"%s",player);
                Player.races=races;
                   
                sprintf(buf,"\n\rNAmE : %s \n\rRACeS : %ld",Player.name,Player.races);   
                PS(buf);
                           
                Seek(fhpl, -sizeof(struct PlayerStruct), OFFSET_CURRENT);
                Write(fhpl, &Player, sizeof(struct PlayerStruct));
                Close(fhpl);              
                        
         }                             
         PS("\r\n\r\nDATA has been written...\r\n\r\n");
}



void topten(void)
{
        
    char buf[256],oldname[32];
        
        BPTR fhpl;
 
        PS("\033[2J");
        XIMFunction(8,0,"sx:developer/cars/car_topten.ans");
        PS("[0;0H");
        PS("[6B");
        //oldname=="EMPTY";
     
        fhpl = Open("sx:developer/cars/car_log", MODE_OLDFILE);
        if(fhpl)
        {                        
                        
                Seek(fhpl,0,OFFSET_BEGINNING);
                while(Read(fhpl, &Player, sizeof(struct PlayerStruct)))
                {
                        if(strcmpi(oldname,Player.name)==0) { Close(fhpl);  break; }
                        strcpy(oldname,Player.name); 
                        sprintf(buf,"\r[2B[5C%-12s[18C%6ld",Player.name,Player.races);   
                        PS(buf);
                        
                }           
                Close(fhpl);              
                PS("[25;0H");        
         }        

}



int rnd(int t)
{
       	int i;
	if(t <= 0) return(0);
	do
        {
	       i = rand()%t;
	}
	while(i > t);
	return(i);
} 

void cara(int camov)   // CAR 1 Mover
{
        int q;
           
        PS("\033[5;5H");
        for(q=0; q<camov; q++)
        {
                PS("  ");
        }
        Auto1();
        //Delay(1);  // TESTPAUSE
}  

void carb(int cbmov)   // CAR 2 Mover
{
        int q;
        PS("\033[8;5H");
        for(q=0; q<cbmov; q++)
        {
              PS("  ");
        }       
        Auto2();     
        //Delay(1);  // TESTPAUSE
} 

void carc(int ccmov)   // CAR 3 Mover
{
        int q;
        PS("\033[11;5H");
        for(q=0; q<ccmov; q++)
        {
              PS("  "); 
        }       
        Auto3();           
        //Delay(1);  // TESTPAUSE
} 

void card(int cdmov)   // CAR 4 Mover
{
        int q;
        PS("\033[14;5H");
        for(q=0; q<cdmov; q++)
        {
              PS("  ");  
        }       
        Auto4();           
       // Delay(1);  // TESTPAUSE
} 

void care(int cemov)   // CAR 5 Mover
{
        int q;
        PS("\033[17;5H");
        for(q=0; q<cemov; q++)
        {
              PS("  ");
        }       
        Auto5();           
        //Delay(1);  // TESTPAUSE
} 

void carf(int cfmov)   // CAR 6 Mover
{
        int q;
        PS("\033[20;5H");
        for(q=0; q<cfmov; q++)
        {
              PS("  ");
        }       
        Auto6();           
        //Delay(1);  // TESTPAUSE
} 

void Auto1(void)
{
        PS("\033[32m .------\\");  // oberteil
        PS("\033[1B\033[10D");     // cursor pos
        PS("  |-o--o-'");          // unterteil
        PS("\033[1A\033[10D ");    // cursor pos
}

void Auto2(void)
{
        PS("\033[36m  .___,-.");
        PS("\033[1B\033[10D");
        PS("  `-o--o-'");
        PS("\033[1A\033[10D ");
}
  
void Auto3(void)
{
        PS("\033[37m  ----.. ");
        PS("\033[1B\033[10D");
        PS("  `-O--o-'");
        PS("\033[1A\033[10D ");       
}      

void Auto4(void)
{
        PS("\033[31m  ____.. ");
        PS("\033[1B\033[10D");
        PS("  `-o--o-\\");
        PS("\033[1A\033[10D ");       
}

void Auto5(void)
{
        PS("\033[35m  -__,._ ");
        PS("\033[1B\033[10D");
        PS("  `-O--O-\\");
        PS("\033[1A\033[10D ");       
}

void Auto6(void)
{
        PS("\033[34m  ____\\. ");
        PS("\033[1B\033[10D");
        PS("  `-o--o-|");
        PS("\033[1A\033[10D ");    
}

void eins(void)
{
        PS("   ###[1B[6D");
        PS("  ####[1B[6D");        
        PS(" ## ##[1B[6D");
        PS("    ##[1B[6D");
        PS("    ##[1B[6D");
        PS("    ##[1B[6D");
}

void zwei(void)
{
        PS(" #####[1B[6D");
        PS("##  ##[1B[6D");        
        PS("   ## [1B[6D");
        PS("  ##  [1B[6D");
        PS(" ##   [1B[6D");
        PS("######[1B[6D");
}


void drei(void)
{
        PS("##### [1B[6D");
        PS("    ##[1B[6D");        
        PS("   ###[1B[6D");
        PS("  ### [1B[6D");
        PS("    ##[1B[6D");
        PS("######[1B[6D");
}

void killnum(void)
{
        PS("      [1B[6D");
        PS("      [1B[6D");        
        PS("      [1B[6D");
        PS("      [1B[6D");
        PS("      [1B[6D");
        PS("      [1B[6D");
}

