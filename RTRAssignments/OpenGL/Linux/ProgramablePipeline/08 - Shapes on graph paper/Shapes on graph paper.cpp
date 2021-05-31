// 2D shapes on graph paper in XWindows in Programmable Pipeline
// Date : 30 April 2021
// By : Darshan Vikam

// General Header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../Include/vmath.h"

// OpenGL specific header files
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

// XWindows specific header files
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

// Namespaces
using namespace std;
using namespace vmath;

// Global enum declaration
enum {
	DV_ATTRIB_POS = 0,
	DV_ATTRIB_COLOR,
	DV_ATTRIB_NORM,
	DV_ATTRIB_TEX,
};

typedef GLXContext (* glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

// Global variable declaration
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext;
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;

int keyPressed = 0;
GLfloat triSide = 0.5f;
GLfloat rect_width = 0.7f, rect_height = 0.5f;
GLfloat radius = 0.5f;

GLuint gVSObj;		// Vertex Shader Object
GLuint gFSObj;		// Fragment Shader Object
GLuint gSPObj;		// Shader Program Object
GLuint gVAObj_GraphPaper;	// Vertex Array Object - graph paper
GLuint gVBObj_GraphPaper[2];	// Vertex Buffer Object - graph paper
GLuint gVAObj_shapes[3];	// Vertex Array Object - [0]-triangle; [1]-rectangle; [2]-circle
GLuint gVBObj_triangle[2];	// Vertex Buffer Object - for triangle
GLuint gVBObj_rectangle[2];	// Vertex Buffer Object - for rectangle
GLuint gVBObj_circle[2];	// Vertex Buffer Object - for circle
GLuint gMVPUniform;	// Model View Projection matrix Uniform

mat4 gPerspMatrix;	// 4x4 matrix for orthographic projection

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
						case XK_Up :
							if(keyPressed == 1 && triSide < 1.5f)
								triSide += 0.1f;
							else if(keyPressed == 2 && rect_width < 1.75f && rect_height < 1.75f) {
								rect_width += 0.1f;
								rect_height += 0.1f;
							}
							else if(keyPressed == 3 && radius < 1.5f)
								radius += 0.1f;
							break;
						case XK_Down :
							if(keyPressed == 1 && triSide > 0.2f)
								triSide -= 0.1f;
							else if(keyPressed == 2 && rect_width > 0.2f && rect_height > 0.2f) {
								rect_width -= 0.1f;
								rect_height -= 0.1f;
							}
							else if(keyPressed == 3 && radius > 0.2f)
								radius -= 0.1f;
							break;
						case XK_Left :
							if(keyPressed > 0)
								keyPressed--;
							break;
						case XK_Right :
							if(keyPressed < 3)
								keyPressed++;
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
	int styleMask;
	static int frameBufferAttribs[] = { GLX_DOUBLEBUFFER, True,	// Enables double buffering for rendering
		GLX_X_RENDERABLE, True,			// Enable hardware based(GPU based) high definition rendering
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,	// Enable drawable type
		GLX_RENDER_TYPE, GLX_RGBA_BIT,		// Enabling rendering type(color style) to RGBA style
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,	// Enabling visual type(display type) to True Color
		GLX_RGBA,				// pixel type
		GLX_RED_SIZE, 8,			// size of RED bits
		GLX_GREEN_SIZE, 8,			// size of GREEN bits
		GLX_BLUE_SIZE, 8,			// size of BLUE bits
		GLX_ALPHA_SIZE, 8,			// size of ALPHA bits
		GLX_DEPTH_SIZE, 24,			// Enables depth for rendering(V4L recomended size - 24)
		GLX_STENCIL_SIZE, 8,			// size of stencil bits
		None };					// None macro/typedef is same as '0' (Zero)
	GLXFBConfig *pGLXFBConfig = NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo = NULL;
	int numFBConfig = 0;
	int bestFBConfig = -1;
	int worstFBConfig = -1;
	int bestSamples = -1;
	int worstSamples = 99;

	// Code
	gpDisplay = XOpenDisplay(NULL);
	if(gpDisplay == NULL) {
		printf("\n ERROR : Unable to open XDisplay.");
		printf("\n Exitting now...");
		Uninitialize();
		exit(1);
	}

	defaultScreen = XDefaultScreen(gpDisplay);

	pGLXFBConfig = glXChooseFBConfig(gpDisplay, defaultScreen, frameBufferAttribs, &numFBConfig);
	if(numFBConfig <= 0)
		Uninitialize();

	for(int i = 0; i < numFBConfig; i++) {
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
		if(pTempXVisualInfo != NULL) {
			int sampleBuffers, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);
			if(bestFBConfig < 0 || sampleBuffers && samples > bestSamples) {
				bestFBConfig = i;
				bestSamples = samples;
			}
			if(worstFBConfig < 0 || !sampleBuffers || samples < worstSamples) {
				worstFBConfig = i;
				worstSamples = samples;
			}
		//	printf("\n %d. GLXFBConfig[%d] ==> sampleBuffer - %d buffers - %d", i+1, i, sampleBuffers, samples);
		}
		XFree(pTempXVisualInfo);
	}
	bestGLXFBConfig = pGLXFBConfig[bestFBConfig];
	gGLXFBConfig = bestGLXFBConfig;
	XFree(pGLXFBConfig);

	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, gGLXFBConfig);

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

	XStoreName(gpDisplay, gWindow, "Shapes on graph paper");

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
	void Uninitialize();
	void ShaderErrorCheck(GLuint, char*);		// Check shader's post compilation and linking errors 

	// Variable declaration
	FILE *OGL_info = NULL;
	const int attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None };
	Bool bIsDirectContext;
	GLfloat GraphPaperVertices[164][2];
	GLfloat GraphPaperColor[164][4];

	// Code
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	if(!gGLXContext) {
		const int attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None };
		gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	}

	bIsDirectContext = glXIsDirect(gpDisplay, gGLXContext);
	printf("\n Rendering Context : ");
	if(bIsDirectContext == True)
		printf("Hardware rendering (best quality)");
	else
		printf("Software rendering (low quality)");
	printf("\n\n");

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
		Uninitialize();

	// OpenGL related log entry
	OGL_info = fopen("OpenGL_info.txt", "w");
	if(OGL_info == NULL)
		printf("Unable to open file to write OpenGL related information");
	fprintf(OGL_info, "*** OpenGL Information ***\n\n");
	fprintf(OGL_info, "*** OpenGL related basic information ***\n");
	fprintf(OGL_info, "OpenGL Vendor Company : %s\n", glGetString(GL_VENDOR));
	fprintf(OGL_info, "OpenGL Renderer(Graphics card company) : %s\n", glGetString(GL_RENDERER));
	fprintf(OGL_info, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(OGL_info, "Graphics Library Shading Language(GLSL) Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(OGL_info, "*** OpenGL supported/related extentions ***\n");
	// OpenGL supported/related Extensions
	GLint numExts;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
	for(int i = 0; i < numExts; i++)
		fprintf(OGL_info, "%d. %s\n", i+1, glGetStringi(GL_EXTENSIONS, i));
	fclose(OGL_info);
	OGL_info = NULL;

	// Vertex Shader
	gVSObj = glCreateShader(GL_VERTEX_SHADER);	// Create shader
	const GLchar *VSSrcCode =			// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vColor;" \
		"out vec3 out_color;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void) {" \
			"out_color = vColor;" \
			"gl_Position = u_mvpMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);			// Compile Shader
	ShaderErrorCheck(gVSObj, (char *)"VERTEX");	// Error checking for shader
//	ShaderErrorCheck(gVSObj, "COMIPLE");		// Error checking for shader

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec3 out_color;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = vec4(out_color, 1.0f);" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);			// Compile Shader
	ShaderErrorCheck(gFSObj, (char *)"FRAGMENT");	// Error checking for shader
//	ShaderErrorCheck(gFSObj, "COMPILE");		// Error checking for shader

	// Shader program
	gSPObj = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj, gVSObj);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj, gFSObj);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
	glBindAttribLocation(gSPObj, DV_ATTRIB_COLOR, "vColor");
	glLinkProgram(gSPObj);
	ShaderErrorCheck(gSPObj, (char *)"PROGRAM");	// Error checking for shader
//	ShaderErrorCheck(gSPObj, "LINK");

	// Get uniform location(s)
	gMVPUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");

	// Other variable declarations
	int i;
	GLfloat vert;
	// Variable initialization - graph paper
	for(i = 0, vert = -2.0f; i < 164 && vert <= 2.1f; i++, vert += 0.1f) {
		// Left coordinate of horizontal line
		GraphPaperVertices[i][0] = -2.0f;
		GraphPaperVertices[i][1] = vert;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
			GraphPaperColor[i][3] = 0.0f;

		// Right coordinate of horizontal line
		i++;
		GraphPaperVertices[i][0] = 2.0f;
		GraphPaperVertices[i][1] = vert;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
			GraphPaperColor[i][3] = 0.0f;

		// Bottom coordinate of Vertical line
		i++;
		GraphPaperVertices[i][0] = vert;
		GraphPaperVertices[i][1] = -2.0f;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
			GraphPaperColor[i][3] = 0.0f;

		// Top coordinate of Vertical line
		i++;
		GraphPaperVertices[i][0] = vert;
		GraphPaperVertices[i][1] = 2.0f;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
			GraphPaperColor[i][3] = 0.0f;
	}
	// Red color of X Axis - left point
	GraphPaperColor[164/2][0] = 1.0f;
	GraphPaperColor[164/2][1] = 0.0f;
	GraphPaperColor[164/2][2] = 0.0f;
	GraphPaperColor[164/2][3] = 0.0f;

	// Red color of X Axis - right point
	GraphPaperColor[(164/2)+1][0] = 1.0f;
	GraphPaperColor[(164/2)+1][1] = 0.0f;
	GraphPaperColor[(164/2)+1][2] = 0.0f;
	GraphPaperColor[(164/2)+1][3] = 0.0f;

	// Green color of Y Axis - Bottom point
	GraphPaperColor[(164/2)-1][0] = 0.0f;
	GraphPaperColor[(164/2)-1][1] = 1.0f;
	GraphPaperColor[(164/2)-1][2] = 0.0f;
	GraphPaperColor[(164/2)-1][3] = 0.0f;

	// Green color of Y Axis - Top point
	GraphPaperColor[(164/2)-2][0] = 0.0f;
	GraphPaperColor[(164/2)-2][1] = 1.0f;
	GraphPaperColor[(164/2)-2][2] = 0.0f;
	GraphPaperColor[(164/2)-2][3] = 0.0f;

	// For graph paper
	glGenVertexArrays(1, &gVAObj_GraphPaper);
	glBindVertexArray(gVAObj_GraphPaper);		// Bind following steps into gVAObj_GraphPaper variable
		// For vPosition
		glGenBuffers(2, gVBObj_GraphPaper);		// Generate buffer
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_GraphPaper[0]);	// Binding buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GraphPaperVertices), GraphPaperVertices, GL_STATIC_DRAW);	// Placing values in OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);	// Formatting of data entered into OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glEnableVertexAttribArray(DV_ATTRIB_POS);	// Enabling of OpenGL's buffer(GL_VERTEX_BUFFER)
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// For vColor
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_GraphPaper[1]);	// Binding buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GraphPaperColor), GraphPaperColor, GL_STATIC_DRAW);	// Placing values in OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glVertexAttribPointer(DV_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, NULL);	// Formatting of data entered into OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glEnableVertexAttribArray(DV_ATTRIB_COLOR);	// Enabling of OpenGL's buffer(GL_VERTEX_BUFFER)
		glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbinding buffer - GL_ARRAY_BUFFER
	glBindVertexArray(0);			// unbind and save above steps into gVAObj_GraphPaper

	// For Shapes
	glGenVertexArrays(3, gVAObj_shapes);	// Generate 3 vertex arrays to draw triangle, rectangle and circle
	glBindVertexArray(gVAObj_shapes[0]);	// For triangle
		// vPosition VBObj of Triangle
		glGenBuffers(2, gVBObj_triangle);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[0]);
		glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vColor VBObj of Triangle
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[1]);
		glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_shapes[1]);	// For rectangle
		glGenBuffers(2, gVBObj_rectangle);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[0]);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[1]);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_shapes[2]);	// For circle
		glGenBuffers(2, gVBObj_circle);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[0]);
		glBufferData(GL_ARRAY_BUFFER, 3600 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[1]);
		glBufferData(GL_ARRAY_BUFFER, 3600 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	gPerspMatrix = mat4::identity();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	Resize(giWindowWidth, giWindowHeight);
}

void ShaderErrorCheck(GLuint shaderObject, char *shaderName) {	// Error checking after shader compilation
	// Function declaration
	void Uninitialize(void);

	// Variable declaration
	GLint iErrorLen = 0;
	GLint iStatus = 0;
	char *szError = NULL;
	char shaderOpr[8];

	// Code
	if(strcmp(shaderName, "VERTEX") == 0 || strcmp(shaderName, "TESS_CONTROL") == 0 || strcmp(shaderName, "TESS_EVALUATION") == 0 || strcmp(shaderName, "GEOMETRY") == 0 || strcmp(shaderName, "FRAGMENT") == 0 || strcmp(shaderName, "COMPUTE") == 0)
		strcpy(shaderOpr, "COMPILE");
	else if(strcmp(shaderName, "PROGRAM") == 0)
		strcpy(shaderOpr, "LINK");
	else {
		printf("Invalid second parameter in ShaderErrorCheck()");
		return;
	}

	if(strcmp(shaderOpr, "COMPILE") == 0)
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &iStatus);
	else if(strcmp(shaderOpr, "LINK") == 0)
		glGetProgramiv(shaderObject, GL_LINK_STATUS, &iStatus);
	if(iStatus == GL_FALSE) {
		if(strcmp(shaderOpr, "COMPILE") == 0)
			glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &iErrorLen);
		else if(strcmp(shaderOpr, "LINK") == 0)
			glGetProgramiv(shaderObject, GL_INFO_LOG_LENGTH, &iErrorLen);
		if(iErrorLen > 0) {
			szError = (char *)malloc(iErrorLen);
			if(szError != NULL) {
				GLsizei written;
				if(strcmp(shaderOpr, "COMPILE") == 0) {
					glGetShaderInfoLog(shaderObject, iErrorLen, &written, szError);
					printf("%s Shader Compilation Error log : \n", shaderName);
				}
				else if(strcmp(shaderOpr, "LINK") == 0) {
					glGetProgramInfoLog(shaderObject, iErrorLen, &written, szError);
					printf("Shader %s linking Error log : \n", shaderName);
				}
				printf("%s \n", szError);
				free(szError);
				szError = NULL;
			}
		}
		else
			printf("Error occured during compilation/linking. No error message. \n");
		Uninitialize();
	}
}

void Resize(int width, int height) {
	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void display(void) {
	// Variable declaration
	mat4 ModelViewMatrix, ModelViewProjectionMatrix;
	mat4 translationMatrix, rotationMatrix;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start of OpenGL shader program
	glUseProgram(gSPObj);

	ModelViewMatrix = mat4::identity();
	ModelViewProjectionMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -5.5f);
	ModelViewMatrix = translationMatrix;

	ModelViewProjectionMatrix = gPerspMatrix * ModelViewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, ModelViewProjectionMatrix);

	// OpenGL Drawing
	glBindVertexArray(gVAObj_GraphPaper);
	for(int i = 0; i < 164; i += 2)
		glDrawArrays(GL_LINES, i, 2);
	glBindVertexArray(0);

	if(keyPressed == 1) {		// 1 ==> Triangle
		const GLfloat triangleVertex[] = {
			0.0f, triSide, 0.0f,		// Apex
			-triSide, -triSide, 0.0f,	// left bottom
			triSide, -triSide, 0.0f		// right bottom
		};
		const GLfloat triangleColors[] = {
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f
		};
		glBindVertexArray(gVAObj_shapes[0]);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertex), triangleVertex, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINE_LOOP, 0, 3);
		glBindVertexArray(0);
	}
	else if(keyPressed == 2) {	// 2 ==> Rectangle
		const GLfloat rectangleVertices[] = {
			-rect_width,  rect_height, 0.0f,	// Top left
			-rect_width, -rect_height, 0.0f,	// Bottom left
			 rect_width, -rect_height, 0.0f,	// Bottom right
			 rect_width,  rect_height, 0.0f	// Top right
		};
		const GLfloat rectangleColor[] = {
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f
		};
		glBindVertexArray(gVAObj_shapes[1]);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINE_LOOP, 0, 4);
		glBindVertexArray(0);
	}
	else if(keyPressed == 3) {		// 3 ==> Circle
		GLfloat circleVertices[3600*2];
		GLfloat circleColor[3600*3];
		for(int i = 0; i < 3600*2; i += 2) {
			circleVertices[i] = radius * cos(((GLfloat)i/20.0f)*(M_PI/180.0f));	// X = r * cos(theta)
			circleVertices[i+1] = radius * sin(((GLfloat)i/20.0f)*(M_PI/180.0f));	// Y = r * sin(theta)
		}
		for(int i = 0; i < 3600*3; i += 3) {
			circleColor[i] = 1.0f;
			circleColor[i+1] = 1.0f;
			circleColor[i+2] = 0.0f;
		}
		glBindVertexArray(gVAObj_shapes[2]);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COLOR);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_POINTS, 0, 3600);
		glBindVertexArray(0);
	}

	// End of OpenGL shading program
	glUseProgram(0);

	glXSwapBuffers(gpDisplay, gWindow);
}

void Update(void) {
	// Code
}

void Uninitialize() {
	// Variable declaration
	GLXContext currentGLXContext;
	
	// Code
	if(bFullscreen == true)
		ToggleFullscreen();

	// Stop using shader program
	glUseProgram(0);

	// Destroy Vertex Array Object
	if(gVAObj_GraphPaper) {
		glDeleteVertexArrays(1, &gVAObj_GraphPaper);
		gVAObj_GraphPaper = 0;
	}
	if(gVAObj_shapes) {
		glDeleteVertexArrays(3, gVAObj_shapes);
		gVAObj_shapes[0] = 0;
		gVAObj_shapes[1] = 0;
		gVAObj_shapes[2] = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_GraphPaper) {
		glDeleteBuffers(2, gVBObj_GraphPaper);
		gVBObj_GraphPaper[0] = 0;
		gVBObj_GraphPaper[1] = 0;
	}
	if(gVBObj_triangle) {
		glDeleteBuffers(2, gVBObj_triangle);
		gVBObj_triangle[0] = 0;
		gVBObj_triangle[1] = 0;
	}
	if(gVBObj_rectangle) {
		glDeleteBuffers(2, gVBObj_rectangle);
		gVBObj_rectangle[0] = 0;
		gVBObj_rectangle[1] = 0;
	}
	if(gVBObj_circle) {
		glDeleteBuffers(2, gVBObj_circle);
		gVBObj_circle[0] = 0;
		gVBObj_circle[1] = 0;
	}

	// Detach shaders
	glDetachShader(gSPObj, gVSObj);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj, gFSObj);		// Detach fragment shader from final shader program

	// Delete shaders
	if(gVSObj) {
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gFSObj) {
		glDeleteShader(gFSObj);
		gFSObj = 0;
	}
	if(gSPObj) {
		glDeleteShader(gSPObj);
		gSPObj = 0;
	}

	// Delete shaders
	if(gVSObj) {			// Delete Vertex shader
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gFSObj) {			// Delete Fragment shader
		glDeleteShader(gFSObj);
		gFSObj = 0;
	}
	if(gSPObj) {		// Delete final shader program
		glDeleteProgram(gSPObj);
		gSPObj = 0;
	}

	// Unlink shader program
	glUseProgram(0);

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

