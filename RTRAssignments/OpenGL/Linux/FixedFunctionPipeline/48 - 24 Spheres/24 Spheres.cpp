// 24 different Sphere of different material in XWindows
// Date : 23 February 2021
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

float XAxisRotation = 0.0f;
float YAxisRotation = 0.0f;
float ZAxisRotation = 0.0f;
int KeyPressCode = 0;

bool gbLight = false;
GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};	// Black light
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};	// White light
GLfloat lightPosition[] = {0.3f, 0.3f, 0.3f, 1.0f};
//GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightModelAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat lightModelLocalViewer[] = {0.0f};

GLUquadric* quadric[24];

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
						case XK_Q :
						case XK_q :
							if(bFullscreen == true)
								ToggleFullscreen();
							bDone = true;
							break;
						case XK_l :
						case XK_L :
							if(gbLight == true) {
								glDisable(GL_LIGHTING);
								gbLight = false;
							}
							else {
								glEnable(GL_LIGHTING);
								gbLight = true;
							}
							break;
						case XK_X :
						case XK_x :
							KeyPressCode = 1;
							break;
						case XK_Y :
						case XK_y :
							KeyPressCode = 2;
							break;
						case XK_Z :
						case XK_z :
							KeyPressCode = 3;
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

	XStoreName(gpDisplay, gWindow, "24 Spheres");

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

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lightModelLocalViewer);

	// actual initialization of light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	//glEnable(GL_LIGHT0);

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

	if(width <= height)
		glOrtho(0.0f, 15.5f, 0.0f, 15.5f*((GLfloat)height/(GLfloat)width), -10.0f, 10.0f);
	else
		glOrtho(0.0f, 15.5f*((GLfloat)width/(GLfloat)height), 0.0f, 15.5f, -10.0f, 10.0f);
	// gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void display(void) {
	// Function declaration
	void Spheres_24();

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	if(KeyPressCode == 1) {
		glRotatef(XAxisRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = XAxisRotation;
		lightPosition[2] = 0.0f;
	}
	else if(KeyPressCode == 2) {
		glRotatef(YAxisRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = YAxisRotation;
	}
	else if(KeyPressCode == 3) {
		glRotatef(ZAxisRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = ZAxisRotation;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	glPopMatrix();

	//glTranslatef(-1.5f, 1.0f, -3.0f);
	Spheres_24();

	glXSwapBuffers(gpDisplay, gWindow);
}

void Spheres_24(void) {
	// Variable declaration
	int i, j;
	GLfloat materialAmbient[][4] =
	{
		{0.0215f, 0.1745f, 0.0215f, 1.0f},	// 1R 1C - Emerald
		{0.135f, 0.2225f, 0.1575f, 1.0f},	// 2R 1C - Jade
		{0.05375f, 0.05f, 0.06625f, 1.0f},	// 3R 1C - Obsidian
		{0.25f, 0.20725f, 0.20725f, 1.0f},	// 4R 1C - Pearl
		{0.1745f, 0.01175f, 0.01175f, 1.0f},	// 5R 1C - Ruby
		{0.1f, 0.18725f, 0.1745f, 1.0f},	// 6R 1C - Turquoise
		{0.329412f, 0.223529f, 0.027451f, 1.0f},// 1R 2C - Brass
		{0.2125f, 0.1275f, 0.054f, 1.0f},	// 2R 2C - Bronze
		{0.25f, 0.25f, 0.25f, 1.0f},		// 3R 2C - Chrome
		{0.19125f, 0.0735f, 0.0225f, 1.0f},	// 4R 2C - Copper
		{0.24725f, 0.1995f, 0.0745f, 1.0f},	// 5R 2C - Gold
		{0.19225f, 0.19225f, 0.19225f, 1.0f},	// 6R 2C - Silver
		{0.0f, 0.0f, 0.0f, 1.0f},		// 1R 3C - Black plastic
		{0.0f, 0.1f, 0.06f, 1.0f},		// 2R 3C - Cyan plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 3R 3C - Green plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 4R 3C - Red plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 5R 3C - White plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 6R 3C - Yellow plastic
		{0.02f, 0.02f, 0.02f, 1.0f},		// 1R 4C - Black rubber
		{0.0f, 0.05f, 0.05f, 1.0f},		// 2R 4C - Cyan rubber
		{0.0f, 0.05f, 0.0f, 1.0f},		// 3R 4C - Green rubber
		{0.05f, 0.0f, 0.0f, 1.0f},		// 4R 4C - Red rubber
		{0.05f, 0.05f, 0.05f, 1.0f},		// 5R 4C - White rubber
		{0.05f, 0.05f, 0.04f, 1.0f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialDiffuse[][4] =
	{
		{0.07568f, 0.61424f, 0.07568f, 1.0f},	// 1R 1C - Emerald
		{0.54f, 0.89f, 0.63f, 1.0f},		// 2R 1C - Jade
		{0.18275f, 0.17f, 0.22525f, 1.0f},	// 3R 1C - Obsidian
		{1.0f, 0.829f, 0.829f, 1.0f},		// 4R 1C - Pearl
		{0.61424f, 0.04136f, 0.04163f, 1.0f},	// 5R 1C - Ruby
		{0.396f, 0.74151f, 0.69102f, 1.0f},	// 6R 1C - Turquoise
		{0.780392f, 0.568627f, 0.113725f, 1.0f},// 1R 2C - Brass
		{0.714f, 0.4284f, 0.18144f, 1.0f},	// 2R 2C - Bronze
		{0.4f, 0.4f, 0.4f, 1.0f},		// 3R 2C - Chrome
		{0.7038f, 0.27048f, 0.0828f, 1.0f},	// 4R 2C - Copper
		{0.75164f, 0.60648f, 0.22648f, 1.0f},	// 5R 2C - Gold
		{0.50754f, 0.50754f, 0.50754f, 1.0f},	// 6R 2C - Silver
		{0.01f, 0.01f, 0.01f, 1.0f},		// 1R 3C - Black plastic
		{0.0f, 0.50980392f, 0.50980392f, 1.0f},	// 2R 3C - Cyan plastic
		{0.1f, 0.35f, 0.1f, 1.0f},		// 3R 3C - Green plastic
		{0.5f, 0.0f, 0.0f, 1.0f},		// 4R 3C - Red plastic
		{0.55f, 0.55f, 0.55f, 1.0f},		// 5R 3C - White plastic
		{0.5f, 0.5f, 0.0f, 1.0f},		// 6R 3C - Yellow plastic
		{0.01f, 0.01f, 0.01f, 1.0f},		// 1R 4C - Black rubber
		{0.4f, 0.5f, 0.5f, 1.0f},		// 2R 4C - Cyan rubber
		{0.4f, 0.5f, 0.4f, 1.0f},		// 3R 4C - Green rubber
		{0.5f, 0.4f, 0.4f, 1.0f},		// 4R 4C - Red rubber
		{0.5f, 0.5f, 0.5, 1.0f},		// 5R 4C - White rubber
		{0.5f, 0.5f, 0.4f, 1.0f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialSpecular[][4] =
	{
		{0.633f, 0.727811f, 0.33f, 1.0f},		// 1R 1C - Emerald
		{0.316228f, 0.316228f, 0.316228f, 1.0f},	// 2R 1C - Jade
		{0.332741f, 0.328634f, 0.346435f, 1.0f},	// 3R 1C - Obsidian
		{0.296648f, 0.296648f, 0.296648f, 1.0f},	// 4R 1C - Pearl
		{0.727811f, 0.626959f, 0.626959f, 1.0f},	// 5R 1C - Ruby
		{0.297254f, 0.308290f, 0.306678f, 1.0f},	// 6R 1C - Turquoise
		{0.992157f, 0.941176f, 0.807843f, 1.0f},	// 1R 2C - Brass
		{0.393548f, 0.271906f, 0.166721f, 1.0f},	// 2R 2C - Bronze
		{0.774597f, 0.774597f, 0.774597f, 1.0f},	// 3R 2C - Chrome
		{0.256777f, 0.137622f, 0.086014f, 1.0f},	// 4R 2C - Copper
		{0.628281f, 0.555802f, 0.366065f, 1.0f},	// 5R 2C - Gold
		{0.508273f, 0.508273f, 0.508273f, 1.0f},	// 6R 2C - Silver
		{0.5f, 0.5f, 0.5f, 1.0f},		// 1R 3C - Black plastic
		{0.50196078f, 0.50196078f, 0.50196078f, 1.0f},	// 2R 3C - Cyan plastic
		{0.45f, 0.55f, 0.45f, 1.0f},		// 3R 3C - Green plastic
		{0.7f, 0.6f, 0.6f, 1.0f},		// 4R 3C - Red plastic
		{0.7f, 0.7f, 0.7f, 1.0f},		// 5R 3C - White plastic
		{0.6f, 0.6f, 0.5f, 1.0f},		// 6R 3C - Yellow plastic
		{0.4f, 0.4f, 0.4f, 1.0f},		// 1R 4C - Black rubber
		{0.04f, 0.7f, 0.7f, 1.0f},		// 2R 4C - Cyan rubber
		{0.04f, 0.7f, 0.04f, 1.0f},		// 3R 4C - Green rubber
		{0.7f, 0.04f, 0.04f, 1.0f},		// 4R 4C - Red rubber
		{0.7f, 0.7f, 0.7f, 1.0f},		// 5R 4C - White rubber
		{0.7f, 0.7f, 0.04f, 1.0f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialShininess[] =
	{	0.6f,		// 1R 1C - Emerald
		0.1f,		// 2R 1C - Jade
		0.3f,		// 3R 1C - Obsidian
		0.088f,		// 4R 1C - Pearl
		0.6f,		// 5R 1C - Ruby
		0.1f,		// 6R 1C - Turquoise
		0.21794872f,	// 1R 2C - Brass
		0.2f,		// 2R 2C - Bronze
		0.6f,		// 3R 2C - Chrome
		0.1f,		// 4R 2C - Copper
		0.4f,		// 5R 2C - Gold
		0.4f,		// 6R 2C - Silver
		0.25f,		// 1R 3C - Black plastic
		0.25f,		// 2R 3C - Cyan plastic
		0.25f,		// 3R 3C - Green plastic
		0.25f,		// 4R 3C - Red plastic
		0.25f,		// 5R 3C - White plastic
		0.25f,		// 6R 3C - Yellow plastic
		0.078125f,	// 1R 4C - Black rubber
		0.078125f,	// 2R 4C - Cyan rubber
		0.078125f,	// 3R 4C - Green rubber
		0.078125f,	// 4R 4C - Red rubber
		0.078125f,	// 5R 4C - White rubber
		0.078125f	// 6R 4C - Yellow rubber
	};

	// Code
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 16.5f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for(i = 0; i < 24; i++) {
		if(i != 0 && i%6 == 0)
			glTranslatef(7.0f, 15.0f, 0.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient[i]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse[i]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular[i]);
		materialShininess[i] *= 128.0f;
		glMaterialf(GL_FRONT, GL_SHININESS, materialShininess[i]);

		glTranslatef(0.0f, -2.5f, 0.0f);
		glColor3f(0.1f, 0.1f, 0.1f);
		quadric[i] = gluNewQuadric();
		gluSphere(quadric[i], 1.0f, 30, 30);
	}
}

void Update(void) {
	// Code
	gGLfAngle += 0.5f;
	if(gGLfAngle >= 360.0f)
		gGLfAngle = 0.0f;

	XAxisRotation += 0.5f;
	YAxisRotation += 0.5f;
	ZAxisRotation += 0.5f;
/*	if(XAxisRotation >= 360.0f)
		XAxisRotation = 0.0f;
	if(YAxisRotation >= 360.0f)
		YAxisRotation = 0.0f;
	if(ZAxisRotation >= 360.0f)
		ZAxisRotation = 0.0f;
*/
}

void Uninitialize() {
	// Variable declaration
	GLXContext currentGLXContext;
	int i;
	
	// Code
	currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext == gGLXContext)
		glXMakeCurrent(gpDisplay, 0, 0);
	if(gGLXContext)
		glXDestroyContext(gpDisplay, gGLXContext);

	for(i = 0; i < 24; i++) {
		if(quadric[i]) {
			gluDeleteQuadric(quadric[i]);
			quadric[i] = NULL;
		}
	}

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

