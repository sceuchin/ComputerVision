/*
 * Hough Transform
 * Input files: box_canny.pgm
 * Output files: box_hough.pgm (Hough transform of input image), 
 * 				box_lines.pgm (Image with detected edges as white lines)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES
#define MAX_NUMLINES 7 //Maximum number of lines to detect

int main(void) {

    //Declare variables
    FILE *box, *hough;
    unsigned int i, j, row=448, col=229, m, p;
    unsigned char img, header;
    int d, **H, max, line_max, numlines, *line_d, notline;
    float theta, *line_theta;

	
	//Prepare image file for reading
	//File obtained after using Canny edge detection 
	box = fopen("box_canny.pgm", "rb");

	//Prepare image file for writing
	hough = fopen("box_hough.pgm", "w");

	//Skip header
	for(i=0; i<15; i++)
		fscanf(box, "%c", &header);

	//Initialize H[d,theta]=0
	H = (int **) calloc(1450,sizeof(int *));
	for(i=0; i<1450; i++)
		H[i] = (int *) calloc(180*5,sizeof(int));

	//Compute d for -90<=theta<90 for every edge pixel
	for(i=0; i<row; i++)
		for(j=0; j<col; j++)
		{
			fscanf(box, "%c", &img);
			if(img)	//Edge pixel
				for(theta=-90; theta<90; theta+=0.2)
				{
					d = round(j*cos(theta*M_PI/180) + i*sin(theta*M_PI/180));
					//Increment array
					H[d+725][(int)((theta+90)*5)]++;
				}
		}

	//Reset max value
	max = 0;
	//Insert header
	fprintf(hough,"P5 900 1450 255 ");

	//Write Hough transform image onto file
	for(i=0; i<1450; i++)
		for(j=0; j<180*5; j++)
		{
			//Find maximum value
			if(H[i][j]>max)	max = H[i][j];

			//Hough transform is scaled to increase visibility
			if(H[i][j]*255/20>255)	fprintf(hough,"%c",255);
			else					fprintf(hough,"%c",H[i][j]*255/20);
		}

	fclose(box);
	fclose(hough);

	//Initialize arrays to store properties of detected lines
	line_d = (int *) calloc(MAX_NUMLINES,sizeof(int));
	line_theta = (float *) calloc(MAX_NUMLINES,sizeof(float));

	//Initialize number of detected lines = 0
	numlines = 0;

	//Display & store detected lines
	printf("\nFor box.pgm, detected lines:\n");
	line_max = max;
	//Consider only sufficiently distinct lines
	while(line_max > 0.1*max && numlines < MAX_NUMLINES)
	{
		for(i=0; i<1450; i++)
			for(j=0; j<180*5; j++)
				if(H[i][j]==line_max)
				{
					//Compare with lines already detected
					d = 9999; theta = 999;
					for(p=0; p<numlines; p++)
					{
						//Find smallest difference in line parameter d
						if(abs(abs(i-725)-abs(line_d[p]))<d)
							d = abs(abs(i-725)-abs(line_d[p]));
						//Find smallest difference in line parameter theta
						if(fabs((float)j/5-90-line_theta[p])<theta)
							theta = fabs((float)j/5-90-line_theta[p]);
					}
					//Use line if not similar to those already detected
					if(d>10 || (theta>2 && fabs(fabs((float)j/5-90)-90)>2))
					{
						//Display detected line
						printf("x cos(%.1f) + y sin(%.1f) = %d\n",
							(float)j/5-90,(float)j/5-90,i-725);
						//Store parameters of detected line
						line_d[numlines] = i-725;
						line_theta[numlines] = (float)j/5-90;
						numlines++;
					}
				}
		line_max--;
	}

	//Prepare image files for reading & writing
	box = fopen("box.pgm", "rb");
	hough = fopen("box_lines.pgm", "wb");
	p = 71;

	//Copy header
	for(i=0; i<p; i++)
		{	fscanf(box, "%c", &header);
			fprintf(hough, "%c", header); }

	for(i=0; i<row; i++)
		for(j=0; j<col; j++)
		{
			notline = 1;
			fscanf(box, "%c", &img);
			for(p=0; p<numlines; p++)
			{
				//Draw detected lines on grey-scale image
				if(round(j*cos(line_theta[p]*M_PI/180) +
					i*sin(line_theta[p]*M_PI/180)) == line_d[p]
					&& notline == 1)
				{	fprintf(hough, "%c", 255); notline = 0;	}
			}
			//If pixel is not part of detected line, copy image
			if(notline)	fprintf(hough, "%c", img);
		}

	fclose(box);
	fclose(hough);

	free(H);
	free(line_d);
	free(line_theta);
	
}