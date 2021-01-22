// Hello world in XWindows
// Date : 10 January 2021
// By : Darshan Vikam

// General Header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

// XWindows specific header files
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

// Namespaces
using namespace std;

// Global variable declaration
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;
char key;

// Entry point function
int main() {
	// Function declaration
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void Uninitialize();

	// Variable declaration
	int winWidth, winHeight;
	XEvent event;
	KeySym keysym;
	static XFontStruct *pXFontStruct = NULL; 
	static GC gc;		// software Graphic Context of H/W
	XGCValues gcValues;
	XColor greenColor;
	char str[] = "Hello XWindows!!! Darshan here...";
	int stringLength, stringWidth, fontHeight;

	// Code
	CreateWindow();
	winWidth = giWindowWidth;
	winHeight = giWindowHeight;

	// Message loop
	while(1) {
		XNextEvent(gpDisplay, &event);
		switch(event.type) {
			case MapNotify :
				pXFontStruct = XLoadQueryFont(gpDisplay, "fixed");
				break;
			case KeyPress :
				keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
				switch(keysym) {
					case XK_Escape :
						XUnloadFont(gpDisplay, pXFontStruct->fid);
						XFreeGC(gpDisplay, gc);
						Uninitialize();
						exit(0);
					case XK_F :
					case XK_f :
						ToggleFullscreen();
						if(bFullscreen == false)
							bFullscreen = true;
						else
							bFullscreen = false;
						break;
					default :
						break;
				}
				//XLookupString(gpDisplay, &key, sizeof(key), NULL, NULL);
				XLookupString(&event.xkey, &key, sizeof(key), NULL, NULL);
				switch(key) {
					case 'x' :
					case 'X' :
						Uninitialize();
						exit(0);
					default :
						break;
				}
				break;
			case ButtonPress :
				switch(event.xbutton.button) {
					case 1 :	// left button of mouse
						break;
					case 2 :	// middle button of mouse
						break;
					case 3 :	// right button of mouse
						break;
					case 4 :	// upward scrolling of wheel
						break;
					case 5 :	// downward scrolling of wheel
						break;
					default :
						break;
				}
				break;
			case MotionNotify :
				break;
			case ConfigureNotify :
				winWidth = event.xconfigure.width;
				winHeight = event.xconfigure.height;
				break;
			case Expose :
				gc = XCreateGC(gpDisplay, gWindow, 0, &gcValues);
				XSetFont(gpDisplay, gc, pXFontStruct->fid);
				XAllocNamedColor(gpDisplay, gColormap, "green", &greenColor, &greenColor);
				XSetForeground(gpDisplay, gc, greenColor.pixel);
				
				stringLength = strlen(str);
				stringWidth = XTextWidth(pXFontStruct, str, stringLength);
				fontHeight = pXFontStruct->ascent + pXFontStruct->descent;
				XDrawString(gpDisplay, gWindow, gc, (winWidth/2) - (stringWidth/2), (winHeight/2) - (fontHeight/2), str, stringLength);
				break;
			case DestroyNotify :
				break;
			case 33 :
				XUnloadFont(gpDisplay, pXFontStruct->fid);
				XFreeGC(gpDisplay, gc);
				Uninitialize();
				exit(0);
			default :
				break;
		}
	}
	Uninitialize();
	return 0;
}

// Function to create window
void CreateWindow(void) {
	// Function declaration
	void Uninitialize();

	// Variable declaration
	XSetWindowAttributes winAttribs;
	int defaultScreen;
	int defaultDepth;
	int styleMask;

	// Code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL) {
		printf("\n ERROR : Unable to open XDisplay.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	defaultDepth = DefaultDepth(gpDisplay, defaultScreen);

	gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo == NULL) {
		printf("\n ERROR : Unable to allocate memory for Visual Info.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo);
	if(gpXVisualInfo == NULL) {
		printf("\n ERROR : Unable to get a Visual.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);
	
	gColormap = winAttribs.colormap;
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), 0, 0, giWindowWidth, giWindowHeight, 0, gpXVisualInfo->depth, InputOutput, gpXVisualInfo->visual, styleMask, &winAttribs);
	if(!gWindow) {
		printf("\n ERROR : Failed to create main window.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	XStoreName(gpDisplay, gWindow, "Hello World in XWindows");

	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);

	XMapWindow(gpDisplay, gWindow);
}

void ToggleFullscreen() {
	// Variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev = { 0 };

	// Code
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	memset(&xev, 0, sizeof(xev));

	xev.type = ClientMessage;
	xev.xclient.window = gWindow;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
	
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;

	XSendEvent(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), False, StructureNotifyMask, &xev);
}

void Uninitialize() {
	if(gWindow)
		XDestroyWindow(gpDisplay, gWindow);

	if(gColormap)
		XFreeColormap(gpDisplay, gColormap);

	if(gpXVisualInfo) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if(gpDisplay) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}

