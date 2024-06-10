#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "snap_it.h"

void takeScreenshot() {

	Display *display;
	int screen, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	Window root;
	XImage *image;

	display = XOpenDisplay(NULL);

	if (display == NULL){
		fprintf(stderr, "Issue creating a connection to the X server.\n");
		exit(1);
	}

	screen = DefaultScreen(display);
	root = RootWindow(display, screen);

	XGrabPointer(display, root, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
			GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	XGrabKeyboard(display, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	while(1){
		XEvent event;
		XNextEvent(display, &event);	

		if(event.type == ButtonPress && event.xbutton.button == Button1){
			x1 = event.xbutton.x;
			y1 = event.xbutton.y;
		}

		else if(event.type == ButtonRelease && event.xbutton.button == Button1){
			x2 = event.xbutton.x;
			y2 = event.xbutton.y;
			break;
		}

		//esc key doesnt work here for some reason
		else if(event.xkey.keycode == 9 || event.xbutton.button == Button3){
			XUngrabPointer(display, CurrentTime);
			XUngrabKeyboard(display, CurrentTime);
			XCloseDisplay(display);
			exit(0);
		}

	}

	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);


	printf("1(X,Y):(%d,%d)\n", x1, y1);
	printf("2(X,Y):(%d,%d)\n", x2, y2);

	// this solution only considers 2 styles of box making when there are in fact 4
	// more elegant solution to come

	x2 -= x1;
	y2 -= y1;

	image = XGetImage(display, root, x1, y1, x2, y2, AllPlanes, ZPixmap);

	if (image == NULL) {
		fprintf(stderr, "Unable to capture image.\n");
		XCloseDisplay(display);
		exit(1);
	}

	printf("image captured\n");

	FILE *fp = fopen("screenshot.ppm", "wb");
	
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file.\n");
		exit(1);
	}

	
	fprintf(fp, "P6\n%d %d\n255\n", x2, y2);

    	for (int y = 0; y < y2; y++) {
        	for (int x = 0; x < x2; x++) {
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
	/*
	if (argc > 2 || strncmp(argv[1], "snap", 4) != 0){
		fprintf(stderr, "issues with argument passed\n");
		return 1;
	}
	*/

	takeScreenshot();

	return 0;
}
