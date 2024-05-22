#include <libraries/dos.h>
#include <exec/memory.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <fcntl.h>

void tree(void);
void sub(char *name);
void files(char *name);
void create_diz(char *comment);
void add_diz(char *path, char *file);

main(int argc, char *argv[])
{
        tree();
}        
      
void tree(void)  // declare the root: -dir
{
                
        struct FileInfoBlock *fib = AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC | MEMF_CLEAR);
        
        BPTR lock;
        char buf[256],name[256], oldname[256];
        
        BOOL ok;
        
        sprintf(name,"aminet:");
              
        if(!fib) goto xit;    
        if(!(lock=Lock(name, SHARED_LOCK))) { printf("..can`t Lock anything !"); goto xit; }
        
        ok = Examine(lock, fib);
        if(!ok)  // if no root: was found
        {
           goto xit;
        }
        else
        {      // if root: is examinable so print the name           
               sprintf(buf,"\n\rActual Dir : [1;4;31m%s[0m",fib->fib_FileName);
               printf(buf);

               while(ExNext(lock, fib))  // subdirectories im rootdir ?
               {               
                         if(fib->fib_DirEntryType > -1) // wenn ja, dann namen merkn
                         {                              // und neuen namen an sub(name) weitergeben
                              strcpy(oldname,name);
                              sprintf(name,"%s%s", oldname, fib->fib_FileName);
                              sub(name);
                              strcpy(name,oldname);      // go parent
                         } else
                         {    
                              // wenn kein dir/device dann is ein File                              
                              sprintf(buf,"[32mFILE : [36m%s\n\r", fib->fib_FileName);
                              printf(buf);
                         }
               } 

               UnLock(lock);
       
        }  
xit:
        printf("\r\n...exiting now ....\r\n");
        FreeMem(fib,sizeof(struct FileInfoBlock));
        exit(0);

}
void sub(char *name)
{
        struct FileInfoBlock *fib = AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC | MEMF_CLEAR);
        
        BPTR lock;
        char buf[256], oldname[256];
        
        BOOL ok;
                  
        if(!fib) return;    
        if(!(lock=Lock(name, SHARED_LOCK))) { printf("..can`t Lock SubDirs !\r"); return; }
        
        ok = Examine(lock, fib);
        if(!ok) { FreeMem(fib,sizeof(struct FileInfoBlock)); return; }
        else
        {       
               // ok=erstes Subdir gefunden, namen ausgeben           
               sprintf(buf,"[34m - [1;37m%s\r\n",fib->fib_FileName);
               printf(buf);

               while(ExNext(lock, fib))
               {             
                         // hat subdir weitere subdir`s ?                  
                         if(fib->fib_DirEntryType > -1)  
                         {
                              strcpy(oldname,name); // neuen namen vorbereiten und an files(name)
                              sprintf(name,"%s/%s", oldname, fib->fib_FileName); // weitergeben   
                              files(name);
                              strcpy(name,oldname); // files() hat ende, wieder weiter...
                              
                         } else  
                         {
                              if(fib->fib_DirEntryType==NULL) printf("  [35mNo New Files here !\n\r");
                              strcpy(name,oldname);  
                              sprintf(buf,"[34m%s  ", fib->fib_FileName);
                              printf(buf);                              
                                                                       
                         }
               } 

               UnLock(lock);
       
        }  
        FreeMem(fib,sizeof(struct FileInfoBlock));
        return;

}
void files(char *name)
{
        struct FileInfoBlock *fib = AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC | MEMF_CLEAR);
        
        BPTR lock;
        char buf[256], fname[64], comment[80];
        
        BOOL ok;
                                      
        if(!fib) return;    
        if(!(lock=Lock(name, SHARED_LOCK))) { printf("..can`t Lock any Files !\n\r"); return; }
        
        ok = Examine(lock, fib);
        if(!ok) 
        { 
                printf("[32m No New Files here !\n\r");          
                UnLock(lock);
                FreeMem(fib,sizeof(struct FileInfoBlock)); 
                return;                
        }
        else
        {                 
               sprintf(buf,"[34m - [32m%s\n\r",fib->fib_FileName);
               printf(buf);

               while(ExNext(lock, fib))
               {                    
                    sprintf(buf,"[31m%12s - [36m%42s\n\r", fib->fib_FileName, fib->fib_Comment);
                    printf(buf);
                    
                    sprintf(buf,"c:copy s:ComDizzer.header to ram:FILE_ID.DIZ");
                    Execute(buf,0,0);
                    
                    sprintf(comment," %s\r\n",fib->fib_Comment); // must have a `return` coz AmBoS diz-port
                    create_diz(comment);                    
                    sprintf(fname,"%s",fib->fib_FileName);
                    add_diz(name,fname);
                     
                    
               } 
               UnLock(lock);       
        }  
        FreeMem(fib,sizeof(struct FileInfoBlock));
        return;

}
void create_diz(char *comment)
{        
        BPTR fh;
        
        fh = Open("ram:FILE_ID.DIZ", MODE_OLDFILE);
        if(!fh)
        {
               printf("\n\rCannot Open the File !");
        }                      
        else
        {
               Seek(fh,0,OFFSET_END);
               Write(fh, comment, strlen(comment));
        }
        Close(fh);
                                      
}

void add_diz(char *path, char *file)
{
        char buf[256];
        
        sprintf(buf,"c:lha -q a %s/%s ram:FILE_ID.DIZ",path,file);
        Execute(buf,0,0);
        
        sprintf(buf,"[35mDiz added to FILE: [32m%s/%s \n\r",path,file);
        printf(buf);
        return;
        
}
















