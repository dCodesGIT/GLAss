// Cube using interleaved array in XWindows in Programmable Pipeline
// Date : 11 July 2021
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
#include <GL/glu.h>
#include <GL/glx.h>
#include <SOIL/SOIL.h>

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

bool gbEnableInterleaved = false;
bool gbEnableAnimation = false;
GLfloat gfAngle = 0.0f;

GLuint gVSObj, gFSObj, gSPObj;
GLuint gVAObj, gVBObj;

GLuint MMatrixUniform, VMatrixUniform, PMatrixUniform;
GLuint LAmbUniform, LDiffUniform, LSpecUniform, LPosUniform;
GLuint KAmbUniform, KDiffUniform, KSpecUniform, KShineUniform;
GLuint InterleavedEnabledUniform;

GLuint gTextureSamplerUniform;
GLuint marble_texture;

mat4 gPerspMatrix;

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
						case XK_I :		// For interleaved
						case XK_i :
							gbEnableInterleaved = !gbEnableInterleaved;
							break;
						case XK_A :		// For animation
						case XK_a :
							gbEnableAnimation = !gbEnableAnimation;
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

	XStoreName(gpDisplay, gWindow, "Interleaved Array");

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
	GLuint LoadBitmapAsTexture(const char*);	// Texture loading funtion

	// Variable declaration
	FILE *OGL_info = NULL;
	const int attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None };
	Bool bIsDirectContext;

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
	gVSObj = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *VSSrcCode =
		"#version 450 core \n" \
		"in vec4 vPosition;" \
		"in vec3 vColor, vNormal;" \
		"in vec2 vTexCoord;" \
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" \
		"uniform vec4 u_LPos;" \
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
		"uniform float u_KShine;" \
		"uniform int u_InterleavedEnabled;" \
		"out vec3 out_light;" \
		"out vec2 out_texCoord;" \
		"void main(void) {" \
			"out_light = vec3(0.0);" \
			"out_texCoord = vec2(0.0);" \
			"if(u_InterleavedEnabled == 1) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" \
				"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));" \
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
				"vec3 viewVector = normalize(-eyeCoords.xyz);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0) * vColor;" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
				
				"out_light = ambient + diffuse + specular;" \
				"out_texCoord = vTexCoord;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);
	ShaderErrorCheck(gVSObj, (char *)"VERTEX");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *FSSrcCode =
		"#version 450 core \n" \
		"in vec3 out_light;" \
		"in vec2 out_texCoord;" \
		"uniform sampler2D u_textureSampler;"
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 out_texture = texture(u_textureSampler, out_texCoord).rgb;" \
			"FragColor = vec4(out_light * out_texture, 1.0);" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);
	ShaderErrorCheck(gFSObj, (char *)"FRAGMENT");

	// Shader program
	gSPObj = glCreateProgram();
	glAttachShader(gSPObj, gVSObj);
	glAttachShader(gSPObj, gFSObj);
	glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
	glBindAttribLocation(gSPObj, DV_ATTRIB_COLOR, "vColor");
	glBindAttribLocation(gSPObj, DV_ATTRIB_NORM, "vNormal");
	glBindAttribLocation(gSPObj, DV_ATTRIB_TEX, "vTexCoord");
	glLinkProgram(gSPObj);
	ShaderErrorCheck(gSPObj, (char *)"PROGRAM");

	// Get uniform location(s)
	MMatrixUniform = glGetUniformLocation(gSPObj, "u_MMatrix");
	VMatrixUniform = glGetUniformLocation(gSPObj, "u_VMatrix");
	PMatrixUniform = glGetUniformLocation(gSPObj, "u_PMatrix");
	LAmbUniform = glGetUniformLocation(gSPObj, "u_LAmb");
	LDiffUniform = glGetUniformLocation(gSPObj, "u_LDiff");
	LSpecUniform = glGetUniformLocation(gSPObj, "u_LSpec");
	LPosUniform = glGetUniformLocation(gSPObj, "u_LPos");
	KAmbUniform = glGetUniformLocation(gSPObj, "u_KAmb");
	KDiffUniform = glGetUniformLocation(gSPObj, "u_KDiff");
	KSpecUniform = glGetUniformLocation(gSPObj, "u_KSpec");
	KShineUniform = glGetUniformLocation(gSPObj, "u_KShine");
	gTextureSamplerUniform = glGetUniformLocation(gSPObj, "u_textureSampler");
	InterleavedEnabledUniform = glGetUniformLocation(gSPObj, "u_InterleavedEnabled");

	// other variable initialization
	const GLfloat Cube[] = {
		// Front face (Top left) - Vertices, Color(red), Normals, TexCoords
		-1.0f, 1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f,
		// Front face (Bottom left) - Vertices, Color(red), Normals, TexCoords
		-1.0f, -1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		// Front face (Bottom right) - Vertices, Color(red), Normals, TexCoords
		1.0f, -1.0, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
		// Front face (Top right) - Vertices, Color(red), Normals, TexCoords
		1.0f, 1.0, 1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
		
		// Right face (Top left) - Vertices, Color(green), Normals, TexCoords
		1.0f, 1.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		// Right face (Botttom left) - Vertices, Color(green), Normals, TexCoords
		1.0f, -1.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		// Right face (Bottom right) - Vertices, Color(green), Normals, TexCoords
		1.0f, -1.0, -1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		// Right face (Top right) - Vertices, Color(green), Normals, TexCoords
		1.0f, 1.0, -1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		
		// Bottom face (Top left) - Vertices, Color(blue), Normals, TexCoords
		1.0f, -1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	0.0f, 1.0f,
		// Bottom face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, -1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	0.0f, 0.0f,
		// Bottom face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, -1.0, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	1.0f, 0.0f,
		// Bottom face (Top right) - Vertices, Color(blue), Normals, TexCoords
		1.0f, -1.0, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	1.0f, 1.0f,
		
		// Left face (Top left) - Vertices, Color(green), Normals, TexCoords
		-1.0f, 1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f,
		// Left face (Bottom left) - Vertices, Color(green), Normals, TexCoords
		-1.0f, -1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		// Left face (Bottom right) - Vertices, Color(green), Normals, TexCoords
		-1.0f, -1.0, 1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		// Left face (Top right) - Vertices, Color(green), Normals, TexCoords
		-1.0f, 1.0, 1.0f,	0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
		
		// Back face (Top left) - Vertices, Color(red), Normals, TexCoords
		1.0f, 1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f,
		// Back face (Bottom left) - Vertices, Color(red), Normals, TexCoords
		1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f,
		// Back face (Bottom right) - Vertices, Color(red), Normals, TexCoords
		-1.0f, -1.0, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f,
		// Back face (Top right) - Vertices, Color(red), Normals, TexCoords
		-1.0f, 1.0, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	1.0f, 1.0f,
		
		// Top face (Top left) - Vertices, Color(blue), Normals, TexCoords
		1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		// Top face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
		1.0f, 1.0f, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		// Top face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, 1.0, -1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		// Top face (Top right) - Vertices, Color(blue), Normals, TexCoords
		-1.0f, 1.0, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	};

	// For Quad
	glGenVertexArrays(1, &gVAObj);
	glBindVertexArray(gVAObj);
		glGenBuffers(1, &gVBObj);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), Cube, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(0 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glVertexAttribPointer(DV_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_COLOR);
		glVertexAttribPointer(DV_ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_NORM);
		glVertexAttribPointer(DV_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(9 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_TEX);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	marble_texture = LoadBitmapAsTexture("marble.png");
	glEnable(GL_TEXTURE_2D);

	gbEnableInterleaved = false;
	gbEnableAnimation = false;

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
		exit(1);
	}
}

GLuint LoadBitmapAsTexture(const char* path) {
	// Variable declaration
	int width, height;
	unsigned char *imageData = NULL;
	GLuint textureID;

	// Code
	imageData = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (const void *)imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(imageData);
	return textureID;
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
	mat4 modelMatrix, viewMatrix, projectionMatrix;
	mat4 translateMatrix, rotationMatrix;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Starting of OpenGL shading program
	glUseProgram(gSPObj);

	if(gbEnableInterleaved) {
		glUniform1i(InterleavedEnabledUniform, 1);
		glUniform3f(LAmbUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(LDiffUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(LSpecUniform, 0.0f, 0.0f, 0.0f);
		glUniform4f(LPosUniform, 0.0f, 0.0f, 2.0f, 1.0f);
		glUniform3f(KAmbUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(KDiffUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(KSpecUniform, 1.0f, 1.0f, 1.0f);
		glUniform1f(KShineUniform, 50.0f);
	}
	else
		glUniform1i(InterleavedEnabledUniform, 0);

	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = gPerspMatrix;
	translateMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	translateMatrix = translate(0.0f, 0.0f, -5.0f);
	rotationMatrix = rotate(gfAngle, 1.0f, 0.0f, 0.0f);
	modelMatrix = translateMatrix * rotationMatrix;
	rotationMatrix = rotate(gfAngle, 0.0f, 1.0f, 0.0f);
	modelMatrix *= rotationMatrix;
	rotationMatrix = rotate(gfAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix *= rotationMatrix;

	glUniformMatrix4fv(MMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(VMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(PMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// Texture enabling and sending it to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marble_texture);
	glUniform1i(gTextureSamplerUniform, 0);
	
	// OpenGL Drawing
	glBindVertexArray(gVAObj);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);

	glXSwapBuffers(gpDisplay, gWindow);
}

void Update(void) {
	// Code
	if(gbEnableAnimation)
		gfAngle += 0.1f;
	if(gfAngle >= 360.0f)
		gfAngle = 0.0f;
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
	if(gVAObj) {
		glDeleteVertexArrays(1, &gVAObj);
		gVAObj = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj) {
		glDeleteBuffers(1, &gVBObj);
		gVBObj = 0;
	}

	// Detach shaders
	glDetachShader(gSPObj, gVSObj);
	glDetachShader(gSPObj, gFSObj);

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
		glDeleteProgram(gSPObj);
		gSPObj = 0;
	}

	if(marble_texture) {
		glDeleteTextures(1, &marble_texture);
		marble_texture = 0;
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
}

