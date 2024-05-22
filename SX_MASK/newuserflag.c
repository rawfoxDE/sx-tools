fh=Open("sx:prefs/access.dat",MODE_OLDFILE);
if(fh)
{
Read(fh,&AccSet,sizeof(struct AccessStruct));
Close(fh);
newuser=AccSet.access;
}
then check for newuser...

