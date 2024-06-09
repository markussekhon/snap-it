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
	
	Window window = XCreateSimpleWindow(display, root, 0, 0, width, height, 0, 0, 0);

	XSelectInput(display, window, KeyPressMask | ButtonPressMask | ButtonReleaseMask);

	int x1,x2,y1,y2;

	printf("outside penor\n");

	XMapWindow(display, window);

	while(1){
		XEvent event;
		
		printf("inbetween penor\n");
		XNextEvent(display, &event);
		printf("unknown location penor\n");

		if(event.type == ButtonPress && event.xbutton.button == Button1){
			x1 = event.xbutton.x;
			y1 = event.xbutton.y;
		}

		else if(event.type == ButtonRelease && event.xbutton.button == Button1){
			x2 = event.xbutton.x;
			y2 = event.xbutton.y;
			break;
		}

		else if(event.xkey.keycode == 27 || event.xbutton.button == Button3){
			exit(0);
		}
		printf("inside penor\n");
	}


	printf("1(X,Y):(%d,%d)\n", x1, y1);
	printf("2(X,Y):(%d,%d)\n", x2, y2);
	

	XImage *image = XGetImage(display, root, x1, y1, x2, y2, AllPlanes, ZPixmap);

	if (image == NULL) {
		fprintf(stderr, "Unable to capture image.\n");
		exit(1);
	}

	printf("image captured\n");

	/*
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

	*/

	XDestroyImage(image);
	XCloseDisplay(display);

	printf("Screenshot saved as screenshot.ppm\n");


}

int main(int argc, char *argv[]) {
	/*
	if (argc > 2 || strncmp(argv[1], "snap", 4) != 0){
		fprintf(stderr, "issues with argument passed\n");
		return 1;
	}
	*/

	takeScreenshot();

	return 0;
}
