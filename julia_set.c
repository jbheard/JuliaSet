/**  julia_set.cpp  **

@author Jacob Heard

Summary: Serial program for Julia set fractal generation. Uses same algorithm as parallel
			implementation by generating a given number of rows at a time.

Created on   : 02/26/2016
Last updated : 12/17/2016
**/
#include <stdio.h>  //cout, cerr
#include <stdint.h> //uint32_t
#include <stdlib.h> //atoi
#include <time.h>   //clock_t

uint32_t height, width;

#include "julia_set.h"

int main(int argc, char* argv[])
{
	double a,b;         // C = a + bi
	int row[2];         // The rows being worked on

	if(argc < 4)
	{
		printf("Usage: %s Output_Filename a b [height] [width] [color map]\n", argv[0]);
		printf("       Output_Filename : The name of the binary file to output\n");
		printf("       a : The real portion of the constant a + bi, \n");
		printf("       b : The imaginary portion of the constant a + bi, \n");
		printf("       width: The width of the image in pixels,          auto=1000\n");
		printf("       height: The height of the image in pixels,        auto=1000\n");
		printf("       color map: The color mapping file to use,         auto=RGB_LIGHT\n");
		return 0;
	}

	a = atof(argv[2]);
	b = atof(argv[3]);
	Complex C; 
	C.real = a, C.imag = b;
	
	width = (argc >= 5) ? atoi(argv[4]) : 1000;
	height = (argc >= 6) ? atoi(argv[5]) : 1000;

	if(argc >= 7)
	{
		FILE *cmap = fopen(argv[6], "rb");
		if(cmap == NULL) {
			printf("Error reading color mapping, using default.\n");
		} else {
			fread(COLORMAP, sizeof *COLORMAP, 51, cmap);
			fclose(cmap);
		}
	}
	
	int fd = writeBMPHeader(argv[1], height, width);
	if(fd < 0) {
		fprintf(stderr, "Error opening file, aborting.\n");
		fflush(stderr);
		return -1;
	}
	
	Color *row_data = malloc(width*STEP* sizeof *row_data); // Array for pixel data
	char zeroes[3] = {0,0,0}; //for padding
	int padding = (4-((width*sizeof(Color))%4))%4;
	int progress, d_prog, step; // Progress, delta progress, number of rows completed
	
	row[0] = height;
	row[1] = height < STEP ? 0 : height-STEP;

	while(row[0] > 0) { // Continue reading until we have done all of the rows
		d_prog = progress;
		progress = 100 - (100*row[1] / height);
		
		if(progress - d_prog >= 10)
			fprintf(stderr, "Working... %d%% done.\n", progress); // Display progress every ~10%
		
		/* Receive the byte offset and column, build the current column of the Julia set
			and then write it to the file. */
		get_data(row_data, COLORMAP, C, ITERS, row, f);
		
		for(int i = row[0]-row[1]-1; i >= 0; i--) { // Write bottom rows first as bitmaps are backwards
			write(fd, row_data+width*i, width*sizeof *row_data);
			write(fd, zeroes, padding);
		}
		
		// Get number of rows that have been done
		step = row[0]-row[1] < 0 ? row[1]-row[0] : row[0]-row[1];
		row[0] = row[1];
		row[1] -= step;
	}
	
	free(row_data);
	close(fd);
	return 0;
}
