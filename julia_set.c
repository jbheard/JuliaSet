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
#include <unistd.h>
#include <getopt.h>

uint32_t height, width;
double minX, minY, maxX, maxY;

#include "julia_set.h"

void usage(const char *name) {
	printf("Usage: %s Output_Filename [-a Creal] [-b Cimag] [-w width] [-h height] [--minX minX] [--minY minY] [--maxX maxX] [--maxY maxY] [--map ColorMap]\n", name);
	printf("       Output_Filename : The name of the bitmap file to output\n");
	printf("       -a : The real portion of the constant C = a + bi, \n");
	printf("       -b : The imaginary portion of the constant C = a + bi, \n");
	printf("       -w : The width of the image in pixels. NOTE: affects image scale if w =/= h\n");
	printf("       -h : The height of the image in pixels. NOTE: affects image scale if w =/= h\n");
	printf("       --minX : The lower bound of the x-axis\n");
	printf("       --minY : The lower bound of the y-axis\n");
	printf("       --maxX : The upper bound of the x-axis\n");
	printf("       --maxY : The upper bound of the y-axis\n");
	printf("       --map: The color mapping file to use\n");
	printf("\n");
}


int main(int argc, char* argv[])
{
	if(argc < 2) {
		usage(argv[0]);
		return 1;
	}
	
	int row[2];		// The rows being worked on
	Complex C;		// The complex variable
	char zeroes[3] = {0,0,0}; //for padding
	int padding;	// Amount of padding for rows in file
	int progress, d_prog, step; // Progress, delta progress, number of rows completed
	Color *row_data; // Array for pixel data
	FILE *cmap;		// Color mapping file, for --map argument

	char ch, *err; // option character and error check
	int opt_index = 0; // Long option index
	static struct option options[] = // long options
	{
		/* These options don’t set a flag.
		 We distinguish them by their indices. */
		{"minY", required_argument, 0, 1},
		{"maxY", required_argument, 0, 2},
		{"minX", required_argument, 0, 3},
		{"maxX", required_argument, 0, 4},
		{"map", required_argument, 0, 5},
		{0, 0, 0, 0}
	};
	
	/* Set some defaults */
	C.real = 0.0f;
	C.imag = 0.0f;
	width = 1000;
	height = 1000;

	/* Start argument loop */
	optind = 2; // Start getopt at 2, don't parse the file name
	while( (ch = getopt_long(argc, argv, "a:b:w:h:", options, &opt_index)) != -1 ) {
		switch(ch) {
			case 1:	// minY
				minY = strtod(optarg, &err);
				break;
			case 2:	// maxY
				maxY = strtod(optarg, &err);
				break;
			case 3:	// minX
				minX = strtod(optarg, &err);
				break;
			case 4:	// maxX
				maxX = strtod(optarg, &err);
				break;
			case 5:	// map
				cmap = fopen(argv[6], "rb");
				if(cmap == NULL) {
					printf("Error reading color mapping, using default.\n");
				} else {
					fread(COLORMAP, sizeof *COLORMAP, 51, cmap);
					fclose(cmap);
				}
				break;
			
			case 'a': // C real
				C.real = strtod(optarg, &err);
				break;
			case 'b': // C imaginary
				C.imag = strtod(optarg, &err);
				break;
			case 'w':
				width = strtol(optarg, &err, 10);
				break;
			case 'h':
				height = strtol(optarg, &err, 10);
				break;
			case '?':
				break;
		}
		if(*err != '\0') {
			printf("Error: Invalid argument provided.\n");
			return 1;
		}
	}
		
	int fd = writeBMPHeader(argv[1], height, width);
	if(fd < 0) {
		fprintf(stderr, "Error opening file, aborting.\n");
		fflush(stderr);
		return 1;
	}

	
	row_data = malloc(width*STEP* sizeof *row_data); // Array for pixel data
	padding = (4-((width*sizeof(Color))%4))%4;
	
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
