#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "snap_it.h"

void saveScreenshot(XImage *image, int width, int height) {
	FILE *fp = fopen("screenshot.ppm", "wb");
	
	if(fp == NULL){
		fprintf(stderr, "Cannot open file.\n");
		exit(1);
	}

	fprintf(fp, "P6\n%d %d\n255\n", width, height);

    	for(int y = 0; y < height; y++){
        	for (int x = 0; x < width; x++) {
            	unsigned long pixel = XGetPixel(image, x, y);
            	fputc((pixel & image->red_mask) >> 16, fp);
            	fputc((pixel & image->green_mask) >> 8, fp);
            	fputc(pixel & image->blue_mask, fp);
		}
	}

	fclose(fp);
}

void calculateXY(Display *display, Window root, int *x, int *y, int *width, int *height) {

	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

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

			if(x1 == x2 && y1 == y2){
				Window parent, child;
				int rx, ry, wx, wy;
				unsigned int mr;
				XWindowAttributes windowAttributes;

				XQueryPointer(display, root, &parent, &child, 
							&rx, &ry, &wx, &wy, &mr);

				XGetWindowAttributes(display, child, &windowAttributes);
			
				*x = windowAttributes.x;
				*y = windowAttributes.y;
				*width = windowAttributes.width;
				*height = windowAttributes.height;
			} else{
				*width = abs(x2-x1);
				*height = abs(y2-y1);
				*x = x1 < x2 ? x1 : x2;
				*y = y1 < y2 ? y1 : y2;
			}

			break;
		}

		else if(event.xkey.keycode == 9 || event.xbutton.button == Button3){
			XUngrabPointer(display, CurrentTime);
			XUngrabKeyboard(display, CurrentTime);
			XCloseDisplay(display);
			exit(0);
		}

	}

	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);

}

void takeScreenshot() {

	Display *display;
	int screen, x = 0, y = 0, width = 0, height = 0;
	Window root;
	XImage *image;

	display = XOpenDisplay(NULL);

	if(display == NULL){
		fprintf(stderr, "Issue creating a connection to the X server.\n");
		exit(1);
	}

	screen = DefaultScreen(display);
	root = RootWindow(display, screen);

	calculateXY(display, root, &x, &y, &width, &height);

	image = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap);

	if(image == NULL){
		fprintf(stderr, "Unable to capture image.\n");
		XCloseDisplay(display);
		exit(1);
	}

	saveScreenshot(image, width, height);

	XDestroyImage(image);
	XCloseDisplay(display);

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
