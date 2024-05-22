#include <stdio.h>
#include <string.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <stdlib.h>

void main(void)
{
   char buf[256], path[256];
   BPTR fh;

   sprintf(path,"ram:t/");

   sprintf(buf,"lha -k e #?.lha #?.readme %s", path);
   Execute(buf,0,0); 

   sprintf(buf, "%s/#?.readme", path);
   fh = Open(buf, MODE_OLDFILE);
  
   if(!fh) printf("\n\r[31mNo .README here !!");

   Read(fh, buf,

}

