/*
 * Otsu's Thresholding Method
 * Input file: text.pgm
 * Output file: text_bin.pgm
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {

    //Declare variables
    FILE *txt, *txt_bin;
    unsigned int i, j, row=161, col=330, total;
    unsigned char img[row][col], header, T, min_T=255, max_T=0;
    unsigned int hist[256];
    double meanL_T, meanH_T=0, meanL_T1, meanH_T1;
    double var, max=0, nT, nL_T, nH_T, nL_T1, nH_T1;

    //Prepare files for reading & writing
    txt = fopen("text.pgm", "rb");
    txt_bin = fopen("text_bin.pgm","wb");

    //Initialize histogram to zeros
	for (i = 0; i < 256; i++)
	    hist[i] = 0;

	//Copy header
    for (i = 0; i < 15; i++)
    {
		fscanf(txt, "%c", &header);
		fprintf(txt_bin, "%c", header);
    }

    //Compute histogram
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			fscanf(txt, "%c", &img[i][j]);
			hist[img[i][j]]++;						//Find image histogram values
			if(img[i][j]<min_T)	min_T = img[i][j];	//Find min image grey-level
			if(img[i][j]>max_T)	max_T = img[i][j];	//Find max image grey-level
			meanH_T += img[i][j];					//Find sum of grey-levels to get mean
		}

	total = row*col;
	//Initialize n and mean when threshold,T = 0
	nL_T = 0;
	nH_T = 1;
	meanL_T = 0;
	meanH_T /= total;

	for (i = min_T; i <= max_T; i++)
	{
		//T = i-1
		var = nL_T*nH_T*(meanL_T-meanH_T)*(meanL_T-meanH_T);
		if(var > max)	{	max = var; T = i-1;	}

		nT = (double)hist[i]/total;
		nL_T1 = nL_T + nT;
		nH_T1 = nH_T - nT;
		meanL_T1 = (meanL_T*nL_T + nT*i)/nL_T1;
		meanH_T1 = (meanH_T*nH_T - nT*i)/nH_T1;
		if(i==max_T)	meanH_T1 = 0;	//No more pixels > i, thus mean = 0

		//Substitute new values when T = i
		nL_T = nL_T1;
		nH_T = nH_T1;
		meanL_T = meanL_T1;
		meanH_T = meanH_T1;
	}

	//T = maximum image grey-level
	var = nL_T*nH_T*(meanL_T-meanH_T)*(meanL_T-meanH_T);
	if(var > max)	{	max = var; T = max_T;	}

	//Display optimal threshold value found
	printf("Threshold = %d\n", T);

	//Threshold image
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
		{
			if(img[i][j]>T)	fprintf(txt_bin, "%c", 255);
			else			fprintf(txt_bin, "%c", 0);
		}

	fclose(txt);
	fclose(txt_bin);
}