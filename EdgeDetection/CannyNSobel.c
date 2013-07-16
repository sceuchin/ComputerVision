/*
 * Canny & Sobel Edge Detection
 * Input file: box.pgm (229 x 448)
 * Output files: box_canny.pgm, box_sobel.pgm 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES

#define TS	45
#define TH	50
#define TL	25

int main(void) {

    //Declare variables
    FILE *box, *box_sobel, *box_canny;
    unsigned int i, j, magnitude;
    unsigned int row = 448, col = 229;
    unsigned char **img, **out, **mag, **gnh, **gnl;
    float **dir, dx, dy;
    char header;
    unsigned char y_m1_m1, y_m1_0, y_m1_p1, y_0_m1, y_0_0, y_0_p1, y_p1_m1, y_p1_0, y_p1_p1;

    //Request OS to prepare files for reading & writing
    box = fopen("box.pgm", "r");
    box_canny = fopen("box_canny.pgm","w");
    box_sobel = fopen("box_sobel.pgm","w");

	out  = (unsigned char **) calloc(row,sizeof(char *));
	for(i=0; i<row; i++)
	    out[i]  = (unsigned char *) calloc(col,sizeof(char));

	img  = (unsigned char **) calloc(row,sizeof(char *));
	for(i=0; i<row; i++)
	    img[i]  = (unsigned char *) calloc(col,sizeof(char));

	//Copy header
	for (i = 0; i < 15; i++){
		fscanf(box, "%c", &header);
		fprintf(box_canny, "%c", header);
		fprintf(box_sobel, "%c", header);
	}

	//Obtain pixel values
	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++){
			fscanf(box, "%c", &img[i][j]);
		}
	}

	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++){

			//Assign block to be filtered
			y_m1_m1 = img[i-1][j-1];	y_m1_0 = img[i-1][j];	y_m1_p1 = img[i-1][j+1];
			y_0_m1 = img[i][j-1];		y_0_0 = img[i][j];		y_0_p1 = img[i][j+1];
			y_p1_m1 = img[i+1][j-1];	y_p1_0 = img[i+1][j];	y_p1_p1 = img[i+1][j+1];
			//Copy edges
			if (i == 0) 		{y_m1_m1 = img[i][j-1];	y_m1_0 = img[i][j];	y_m1_p1 = img[i][j+1];}
			if (j == 0)			{y_m1_m1 = img[i-1][j];	y_0_m1 = img[i][j]; y_p1_m1 = img[i+1][j];}
			if (i == (row-1))	{y_p1_m1 = img[i][j-1];	y_p1_0 = img[i][j];	y_p1_p1 = img[i][j+1];}
			if (j == (col-1))	{y_m1_p1 = img[i-1][j];	y_0_p1 = img[i][j];	y_p1_p1 = img[i+1][j];}
			//Copy corners
			if (i == 0 && j == 0)				y_m1_m1 = img[i][j];
			if (i == 0 && j == (col-1))			y_m1_p1 = img[i][j];
			if (i == (row-1) && j == 0)			y_p1_m1 = img[i][j];
			if (i == (row-1) && j == (col-1))	y_p1_p1 = img[i][j];

			//Gaussian filter
			out[i][j]=(y_m1_m1+2*y_m1_0+y_m1_p1+2*y_0_m1+4*y_0_0+2*y_0_p1+y_p1_m1+2*y_p1_0+y_p1_p1)/16;

			//Sobel Operator
			dx = -y_m1_m1 + y_m1_p1 - 2*y_0_m1 + 2*y_0_p1 - y_p1_m1 + y_p1_p1;
			dy = y_m1_m1 + 2*y_m1_0 + y_m1_p1 - y_p1_m1 - 2*y_p1_0 - y_p1_p1;
			magnitude = floor(sqrt(dx*dx + dy*dy));
			if (magnitude > TS) fprintf(box_sobel, "%c", 255);
			else				fprintf(box_sobel, "%c", 0);
		}
	}

	free(img);

	dir  = (float **) calloc(row,sizeof(float *));
	for(i=0; i<row; i++)
	    dir[i]  = (float *) calloc(col,sizeof(float));

	mag  = (unsigned char **) calloc(row,sizeof(char *));
	for(i=0; i<row; i++)
	    mag[i]  = (unsigned char *) calloc(col,sizeof(char));

	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++){

			//Assign block to be operated on
			y_m1_m1 = out[i-1][j-1];	y_m1_0 = out[i-1][j];	y_m1_p1 = out[i-1][j+1];
			y_0_m1 = out[i][j-1];		y_0_0 = out[i][j];		y_0_p1 = out[i][j+1];
			y_p1_m1 = out[i+1][j-1];	y_p1_0 = out[i+1][j];	y_p1_p1 = out[i+1][j+1];
			//Copy edges
			if (i == 0) 		{y_m1_m1 = out[i][j-1];	y_m1_0 = out[i][j];	y_m1_p1 = out[i][j+1];}
			if (j == 0)			{y_m1_m1 = out[i-1][j];	y_0_m1 = out[i][j]; y_p1_m1 = out[i+1][j];}
			if (i == (row-1))	{y_p1_m1 = out[i][j-1];	y_p1_0 = out[i][j];	y_p1_p1 = out[i][j+1];}
			if (j == (col-1))	{y_m1_p1 = out[i-1][j];	y_0_p1 = out[i][j];	y_p1_p1 = out[i+1][j];}
			//Copy corners
			if (i == 0 && j == 0)				y_m1_m1 = out[i][j];
			if (i == 0 && j == (col-1))			y_m1_p1 = out[i][j];
			if (i == (row-1) && j == 0)			y_p1_m1 = out[i][j];
			if (i == (row-1) && j == (col-1))	y_p1_p1 = out[i][j];

			//Sobel operator
			dx = -y_m1_m1 + y_m1_p1 - 2*y_0_m1 + 2*y_0_p1 - y_p1_m1 + y_p1_p1;
			dy = y_m1_m1 + 2*y_m1_0 + y_m1_p1 - y_p1_m1 - 2*y_p1_0 - y_p1_p1;
			mag[i][j] = floor(sqrt(dx*dx + dy*dy));
			dir[i][j] = atan(dy/dx);
			if (dx == 0)	dir[i][j] = M_PI/2;
		}
	}

	free(out);

	gnh  = (unsigned char **) calloc(row,sizeof(char *));
	gnl  = (unsigned char **) calloc(row,sizeof(char *));
	for(i=0; i<row; i++)
	{
	    gnh[i]  = (unsigned char *) calloc(col,sizeof(char));
	    gnl[i]  = (unsigned char *) calloc(col,sizeof(char));
	}

	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++){

			//Assign block to be filtered
			y_m1_m1 = mag[i-1][j-1];	y_m1_0 = mag[i-1][j];	y_m1_p1 = mag[i-1][j+1];
			y_0_m1 = mag[i][j-1];		y_0_0 = mag[i][j];		y_0_p1 = mag[i][j+1];
			y_p1_m1 = mag[i+1][j-1];	y_p1_0 = mag[i+1][j];	y_p1_p1 = mag[i+1][j+1];
			//Copy edges
			if (i == 0) 		{y_m1_m1 = mag[i][j-1];	y_m1_0 = mag[i][j];	y_m1_p1 = mag[i][j+1];}
			if (j == 0)			{y_m1_m1 = mag[i-1][j];	y_0_m1 = mag[i][j]; y_p1_m1 = mag[i+1][j];}
			if (i == (row-1))	{y_p1_m1 = mag[i][j-1];	y_p1_0 = mag[i][j];	y_p1_p1 = mag[i][j+1];}
			if (j == (col-1))	{y_m1_p1 = mag[i-1][j];	y_0_p1 = mag[i][j];	y_p1_p1 = mag[i+1][j];}
			//Copy corners
			if (i == 0 && j == 0)				y_m1_m1 = mag[i][j];
			if (i == 0 && j == (col-1))			y_m1_p1 = mag[i][j];
			if (i == (row-1) && j == 0)			y_p1_m1 = mag[i][j];
			if (i == (row-1) && j == (col-1))	y_p1_p1 = mag[i][j];

			//Non-maxima Suppression
			if 		((dir[i][j] > 3*M_PI/8 || dir[i][j] <= -3*M_PI/8)
				&& (y_0_0<y_m1_0 || y_0_0<y_p1_0))	//horizontal edge
					gnh[i][j] = gnl[i][j] = 0;
			else if ((dir[i][j] > -3*M_PI/8 && dir[i][j] <= -M_PI/8)
				&& (y_0_0<y_m1_m1 || y_0_0<y_p1_p1))	//+45 edge
					gnh[i][j] = gnl[i][j] = 0;
			else if ((dir[i][j] > -M_PI/8 && dir[i][j] <= M_PI/8)
				&& (y_0_0<y_0_m1 || y_0_0<y_0_p1))	//vertical edge
					gnh[i][j] = gnl[i][j] = 0;
			else if ((dir[i][j] > M_PI/8 && dir[i][j] <= 3*M_PI/8)
				&& (y_0_0<y_m1_p1 || y_0_0<y_p1_m1))	//-45 edge
					gnh[i][j] = gnl[i][j] = 0;
			else
				{
					if (y_0_0>=TH)					{ gnh[i][j] = 1; gnl[i][j] = 0; }
					else if (y_0_0<TH && y_0_0>=TL)	{ gnh[i][j] = 0; gnl[i][j] = 1; }
					else							{ gnh[i][j] = gnl[i][j] = 0;	}
				}
		}
	}

	free(dir);
	free(mag);

	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++){

			//Assign block to be operated on
			y_m1_m1 = gnh[i-1][j-1];	y_m1_0 = gnh[i-1][j];	y_m1_p1 = gnh[i-1][j+1];
			y_0_m1 = gnh[i][j-1];		y_0_0 = gnh[i][j];		y_0_p1 = gnh[i][j+1];
			y_p1_m1 = gnh[i+1][j-1];	y_p1_0 = gnh[i+1][j];	y_p1_p1 = gnh[i+1][j+1];
			//Copy edges
			if (i == 0) 		{y_m1_m1 = gnh[i][j-1];	y_m1_0 = gnh[i][j];	y_m1_p1 = gnh[i][j+1];}
			if (j == 0)			{y_m1_m1 = gnh[i-1][j];	y_0_m1 = gnh[i][j]; y_p1_m1 = gnh[i+1][j];}
			if (i == (row-1))	{y_p1_m1 = gnh[i][j-1];	y_p1_0 = gnh[i][j];	y_p1_p1 = gnh[i][j+1];}
			if (j == (col-1))	{y_m1_p1 = gnh[i-1][j];	y_0_p1 = gnh[i][j];	y_p1_p1 = gnh[i+1][j];}
			//Copy corners
			if (i == 0 && j == 0)				y_m1_m1 = gnh[i][j];
			if (i == 0 && j == (col-1))			y_m1_p1 = gnh[i][j];
			if (i == (row-1) && j == 0)			y_p1_m1 = gnh[i][j];
			if (i == (row-1) && j == (col-1))	y_p1_p1 = gnh[i][j];

			//Hysteresis thresholding
			if (y_0_0)
				fprintf(box_canny, "%c", 255);
			else
            	if (gnl[i][j] && (y_m1_m1 || y_m1_0 || y_m1_p1 || y_0_m1
            			|| y_0_p1 || y_p1_m1 || y_p1_0 || y_p1_p1))
				{
					gnh[i][j] = 1;
					fprintf(box_canny, "%c", 255);
				}
				else
					fprintf(box_canny, "%c", 0);
		}
	}

	free(gnh);
	free(gnl);

	//Close files
	fclose(box);
	fclose(box_canny);
	fclose(box_sobel);

	return (EXIT_SUCCESS);

}
