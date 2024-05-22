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
void del_diz(char *path, char *file);

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
               sprintf(buf,"\n\rRootDir : [1;4;31m%s[0m\n\r",fib->fib_FileName);
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
        printf("...exiting now ....");
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
        if(!(lock=Lock(name, SHARED_LOCK))) { printf("..can`t Lock SubDirs !\n\r"); return; }
        
        ok = Examine(lock, fib);
        if(!ok) { FreeMem(fib,sizeof(struct FileInfoBlock)); return; }
        else
        {       
               // ok=erstes Subdir gefunden, namen ausgeben           
               sprintf(buf,"[34mSubDir  : [4;37m %s [0m\n\r",fib->fib_FileName);
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
        char buf[256], fname[64];
        
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
               sprintf(buf,"\n\r[34mSubDir  : [4;32m %s [0m\n\r",fib->fib_FileName);
               printf(buf);

               while(ExNext(lock, fib))
               {                    
                    sprintf(buf,"[31m%12s - [36m%42s\n\r", fib->fib_FileName, fib->fib_Comment);
                    printf(buf);
                                       
                    sprintf(fname,"%s",fib->fib_FileName);
                    del_diz(name,fname);                   
               } 
               UnLock(lock);       
        }  
        FreeMem(fib,sizeof(struct FileInfoBlock));
        return;

}

void del_diz(char *path, char *file)
{
        char buf[256];
        
        sprintf(buf,"c:lha -q d %s/%s FILE_ID.DIZ",path,file);
        Execute(buf,0,0);
        
        sprintf(buf,"[35mDelete FILE_ID.DIZ of [32m%s/%s [35msuccessfull !\n\r\n\r",path,file);
        printf(buf);
        return;
        
}
















