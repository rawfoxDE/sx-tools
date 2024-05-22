/* Convertion d'une serie de pt qui est a la precision maximum a une */
/* serie de points qui ne garde que les point de precision 5		*/

#include <exec/types.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

tst_end(
	FILE	*input
	)
{
	char c;
	int	code_ret;

	/* lecture du prochain caractere */
	do
    {
		c=getc(input);
		if(c==EOF)return(TRUE);
		if(c=='#')
        {
			do
            {
				c=getc(input);
			}
              while ((c!=0x0a)&&(c!=EOF));
			c=getc(input);
		}

	}
      while  ((!isalnum(c))&&(c!='E'));
	if  ((!isalpha(c))||(c!='E'))
    {
		ungetc(c,input);
		code_ret=FALSE;
	}
    else
    {
		code_ret=TRUE;
	}
	return(code_ret);
}

void convert_map(
	char *p_filename,
	char *out_filename
	)
{
	FILE	*input;
	FILE	*output;
	int	x, y,code;
	int	minx,miny,maxx,maxy;
	char	c1,c2,c;
	BOOL	flg_end,flg_border,flg_zone;
	int	num_border,num_pt;

	printf("Lecture de %s\n",p_filename);
	flg_end=FALSE;

	input = fopen(p_filename,"r");
	output = fopen(out_filename,"w");
	if(input && output )
    {


		/* Premiere chose, la taille max de notre carte...*/

		fscanf( input ,"%ld%ld%ld%ld"  ,&minx,&miny,&maxx,&maxy);
		minx=-18000;
		maxx= 18000;
		miny=-9000;
		maxy= 9000;
		fprintf(output,"%ld %ld %ld %ld\n", minx, miny, maxx, maxy);
		printf("minx:%ld miny:%ld maxx:%ld maxy:%ld\n",minx,miny,maxx,maxy);


		printf("Lecture des borders\n");
		flg_zone=TRUE;
		num_border=0;
	
		do
        {
			flg_border=TRUE;
			num_pt=0;
			do
            {
				c1=getc(input);
				c2=getc(input);
				if (c2=='#')
                {

					do
                    {
						c=getc(input);
					}
                      while ((c!=0x0a)&&(c!=EOF));
					flg_border=FALSE;

				}
                else
                {
					ungetc(c2,input);
					ungetc(c1,input);
				
					fscanf(input,"%ld %ld %ld",&code,&x,&y);
					if ((code>=5)||(num_pt==0))
                    {
						fprintf(output," %ld %ld %ld",code,x,y);
						num_pt++;
						if(num_pt>30)
                        {
							fprintf(output,"\n");
							num_pt=0;
						}
					}

				}
			}
              while(flg_border==TRUE);

		
			/* On s'assure que le dernier point est bien de niveau 5 */
			if (code<5)
            {
				fprintf(output," 5 %ld %ld",x,y);
			}
			fprintf(output," #%d\n",num_border);
			num_border++;

			if (tst_end(input)==TRUE)
            {
				printf("End of border def\n");
				flg_zone=FALSE;
			}

		}
          while (flg_zone==TRUE);
		fprintf(output,"E\n");
		fclose(input);
		fclose(output);
	}
}

main(int	argc,char **argv)
{
	if(argc!=3)
    {
		printf("Conversion e fichier .data\nUsage convert_map <fichier source> <fichier dest>\n");
	}
    else
    {
		convert_map(argv[1],argv[2]);
	}
}
