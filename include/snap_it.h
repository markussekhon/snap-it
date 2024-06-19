#ifndef SNAP_IT_H
#define SNAP_IT_H

/*

typedef struct {
    int x, y, width, height;
} ScreenInfo;

*/

void saveScreenshot(XImage *image, int width, int height);

void buttonPress(XEvent event, int *x1, int *y1, int *breakCondition, int *terminate);

void buttonRelease(Display *display, Window root, XEvent event, int x1, int y1,
                   int *x, int *y, int *width, int *height, int *breakCondition);

void keyPress(Display *display, Window root, XEvent event, int *x, int *y, int *width, int *height,
              int *breakCondition, int *terminate);

void calculateXY(Display *display, Window root, int *x, int *y, int *width, int *height);

void takeScreenshot();


#endif // SNAP_IT_H
