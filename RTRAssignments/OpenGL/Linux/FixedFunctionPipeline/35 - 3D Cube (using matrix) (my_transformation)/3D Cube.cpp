// 3D Cube using matrix in XWindows
// Date : 17 February 2021
// By : Darshan Vikam

// General Header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

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
GLfloat identityMatrix[16];
GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat rotationMatrix[16];

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

	XStoreName(gpDisplay, gWindow, "3D Cube using my_transformation");

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
	void my_glIdentity(GLfloat *);
	void my_glTranslate(GLfloat *, float, float, float);
	void my_glScale(GLfloat *, float, float, float);

	// Code
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	my_glIdentity(identityMatrix);
	my_glTranslate(translationMatrix, 0.0f, 0.0f, -5.0f);
	my_glScale(scaleMatrix, 0.75f, 0.75f, 0.75f);

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
	// Function declaration
	void my_glRotation(GLfloat, float, float ,float);
	
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	// glLoadIdentity();
	glLoadMatrixf(identityMatrix);		// Instead of glLoadIdentity()

	// glTranslatef(0.0f, 0.0f, -5.0f);
	// glRotatef(gGLfAngle, 1.0f, 0.0f, 0.0f);
	// glRotatef(gGLfAngle, 0.0f, 1.0f, 0.0f);
	// glRotatef(gGLfAngle, 0.0f, 0.0f, 1.0f);

	glMultMatrixf(translationMatrix);
	glMultMatrixf(scaleMatrix);
	my_glRotation(gGLfAngle, 1.0f, 0.0f, 0.0f);
	my_glRotation(gGLfAngle, 0.0f, 1.0f, 0.0f);
	my_glRotation(gGLfAngle, 0.0f, 0.0f, 1.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);		// Front face
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);		// Right face
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);		// Bottom face
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);		// Left face
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glColor3f(1.0f, 0.0f, 0.0f);		// Back face
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);		// Top face
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

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

	if(gpDisplay) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}

void my_glIdentity(GLfloat *identity) {
	// Variable declaration
	GLfloat matrix[4][4];
	int i, j;

	// Function declaration
	void TwoD2OneDColumnMajor(GLfloat matrix[4][4], GLfloat identity[16]);

	// Code
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(i == j)
				matrix[i][j] = 1;
			else
				matrix[i][j] = 0;
		}
	}	
	TwoD2OneDColumnMajor(matrix, identity);
}

void my_glTranslate(GLfloat *matrix, float x, float y, float z) {
	// Code
	my_glIdentity(matrix);
	matrix[12] = (GLfloat)x;
	matrix[13] = (GLfloat)y;
	matrix[14] = (GLfloat)z;
}

void my_glScale(GLfloat *matrix, float x, float y, float z) {
	// Code
	my_glIdentity(matrix);
	matrix[0] = (GLfloat)x;
	matrix[5] = (GLfloat)y;
	matrix[10] = (GLfloat)z;
}

void my_glRotation(GLfloat angle, float x, float y, float z) {
	// Variable declaration
	float radAngle;

	// Code
	my_glIdentity(rotationMatrix);
	radAngle = (float)(angle * (M_PI / 180.0f));
	if(x == 1.0f) {
		rotationMatrix[5] = cos(radAngle);
		rotationMatrix[6] = sin(radAngle);
		rotationMatrix[9] = -sin(radAngle);
		rotationMatrix[10] = cos(radAngle);
	}
	else if(y == 1.0f) {
		rotationMatrix[0] = cos(radAngle);
		rotationMatrix[2] = -sin(radAngle);
		rotationMatrix[8] = sin(radAngle);
		rotationMatrix[10] = cos(radAngle);
	}
	else if(z == 1.0f) {
		rotationMatrix[0] = cos(radAngle);
		rotationMatrix[1] = sin(radAngle);
		rotationMatrix[4] = -sin(radAngle);
		rotationMatrix[5] = cos(radAngle);
	}
	glMultMatrixf(rotationMatrix);
}

void TwoD2OneDColumnMajor(GLfloat TwoDMatrix[4][4], GLfloat OneDMatrix[16]) {
	// Variable declaration
	int i, j;

	// Code
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++)
			OneDMatrix[((i*4)+j)] = TwoDMatrix[j][i];
	}
}

