#ifndef JULIA_SET_H
#define JULIA_SET_H
/**  julia_set.h **

@author Jacob Heard
@summary: Contains functions for building a Julia set fractal
		  column-by-column and writing these data points to a file.

**/
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#define BLOWUP   2
#define ITERS    50
#define STEP     50

extern uint32_t height, width; // Width and Height of canvas, defined in main file
const double TOOBIG = BLOWUP*BLOWUP; //Do this to avoid use of sqrt later on


/* Struct to hold a color (BGR) value */
#pragma pack(1)
typedef struct Color {
	unsigned char b,g,r; 
} Color;

/** 
 * Converts a uint32_t RGBA value to Color
 */
Color uint32_to_Color(uint32_t rgba) {
	Color rgb;
	rgb.r = (unsigned char) ((rgba & 0xFF0000) >> 16);
	rgb.g = (unsigned char) ((rgba & 0x00FF00) >> 8);
	rgb.b = (unsigned char)  (rgba & 0x0000FF);
	return rgb;
}

/**
 * Write a bitmap header and get the file descriptor.
 */
int writeBMPHeader(char* filename, unsigned int w, unsigned int h) {
	int fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC);
	if(fd < 0) // On file error
		return -1;
	unsigned int padding = (4 - ((w*sizeof(Color)) % 4)) % 4;
	unsigned int header_size = 54;
	unsigned int pixel_size = (sizeof(Color)*w+padding)*h;

	static unsigned char header[54] = {0x42,0x4D,0}; //rest is zeroes
	*(uint32_t*)&header[2] = header_size + pixel_size; // Complete file size
	*(uint32_t*)&header[10] = 54;   // Bytes before pixel data, e.g. pixel offset
	*(uint32_t*)&header[14] = 40;   // BITMAPINFOHEADER size
	*(uint32_t*)&header[18] = h;    // height
	*(uint32_t*)&header[22] = w;    // width

	*(uint16_t*)&header[26] = 1;  // Must be 1, # of color planes
	*(uint16_t*)&header[28] = 24; // BPP, 3 byte RGB for this purpose
	*(uint16_t*)&header[34] = pixel_size; // Size of raw pixel data

	*(uint32_t*)&header[38] = 2835; // Horizontal resolution
	*(uint32_t*)&header[42] = 2835; // Vertical resoluton
	
	write(fd, header, 54);
	return fd;
}

/* A basic structure to work with Complex numbers. */
typedef struct {
	double real, imag;
} Complex;

Complex f(Complex z0, Complex c) {
	/* The Julia set generator function, z0^2 + c
		@param z0 - The Complex point to start at
		@param c  - The constant to add
		@return z0^2 + c (Complex)	*/
	Complex tmp;
	tmp.real = z0.real*z0.real - z0.imag*z0.imag;
	tmp.imag = z0.imag*z0.real + z0.real*z0.imag;
	tmp.real += c.real;
	tmp.imag += c.imag;
	return tmp;
}

double norm(Complex c) {
	/* Returns the norm squared of a Complex value. The square root
		is left out to increase precision.
		@param c - The Complex number to find the norm of
		@return |c|^2 (double) */
	return c.real*c.real + c.imag*c.imag;
}

double LBOUND = -2.0f, UBOUND = 2.0f;

/**
 * Generates a given row of a Julia set.
 *	@param colorMap - The color mapping to use. Must have >= maxIters
 *					  elements (nullptr for black/white)
 *	@param seed     - The constant to be used as a seed for the Julia set
 *	@param maxIters - The number of points to calculate on the orbit
 *					  of a given point within the row
 *	@param dst      - The rows of the image to calculate
 *	@return An array of color values corresponding to the row of the
 *			Julia set 
*/
void get_data(Color *dst, uint32_t *colorMap, Complex seed, int maxIters, int row[2], Complex (*funct)(Complex, Complex)) {
	double y;
	Complex z;
	int num;
	for(int j = 0; j < row[0]-row[1]; j++) {
		y = minY + (double)((maxY-minY) / height) * (j+row[1]);
		//y = -2.0 + (j+row[1])*4.0/height;

		for(int i = 0; i < width; i ++) {
			z.real = minX + (double)((maxX-minX) / width) * i;
			//z.real = -2.0 + i*4.0/width;
			z.imag = y;
			for(num = 0; num < maxIters; num ++) {
				z = funct(z, seed);
				if (norm(z) > TOOBIG) break;
			}
			dst[j*width + i] = uint32_to_Color(colorMap[num]);
		}
	}
}

uint32_t COLORMAP[51] =     { 0xFFFFFF, 0xEEEEEE, 0xDDDDDD, 0xCCCCCC, 0xBBBBBB, 0xAAAAAA,
							  0x999999, 0x888888, 0x777777, 0x666666, 0x555555, 0x444444,
							  0x333333, 0x222222, 0x111111, 0x220000, 0x550000, 0x880000,
							  0xAA0000, 0xDD0000, 0xFF0000, 0x222200, 0x555500, 0x888800,
							  0xAAAA00, 0xDDDD00, 0xFFFF00, 0x002200, 0x005500, 0x008800,
							  0x00AA00, 0x00DD00, 0x00FF00, 0x002222, 0x005555, 0x008888,
							  0x00AAAA, 0x00DDDD, 0x00FFFF, 0x000022, 0x000055, 0x000088,
							  0x0000AA, 0x0000DD, 0x0000FF, 0x444444, 0x444444, 0x222222,
							  0x222222, 0x000000, 0x000000
							};
#endif // JULIA_SET_H
