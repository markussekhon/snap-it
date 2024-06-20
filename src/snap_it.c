#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "utilities.h"
#include "snap_it.h"
 
void calculateRegion(Display *display, Window root, ScreenInfo *info) {
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
			buttonRelease(display, root, event, x1, y1, info, &breakCondition);
		} else if(event.type == KeyPress){
			keyPress(display, root, event, info, &breakCondition, &terminate);
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
	//int x = 0, y = 0, width = 0, height = 0;

	Display *display = XOpenDisplay(NULL);

	if(display == NULL){
		fprintf(stderr, "Issue creating a connection to the X server.\n");
		exit(1);
	}

	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);


	ScreenInfo info;
	calculateRegion(display, root, &info);

	XImage *image = XGetImage(display, root, info.x, info.y, info.width, info.height, 
			   AllPlanes, ZPixmap);

	if(image == NULL){
		fprintf(stderr, "Unable to capture image.\n");
		XCloseDisplay(display);
		exit(1);
	}

	char screenshotTime[50];
	getCurrentTimeFormatted(screenshotTime, sizeof(screenshotTime));

	char filename[100];
	snprintf(filename, sizeof(filename), "screenshot_%s.png", screenshotTime);

	saveScreenshot(image, info.width, info.height, filename);

	XDestroyImage(image);
	XCloseDisplay(display);

	return 0;
}
