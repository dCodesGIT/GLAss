// 24 sphere (Per Fragment lighting) in XWindows in Programmable Pipeline
// Date : 6 May 2021
// By : Darshan Vikam

// General Header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "../Include/vmath.h"
#include "../Include/Sphere.h"

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

bool gbLightingEnabled = false;
bool gbXRotationEnabled = false;
bool gbYRotationEnabled = false;
bool gbZRotationEnabled = false;
GLfloat gGLfAngle = 0.0f;
int gWidth, gHeight;

GLfloat sphereVertices[1146];
GLfloat sphereNormals[1146];
GLfloat sphereTextures[764];
unsigned short sphereElements[2280];
GLuint gNumVertices, gNumElements;

GLuint gVSObj;		// Vertex Shader Object
GLuint gFSObj;		// Fragment Shader Object
GLuint gSPObj;		// Shader Program Object
GLuint gVAObj_Sphere;	// Vertex Array Object - 3D Sphere 
GLuint gVBObj_Sphere[3];	// Buffer Object - Sphere[3] = [0]-Position; [1]-Normals; [2]-elements;

GLuint gMUniform;	// Model Matrix uniform
GLuint gVUniform;	// View Matrix uniform
GLuint gPUniform;	// Projection Matrix uniform
GLuint gKeyUniform;	// Key press uniform

// Light related uniforms
GLuint gLAmbUniform;		// Ambiemt component of light
GLuint gLDiffUniform;		// Diffuse component of light
GLuint gLSpecUniform;		// Specular componenet of light
GLuint gLPosUniform;		// Light Position
GLuint gKAmbUniform;		// Ambient componenet of Material
GLuint gKDiffUniform;		// Diffuse component of Material
GLuint gKSpecUniform;		// Specular componenet of Material
GLuint gKShineUniform;		//  Shininess of Material

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
						case XK_Q :
						case XK_q :
							if(bFullscreen == true)
								ToggleFullscreen();
							bDone = true;
							break;
						case XK_L :
						case XK_l :
							gbLightingEnabled = !gbLightingEnabled;
							break;
						case XK_X :
						case XK_x :
							gbXRotationEnabled = true;
							gbYRotationEnabled = false;
							gbZRotationEnabled = false;
							gGLfAngle = 0.0f;
							break;
						case XK_Y :
						case XK_y :
							gbXRotationEnabled = false;
							gbYRotationEnabled = true;
							gbZRotationEnabled = false;
							gGLfAngle = 0.0f;
							break;
						case XK_Z :
						case XK_z :
							gbXRotationEnabled = false;
							gbYRotationEnabled = false;
							gbZRotationEnabled = true;
							gGLfAngle = 0.0f;
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
	if(numFBConfig <= 0) {
		Uninitialize();
		exit(1);
	}

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
		"in vec3 vNormal;" \
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
		"uniform int u_KeyPressed;" \
		"uniform vec4 u_LPos;" \
		"out vec3 tNorm, LSrc, viewVec;" \
		"void main(void) {" \
			"if(u_KeyPressed == 1) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" \
				"LSrc = vec3(u_LPos - eyeCoords);" \
				"viewVec = -eyeCoords.xyz;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);			// Compile Shader
	ShaderErrorCheck(gVSObj, (char *)"VERTEX");	// Error checking for shader

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 450 core" \
		"\n" \
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" \
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
		"uniform float u_KShine;" \
		"uniform int u_KeyPressed;" \
		"in vec3 tNorm, LSrc, viewVec;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 lighting;" \
			"if(u_KeyPressed == 1) {" \
				"vec3 transformedNormal = normalize(tNorm);" \
				"vec3 lightSource = normalize(LSrc);" \
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
				"vec3 viewVector = normalize(viewVec);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0f);" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
				"lighting = ambient + diffuse + specular;" \
			"}" \
			"else {" \
				"lighting = vec3(0.0f);" \
			"}" \
			"FragColor = vec4(lighting, 1.0f);" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);			// Compile Shader
	ShaderErrorCheck(gFSObj, (char *)"FRAGMENT");	// Error checking for shader

	// Shader program
	gSPObj = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj, gVSObj);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj, gFSObj);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
	glBindAttribLocation(gSPObj, DV_ATTRIB_NORM, "vNormal");
	glLinkProgram(gSPObj);
	ShaderErrorCheck(gSPObj, (char *)"PROGRAM");	// Error checking for shader

	// Get uniform location(s)
	gMUniform = glGetUniformLocation(gSPObj, "u_MMatrix");
	gVUniform = glGetUniformLocation(gSPObj, "u_VMatrix");
	gPUniform = glGetUniformLocation(gSPObj, "u_PMatrix");
	gLAmbUniform = glGetUniformLocation(gSPObj, "u_LAmb");
	gLDiffUniform = glGetUniformLocation(gSPObj, "u_LDiff");
	gLSpecUniform = glGetUniformLocation(gSPObj, "u_LSpec");
	gLPosUniform = glGetUniformLocation(gSPObj, "u_LPos");
	gKAmbUniform = glGetUniformLocation(gSPObj, "u_KAmb");
	gKDiffUniform = glGetUniformLocation(gSPObj, "u_KDiff");
	gKSpecUniform = glGetUniformLocation(gSPObj, "u_KSpec");
	gKShineUniform = glGetUniformLocation(gSPObj, "u_KShine");
	gKeyUniform = glGetUniformLocation(gSPObj, "u_KeyPressed");

	// Variable declaration - sphere related
	getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// For 3D Sphere
	glGenVertexArrays(1, &gVAObj_Sphere);
	glBindVertexArray(gVAObj_Sphere);		// For Sphere
		glGenBuffers(3, gVBObj_Sphere);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Sphere[0]);	// For Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Sphere[1]);	// For Normals
		glBufferData(GL_ARRAY_BUFFER, sizeof(sphereNormals), sphereNormals, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_NORM);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVBObj_Sphere[2]);	// For Elements
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereElements), sphereElements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

	gWidth = width;
	gHeight = height;

	gPerspMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void display(void) {
	// Variable declaration
	mat4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	mat4 translationMatrix;
	GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[4];
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
		{0.5f, 0.5f, 0.5f, 1.0f},			// 1R 3C - Black plastic
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
	GLfloat radian = M_PI / 180.0f;
	GLfloat radius = 10.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 6; j++) {
			// Starting of OpenGL shading program
			glUseProgram(gSPObj);

			glViewport((gWidth / 4) * i, (gHeight / 6) * (5-j), (GLsizei)(gWidth/4), (GLsizei)(gHeight/6));

			ModelMatrix = mat4::identity();
			ViewMatrix = mat4::identity();
			ProjectionMatrix = mat4::identity();
			translationMatrix = mat4::identity();

			if(gbXRotationEnabled == true) {
				lightPosition[0] = 0.0f; 
				lightPosition[1] = radius * (GLfloat)cos(gGLfAngle * radian);
				lightPosition[2] = radius * (GLfloat)sin(gGLfAngle * radian);
			}
			else if(gbYRotationEnabled == true) {
				lightPosition[0] = radius * (GLfloat)sin(gGLfAngle * radian);
				lightPosition[1] = 0.0f;
				lightPosition[2] = radius * (GLfloat)cos(gGLfAngle * radian);
			}
			else if(gbZRotationEnabled == true) {
				lightPosition[0] = radius * (GLfloat)cos(gGLfAngle * radian);
				lightPosition[1] = radius * (GLfloat)sin(gGLfAngle * radian);
				lightPosition[2] = 0.0f;
			}
			else {
				lightPosition[0] = 10.0f;
				lightPosition[1] = 10.0f;
				lightPosition[2] = 10.0f;
				lightPosition[3] = 1.0f;
			}

			if(materialShininess[(i*6)+j] < 1.0f)
				materialShininess[(i*6)+j] *= 128.0f;
			if(gbLightingEnabled == true) {
				glUniform1i(gKeyUniform, 1);
				glUniform3fv(gLAmbUniform, 1, lightAmbient);
				glUniform3fv(gLDiffUniform, 1, lightDiffuse);
				glUniform3fv(gLSpecUniform, 1, lightSpecular);
				glUniform4fv(gLPosUniform, 1, lightPosition);
				glUniform3fv(gKAmbUniform, 1, materialAmbient[(i*6)+j]);
				glUniform3fv(gKDiffUniform, 1, materialDiffuse[(i*6)+j]);
				glUniform3fv(gKSpecUniform, 1, materialSpecular[(i*6)+j]);
				glUniform1fv(gKShineUniform, 1, &materialShininess[(i*6)+j]);
			}
			else
				glUniform1i(gKeyUniform, 0);

			translationMatrix = translate(0.0f, 0.0f, -2.5f);
			ModelMatrix = translationMatrix;
			ProjectionMatrix = gPerspMatrix;

			glUniformMatrix4fv(gMUniform, 1, GL_FALSE, ModelMatrix);
			glUniformMatrix4fv(gVUniform, 1, GL_FALSE, ViewMatrix);
			glUniformMatrix4fv(gPUniform, 1, GL_FALSE, ProjectionMatrix);

			// OpenGL Drawing
			glBindVertexArray(gVAObj_Sphere);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVBObj_Sphere[2]);
			glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);

			// End of OpenGL shading program
			glUseProgram(0);
		}
	}

	glXSwapBuffers(gpDisplay, gWindow);
}

void Update(void) {
	// Code
	if(gbLightingEnabled == true) {
		if(gbXRotationEnabled == true || gbYRotationEnabled == true || gbZRotationEnabled == true)
			gGLfAngle += 0.5f;
	}
	if(gGLfAngle >= 360.0f)
		gGLfAngle = 0.0f;
}

void Uninitialize() {
	// Variable declaration
	GLXContext currentGLXContext;
	
	// Code
	if(bFullscreen == true)
		ToggleFullscreen();

	// Stop using shader program
	if(glXGetCurrentContext != NULL)
		glUseProgram(0);

	// Destroy Vertex Array Object
	if(gVAObj_Sphere) {
		glDeleteVertexArrays(1, &gVAObj_Sphere);
		gVAObj_Sphere = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_Sphere) {
		glDeleteBuffers(3, gVBObj_Sphere);
		gVBObj_Sphere[0] = 0;
		gVBObj_Sphere[1] = 0;
		gVBObj_Sphere[2] = 0;
	}

	// Detach shaders
	glDetachShader(gSPObj, gVSObj);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj, gFSObj);		// Detach fragment shader from final shader program

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

	exit(0);
}

