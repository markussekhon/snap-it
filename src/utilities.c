#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "stb_image_write.h"
#include "utilities.h"

void getCurrentTimeFormatted(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, buffer_size, "%Y-%m-%d_%H-%M-%S", t);
}

void saveScreenshot(XImage *image, int width, int height, const char *filename) {
    unsigned char *data = malloc(width * height * 3);
    if (!data) {
        fprintf(stderr, "Failed to allocate memory for image data\n");
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned long pixel = XGetPixel(image, x, y);
            int offset = (y * width + x) * 3;
            data[offset + 0] = (pixel & image->red_mask) >> 16;
            data[offset + 1] = (pixel & image->green_mask) >> 8;
            data[offset + 2] = (pixel & image->blue_mask);
        }
    }

    stbi_write_png(filename, width, height, 3, data, width * 3);
    free(data);
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

