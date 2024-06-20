#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "snap_it.h"

/*

typedef struct {
	int x, y, width, height;
} ScreenInfo;

*/

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

void buttonPress(XEvent event, int *x1, int *y1, int *breakCondition, int *terminate) {
	
	switch(event.xbutton.button){
		case Button1:
			*x1 = event.xbutton.x;
			*y1 = event.xbutton.y;
			break;

		case Button3:
			*breakCondition = 1;
			*terminate = 1;
			break;
	}
}

void buttonRelease(Display *display, Window root, XEvent event, int x1, int y1,
		   int *x, int *y, int *width, int *height, int *breakCondition) {

	switch(event.xbutton.button) {
		case Button1:
			int x2 = event.xbutton.x;
			int y2 = event.xbutton.y;
		
			if (x1 == x2 && y1 == y2) {
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
			} else {
				*width = abs(x1-x2);
				*height = abs(y1-y2);
				*x = x1 < x2 ? x1 : x2;
				*y = y1 < y2 ? y1 : y2;
			}
			*breakCondition = 1;
			break;
	}
}

void keyPress(Display *display, Window root, XEvent event, int *x, int *y, int *width, int *height, 
								int *breakCondition, int *terminate) {
	
	KeySym key = XLookupKeysym(&event.xkey, 0);

	switch(key){
		case XK_Escape:
			*breakCondition = 1;
			*terminate = 1;
			break;

		case XK_0 ... XK_9:
			int screenNumber = key - (XK_1);

			XRRScreenResources *screenResources = XRRGetScreenResources(display, root);
			RRCrtc crtc = screenResources->crtcs[screenNumber];
			XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(display, screenResources, crtc);

			*x = crtcInfo->x;
			*y = crtcInfo->y;
			*width = crtcInfo->width;
			*height = crtcInfo->height;

			XRRFreeCrtcInfo(crtcInfo);
			
			*breakCondition = 1;
			break;
	}
}
 
void calculateXY(Display *display, Window root, int *x, int *y, int *width, int *height) {
	int x1 = 0, y1 = 0, breakCondition = 0, terminate = 0;

	XGrabPointer(display, root, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
						GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

	XGrabKeyboard(display, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	while(!breakCondition){
		XEvent event;
		XNextEvent(display, &event);

		if(event.type == ButtonPress){
			buttonPress(event, &x1, &y1, &breakCondition, &terminate);
		} else if(event.type == ButtonRelease){
			buttonRelease(display, root, event, x1, y1, x, y, width, height, &breakCondition);
		} else if(event.type == KeyPress){
			keyPress(display, root, event, x, y, width, height, &breakCondition, &terminate);
		}
	}

	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);

	if(terminate) {
		XCloseDisplay(display);
		exit(0);
	}

}

int main(int argc, char *argv[]) {

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

	return 0;
}
