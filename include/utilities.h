#ifndef UTILITIES_H
#define UTILITIES_H

#include <X11/Xlib.h>

void getCurrentTimeFormatted(char *buffer, size_t buffer_size);
void saveScreenshot(XImage *image, int width, int height, const char *filename);
void buttonPress(XEvent event, int *x1, int *y1, int *breakCondition, int *terminate);
void buttonRelease(Display *display, Window root, XEvent event, int x1, int y1,
                   int *x, int *y, int *width, int *height, int *breakCondition);
void keyPress(Display *display, Window root, XEvent event, int *x, int *y, int *width, int *height,
              int *breakCondition, int *terminate);

#endif // UTILITIES_H

