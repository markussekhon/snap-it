#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "snap_it.h"

void takeScreenshot() {
	// placeholder
	
	Display *display = XOpenDisplay(NULL);

	if (display == NULL){
		fprintf(stderr, "Issue creating a connection to the X server.\n");
		exit(1);
	}

	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);
	XWindowAttributes window_attributes;
	XGetWindowAttributes(display, root, &window_attributes);
	int width = window_attributes.width;
	int height = window_attributes.height;

	printf("(X,Y):(%d,%d)\n", width, height);

	XImage *image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

	if (image == NULL) {
		fprintf(stderr, "Unable to capture image.\n");
		exit(1);
	}

	FILE *fp = fopen("screenshot.ppm", "wb");
	
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file.\n");
		exit(1);
	}

	
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

    	for (int y = 0; y < height; y++) {
        	for (int x = 0; x < width; x++) {
            	unsigned long pixel = XGetPixel(image, x, y);
            	fputc((pixel & image->red_mask) >> 16, fp);
            	fputc((pixel & image->green_mask) >> 8, fp);
            	fputc(pixel & image->blue_mask, fp);
		}
	}

	fclose(fp);

	XDestroyImage(image);
	XCloseDisplay(display);

	printf("Screenshot saved as screenshot.ppm\n");


}

int main(int argc, char *argv[]) {
	
	if (argc > 2 || strncmp(argv[1], "snap", 4) != 0){
		fprintf(stderr, "issues with argument passed\n");
		return 1;
	}

	takeScreenshot();

	return 0;
}
