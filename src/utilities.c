#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include "stb_image_write.h"
#include "utilities.h"

void createDirectory(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

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
                   ScreenInfo *info, int *breakCondition) {
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

                info->x = windowAttributes.x;
                info->y = windowAttributes.y;
                info->width = windowAttributes.width;
                info->height = windowAttributes.height;
            } else {
                info->width = abs(x1-x2);
                info->height = abs(y1-y2);
                info->x = x1 < x2 ? x1 : x2;
                info->y = y1 < y2 ? y1 : y2;
            }
            *breakCondition = 1;
            break;
    }
}

void keyPress(Display *display, Window root, XEvent event, ScreenInfo *info, 
              int *breakCondition, int *terminate) {
    KeySym key = XLookupKeysym(&event.xkey, 0);

    switch(key){
        case XK_Escape:
            *breakCondition = 1;
            *terminate = 1;
            break;

        case XK_0 ... XK_9:
            //this makes it so user can assume 1 indexing instead of 0
            int screenNumber = key - (XK_1);

            XRRScreenResources *screenResources = XRRGetScreenResources(display, root);
            RRCrtc crtc = screenResources->crtcs[screenNumber];
            XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(display, screenResources, crtc);

            info->x = crtcInfo->x;
            info->y = crtcInfo->y;
            info->width = crtcInfo->width;
            info->height = crtcInfo->height;

            XRRFreeCrtcInfo(crtcInfo);
            
            *breakCondition = 1;
            break;
    }
}

