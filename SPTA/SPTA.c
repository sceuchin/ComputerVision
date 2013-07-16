/*
 * Safe-Point Thinning Algorithm (SPTA)
 * Reference: Naccache, N.J.; Shinghal, Rajjan, "SPTA: A proposed algorithm for thinning binary patterns," Systems, Man and Cybernetics, IEEE Transactions on , vol.SMC-14, no.3, pp.409,418, May-June 1984
 * URL: http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6313233&isnumber=6313228
 * Input file: letter.e 
 * Output file: letter_spta.e
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#define _USE_MATH_DEFINES
#define ROW 36
#define COL 52

int spta1(char **e, int lbl);
int spta2(char **e, int lbl);

int main(void) {

    //Declare variables
    FILE *letter, *letter_spta;
    unsigned int i, j, row=ROW, col=COL;
    char **e, pix, endline=0;
    int flags1, flags2, lbl;

	//Initialize array to store image pixels to zeros
    e = (char **) calloc(row,sizeof(char *));
	for(i=0; i<ROW; i++)
		e[i] = (char *) calloc(col,sizeof(char));

	//Input letter pixels into array
	//0 for dark points, -CHAR_MAX for white points
	letter = fopen("letter.e", "rb");
	for(j=0; j<COL; j++)
		e[0][j] = -CHAR_MAX;	//Pad top row with white points
    for(i=1; i<ROW; i++)
    {
    	endline = 0;
    	for(j=0; j<COL; j++)
		{
			if(endline==0)
			{
				fscanf(letter,"%c",&pix);
				if(pix==0x78)			e[i][j] = 0;	//Dark points
				else if(pix==0x0A)	{ 	e[i][j] = -CHAR_MAX; endline = 1; }
				else					e[i][j] = -CHAR_MAX;
			}
			else
				e[i][j] = -CHAR_MAX;
		}
	}
	fclose(letter);

	lbl = 0;
	do{
		lbl++;
		if(flags1)	flags1 = spta1(e, lbl);	//Left and right SPTA
		if(flags2)	flags2 = spta2(e, lbl);	//Top and bottom SPTA
		printf("lbl = %d\tflags1 = %d\tflags2 = %d\n",lbl,flags1,flags2);
	} while(flags1 || flags2);	//Check if there are more to be flagged

	//Output thinned image onto file
	letter_spta = fopen("letter_spta.e", "wb");
	for(i=1; i<ROW; i++)
	{
		for(j=0; j<COL; j++)
			if(e[i][j]>0)	fprintf(letter_spta,"x");
			else			fprintf(letter_spta," ");
		fprintf(letter_spta,"\n");
	}
	fclose(letter_spta);

    free(e);
}

//Left and right SPTA
int spta1(char **e, int lbl)
{
	int i,j,flags=0;

	for(i=1; i<ROW-1; i++)
		for(j=1; j<COL-1; j++)
		{
			if(e[i][j]>=0)		//Dark and unflagged
			{
				if(e[i][j-1]<(lbl-CHAR_MAX) || e[i][j+1]<(lbl-CHAR_MAX))	//Edge-point
				{
					//Left SP
					if(e[i][j+1]>=0 && (e[i-1][j+1]>=0 || e[i-1][j]>=0 || e[i+1][j]>=0 || e[i+1][j+1]>=0)
						&& (e[i-1][j]>=0 || e[i-1][j-1]<(lbl-CHAR_MAX)) && (e[i+1][j-1]<(lbl-CHAR_MAX) || e[i+1][j]>=0))
							{ 	e[i][j] = lbl - CHAR_MAX; flags++;	}	//Not a left SP, Flag
					//Right SP
					else if(e[i][j-1]>=0 && (e[i-1][j]>=0 || e[i-1][j-1]>=0 || e[i+1][j-1]>=0 || e[i+1][j]>=0)
						&& (e[i+1][j]>=0 || e[i+1][j+1]<(lbl-CHAR_MAX)) && (e[i-1][j+1]<(lbl-CHAR_MAX) || e[i-1][j]>=0))
							{ 	e[i][j] = lbl - CHAR_MAX; flags++;	}	//Not a right SP, Flag
					else 	{	if(!e[i][j])	e[i][j] = lbl;		}	//Label SP if not yet labeled
				}
			}
		}
	return flags;
}

//Top & bottom SPTA
int spta2(char **e, int lbl)
{
	int i,j,flags=0;

	for(i=1; i<ROW-1; i++)
		for(j=1; j<COL-1; j++)
		{
			if(e[i][j]>=0)		//Dark and unflagged
			{
				if(e[i-1][j]<(lbl-CHAR_MAX) || e[i+1][j]<(lbl-CHAR_MAX))	//Edge-point
				{
					//Top SP
					if(e[i+1][j]>=0 && (e[i][j+1]>=0 || e[i][j-1]>=0 || e[i+1][j-1]>=0 || e[i+1][j+1]>=0)
						&& (e[i][j+1]>=0 || e[i-1][j+1]<(lbl-CHAR_MAX)) && (e[i-1][j-1]<(lbl-CHAR_MAX) || e[i][j-1]>=0))
							{ 	e[i][j] = lbl - CHAR_MAX; flags++;	}	//Not a top SP, Flag
					//Bottom SP
					else if(e[i-1][j]>=0 && (e[i][j+1]>=0 || e[i-1][j+1]>=0 || e[i-1][j-1]>=0 || e[i][j-1]>=0)
						&& (e[i][j-1]>=0 || e[i+1][j-1]<(lbl-CHAR_MAX)) && (e[i][j+1]>=0 || e[i+1][j+1]<(lbl-CHAR_MAX)))
						 	{ 	e[i][j] = lbl - CHAR_MAX; flags++;	}	//Not a bottom SP, Flag
					else	{	if(!e[i][j])	e[i][j] = lbl; 		}	//Label SP if not yet labeled
				}
			}
		}
	return flags;
}