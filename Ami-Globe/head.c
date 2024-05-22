#include <exec/types.h>
#include <stdlib.h>
#include <stdio.h>

main(int argc, char **argv)
/* copie les argv[3] premières lignes d'un fichier texte argv[1] dans argv[2]*/
{
    FILE *fp1,*fp2;
	int	nb,cont;
	char	c;
	int	flg_end;
	fp1=fopen(argv[1],"r");
	fp2=fopen(argv[2],"w");
	nb=atoi(argv[3]);
	printf("Nombre de lignes:%d\n",nb);
	cont=0;
	flg_end=FALSE;
	if (fp1 && fp2) 
    {
	    while (flg_end == FALSE)
        {
		    c=getc(fp1);
		    if (c==0x0a) /* si c= retour a la ligne */
            {
			    cont++;
			    if (cont>nb) 
                    flg_end=TRUE;
	        }
            else 
            if (c==EOF)
            {
			    flg_end=TRUE;
	        }
		    putc(c,fp2);
        }
        fclose(fp1);
	    fclose(fp2);
	}
}          
