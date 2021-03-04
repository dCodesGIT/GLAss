// Solar system with moon using push pop matrix in XWindows
// Date : 21 February 2021
// By : Darshan Vikam

// General Header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

// OpenGL specific header files
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

// XWindows specific header files
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

// Namespaces
using namespace std;

// Global variable declaration
GLXContext gGLXContext;
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;
GLfloat gGLfAngle = 0.0f;
int giMoon = 0;
int giDay = 0;
int giYear = 0;
GLUquadric* quadric = NULL;

// Entry point function
int main() {
	// Function declaration
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void Initialize(void);
	void Resize(int, int);
	void display(void);
	void Update(void);
	void Uninitialize();

	// Variable declaration
	bool bDone = false;
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;
	int i = 0;

	// Code
	CreateWindow();
	Initialize();

	// Message loop
	XEvent event;
	KeySym keysym;
	while(bDone == false) {
		while(XPending(gpDisplay)) {
			XNextEvent(gpDisplay, &event);
			switch(event.type) {
				case MapNotify :
					break;
				case KeyPress :
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch(keysym) {
						case XK_Escape :
							bDone = true;
							break;
						case XK_F :
						case XK_f :
							ToggleFullscreen();
							if(bFullscreen == false)
								bFullscreen = true;
							else
								bFullscreen = false;
							break;
						case XK_X :
						case XK_x :
							if(bFullscreen == true)
								ToggleFullscreen();
							bDone = true;
							break;
						case XK_M : 
							giMoon = (giMoon + 12) % 360;
							break;
						case XK_m : 
							giMoon = (giMoon - 12) % 360;
							break;
						case XK_D :
							giDay = (giDay + 6) % 360;
							giMoon = (giMoon + 12) % 360;
							break;
						case XK_d :
							giDay = (giDay - 6) % 360;
							giMoon = (giMoon - 12) % 360;
							break;
						case XK_Y :
							giYear = (giYear + 3) % 360;
							giDay = (giDay + 6) % 360;
							giMoon = (giMoon + 24) % 360;
							break;
						case XK_y :
							giYear = (giYear - 3) % 360;
							giDay = (giDay - 6) % 360;
							giMoon = (giMoon - 24) % 360;
							break;
						default :
							break;
					}
					break;
				case ButtonPress :
					switch(event.xbutton.button) {
						case 1 :
							break;
						case 2 :
							break;
						case 3 :
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
					Resize(winWidth, winHeight);
					break;
				case Expose :
					break;
				case DestroyNotify :
					break;
				case 33 :
					bDone = true;
					break;
				default :
					break;
			}
		}
		Update();
		display();
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
	static int frameBufferAttribs[] = { GLX_DOUBLEBUFFER, True,	// Enables double buffering for rendering
		GLX_RGBA,				// pixel type
		GLX_RED_SIZE, 8,			// size of RED bits
		GLX_GREEN_SIZE, 8,			// size of GREEN bits
		GLX_BLUE_SIZE, 8,			// size of BLUE bits
		GLX_ALPHA_SIZE, 8,			// size of ALPHA bits
		GLX_DEPTH_SIZE, 24,			// Enables depth for rendering(V4L recomended size - 24)
		None };					// None macro/typedef is same as '0' (Zero)

	// Code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL) {
		printf("\n ERROR : Unable to open XDisplay.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));
	if(gpXVisualInfo == NULL) {
		printf("\n ERROR : Unable to allocate memory for Visual Info.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}
	
	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttribs);

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

	XStoreName(gpDisplay, gWindow, "Solar System with moon");

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

void Initialize(void) {
	// Function declaration
	void Resize(int, int);
	
	// Code
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	Resize(giWindowWidth, giWindowHeight);
}

void Resize(int width, int height) {
	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void display(void) {
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);	// glTranslatef(0.0f, 0.0f, 5.0f);

	glPushMatrix();						// Sun
		glRotatef((GLfloat)90.0f, 1.0f, 0.0f, 0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(1.0f, 1.0f, 0.0f);
		quadric = gluNewQuadric();
		gluSphere(quadric, 0.75f, 30, 30);
	glPopMatrix();

	glPushMatrix();						// Earth
		glRotatef((GLfloat)giYear, 0.0f, 1.0f, 0.0f);
		glTranslatef(2.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)giDay, 0.0f, 0.0f, 1.0f);
		glRotatef((GLfloat)23.5f, 1.0f, 0.0f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.4f, 0.9f, 1.0f);
		quadric = gluNewQuadric();
		gluSphere(quadric, 0.2f, 20, 20);
	glPopMatrix();

	glPushMatrix();						// Moon
		glRotatef((GLfloat)giYear, 0.0f, 1.0f, 0.0f);
		glTranslatef(2.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)giDay, 0.0f, 0.0f, 1.0f);
		glRotatef((GLfloat)23.5f, 1.0f, 0.0f, 1.0f);
		glTranslatef(0.5f, 0.0f, 0.0f);
		glRotatef((GLfloat)90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef((GLfloat)giMoon, 0.0f, 0.0f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(1.0f, 1.0f, 1.0f);
		quadric = gluNewQuadric();
		gluSphere(quadric, 0.05f, 10, 10);
	glPopMatrix();

	glXSwapBuffers(gpDisplay, gWindow);
}

void Update(void) {
	// Code
	gGLfAngle += 0.5f;
	if(gGLfAngle >= 360.0f)
		gGLfAngle = 0.0f;
}

void Uninitialize() {
	// Variable declaration
	GLXContext currentGLXContext;
	
	// Code
	currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext == gGLXContext)
		glXMakeCurrent(gpDisplay, 0, 0);
	if(gGLXContext)
		glXDestroyContext(gpDisplay, gGLXContext);

	if(gWindow)
		XDestroyWindow(gpDisplay, gWindow);

	if(gColormap)
		XFreeColormap(gpDisplay, gColormap);

	if(gpXVisualInfo) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	gluDeleteQuadric(quadric);

	if(gpDisplay) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}

