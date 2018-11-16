#pragma once

#define IMSIZE 16384

struct Image {
	Image(float *pixels, int width, int height) : pixels(pixels), width(width), height(height) {}

	float *pixels;
	int width;
	int height;
};

// Functions
Image loadImage(const char *fileName);
void readLowResulutionImage(float *pixels);
void saveAsBMP(const char *fileName);