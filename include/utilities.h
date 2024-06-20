#ifndef UTILITIES_H
#define UTILITIES_H

#include <X11/Xlib.h>

typedef struct {
   int x, y, width, height;
} ScreenInfo;

void getCurrentTimeFormatted(char *buffer, size_t buffer_size);
void saveScreenshot(XImage *image, int width, int height, const char *filename);
void buttonPress(XEvent event, int *x1, int *y1, int *breakCondition, int *terminate);
void buttonRelease(Display *display, Window root, XEvent event, int x1, int y1, 
                   ScreenInfo *info, int *breakCondition);
void keyPress(Display *display, Window root, XEvent event, ScreenInfo *info, 
              int *breakCondition, int *terminate);
void createDirectory(const char *path);

#endif // UTILITIES_H

