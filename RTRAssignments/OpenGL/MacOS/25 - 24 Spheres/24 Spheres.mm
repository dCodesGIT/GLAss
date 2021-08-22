//
//  24 Spheres.mm
//  
//  Created by Darshan Vikam on 05/08/21.
//

// Importing required headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>			// gl.h - for legacy OpenGL (FFP)
#import "../Include/vmath.h"
#import "../Include/sphere.h"

// Call back function
CVReturn myDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *);

// Global variables
FILE *gfp_log = NULL;

using namespace vmath;

// Declaring interfaces
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface OpenGLView : NSOpenGLView
@end

// Entry point function - main()
int main(int argc, char* argv[]) {
	// Code
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSApp = [NSApplication sharedApplication];		// NSApp is global variable given by MacOS
	[NSApp setDelegate : [[AppDelegate alloc] init]];
	
	[NSApp run];            // Run loop - similar to message loop or game loop
	
	[pool release];
	
	return 0;
}

// Implementation of AppDelegate interface
@implementation AppDelegate {
	@private
	NSWindow *window;
	OpenGLView *glView;
}
-(void)applicationDidFinishLaunching : (NSNotification *)aNotification {
	// Code
	NSBundle *appBundle = [NSBundle mainBundle];
	NSString *appLocn = [appBundle bundlePath];
	const char *logFileName = [[NSString stringWithFormat : @"%@/log.txt", [appLocn stringByDeletingLastPathComponent]] cStringUsingEncoding : NSASCIIStringEncoding];
	gfp_log = fopen(logFileName, "w");
	if(gfp_log == NULL) {
		[self release];
		[NSApp terminate : self];
	}
	fprintf(gfp_log, "Log file created successfully...\n");
	fprintf(gfp_log, "Program started successfully..\n");
	
	NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);
	window = [[NSWindow alloc] initWithContentRect : win_rect
			styleMask : NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
			backing : NSBackingStoreBuffered
			defer : NO];
	[window setTitle : @"24 Spheres"];
	[window center];
	
	glView = [[OpenGLView alloc] initWithFrame : win_rect];
	
	[window setContentView : glView];
	[window setDelegate : self];
	[window makeKeyAndOrderFront : self];
}
-(void)applicationWillTerminate : (NSNotification *)aNotification {
	// Code
	if(gfp_log) {
		fprintf(gfp_log, "Program terminated successfully...\n");
		fprintf(gfp_log, "Log file closing successfully...");
		fclose(gfp_log);
		gfp_log = NULL;
	}
}
-(void)windowWillClose : (NSNotification *)aNotification {
	// Code
	[NSApp terminate : self];
}
-(void)dealloc {
	// Code
	[glView release];
	[window release];
	[super dealloc];
}
@end

// Implementation of OpenGLView interface
@implementation OpenGLView {
	@private
	CVDisplayLinkRef displayLink;
	
	MySphere *sphere;
	
	// enum declaration
	enum {
		DV_ATTRIB_POS = 0,
		DV_ATTRIB_COL,
		DV_ATTRIB_NORM,
		DV_ATTRIB_TEX,
	};
	
	GLuint gVSObj;		// Vertex Shader Object
	GLuint gFSObj;		// Fragment Shader Object
	GLuint gSPObj;		// Shader Program Object
	GLuint gVAObj;		// Vertex Array Object
	GLuint gVBObj_sphere[2];	// Vertex Buffer Object

	GLuint gMMatrixUniform, gVMatrixUniform, gPMatrixUniform;	// Model, View, Projection Matrix Uniform
	GLuint gLAmbUniform, gLDiffUniform, gLSpecUniform, gLPosUniform;
	GLuint gKAmbUniform, gKDiffUniform, gKSpecUniform, gKShineUniform;
	GLuint gLightEnabledUniform;
	
	int pointCnt;
	GLfloat *sphereVertex, *sphereNormal;
	
	bool lightEnabled;
	bool xRotEnabled, yRotEnabled, zRotEnabled;
	GLfloat angle;
	
	float gWidth, gHeight;
	
	mat4 gPerspProjMatrix;
}
-(id)initWithFrame : (NSRect)frame {
	// Code
	self = [super initWithFrame : frame];
	if(self) {
		NSOpenGLPixelFormatAttribute pfa[] = { NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
				NSOpenGLPFAScreenMask, CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
				NSOpenGLPFANoRecovery,
				NSOpenGLPFAAccelerated,
				NSOpenGLPFAColorSize, 24,
				NSOpenGLPFADepthSize, 24,
				NSOpenGLPFAAlphaSize, 8,
				NSOpenGLPFADoubleBuffer, 0 };
		
		NSOpenGLPixelFormat *pixelFormat = [[[NSOpenGLPixelFormat alloc] initWithAttributes : pfa] autorelease];
		if(pixelFormat == nil) {
			fprintf(gfp_log, "Unable to get Pixel Format.\n");
			[self release];
			[NSApp terminate : self];
		}
		
		NSOpenGLContext *glContext = [[[NSOpenGLContext alloc] initWithFormat : pixelFormat shareContext : nil] autorelease];
		if(glContext == nil) {
			fprintf(gfp_log, "Unable to get OpenGL Context.\n");
			[self release];
			[NSApp terminate : self];
		}
		
		[self setPixelFormat : pixelFormat];
		[self setOpenGLContext : glContext];
	}
	return self;
}
-(CVReturn)getFrameForTime : (const CVTimeStamp *)outputTime {
	// Code
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[self drawView];
	[pool release];
	return (kCVReturnSuccess);
}
-(void)prepareOpenGL {
	// Code
	[super prepareOpenGL];
	[[self openGLContext] makeCurrentContext];
	
	// Swap interval
	GLint swapInterval = 1;
	[[self openGLContext] setValues : &swapInterval forParameter : NSOpenGLCPSwapInterval];
	
	// Vertex Shader
	gVSObj = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode = 				// Source code of shader
		"#version 410 core\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
		"uniform vec4 u_LPos;" \
		"uniform bool u_LightEnabled;" \
		"out vec3 out_tNorm, out_lSrc, out_viewVec;"\
		"void main(void) {" \
			"if(u_LightEnabled) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"out_tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" \
				"out_lSrc = vec3(u_LPos - eyeCoords);" \
				"out_viewVec = -eyeCoords.xyz;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);				// Compile Shader
	[self shaderErrorCheck : gVSObj option : "COMPILE"];
	fprintf(gfp_log, "Vertex Shader Compiled successfully...\n");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 410 core\n" \
		"in vec3 out_tNorm, out_lSrc, out_viewVec;" \
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" \
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
		"uniform float u_KShine;" \
		"uniform bool u_LightEnabled;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 light = vec3(0.0);" \
			"if(u_LightEnabled) {" \
				"vec3 transformedNormal = normalize(out_tNorm);" \
				"vec3 lightSource = normalize(out_lSrc);" \
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
				"vec3 viewVector = normalize(out_viewVec);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0);" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
				"light = ambient + diffuse + specular;" \
			"}" \
		"FragColor = vec4(light, 1.0);" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);				// Compile Shader
	[self shaderErrorCheck : gFSObj option : "COMPILE"];
	fprintf(gfp_log, "Fragment Shader Compiled successfully...\n");

	// Shader program
	gSPObj = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj, gVSObj);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj, gFSObj);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
	glBindAttribLocation(gSPObj, DV_ATTRIB_NORM, "vNormal");
	glLinkProgram(gSPObj);
	[self shaderErrorCheck : gSPObj option : "LINK"];
	fprintf(gfp_log, "Shader Program Compiled successfully...\n");

	// get uniform location(s)
	gMMatrixUniform = glGetUniformLocation(gSPObj, "u_MMatrix");
	gVMatrixUniform = glGetUniformLocation(gSPObj, "u_VMatrix");
	gPMatrixUniform = glGetUniformLocation(gSPObj, "u_PMatrix");
	gLAmbUniform = glGetUniformLocation(gSPObj, "u_LAmb");
	gLDiffUniform = glGetUniformLocation(gSPObj, "u_LDiff");
	gLSpecUniform = glGetUniformLocation(gSPObj, "u_LSpec");
	gLPosUniform = glGetUniformLocation(gSPObj, "u_LPos");
	gKAmbUniform = glGetUniformLocation(gSPObj, "u_KAmb");
	gKDiffUniform = glGetUniformLocation(gSPObj, "u_KDiff");
	gKSpecUniform = glGetUniformLocation(gSPObj, "u_KSpec");
	gKShineUniform = glGetUniformLocation(gSPObj, "u_KShine");
	gLightEnabledUniform = glGetUniformLocation(gSPObj, "u_LightEnabled");
	
	// other global variable initialization
	sphere = [[MySphere alloc] init];
	pointCnt = [sphere GenSphere : 1.0f stack : 30 slices : 30];
	sphereVertex = (GLfloat *)[sphere getSphereVertices];
	sphereNormal = (GLfloat *)[sphere getSphereNormals];
	
	glGenVertexArrays(1, &gVAObj);
	glBindVertexArray(gVAObj);
		glGenBuffers(2, gVBObj_sphere);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_sphere[0]);
		glBufferData(GL_ARRAY_BUFFER, pointCnt * 3 * sizeof(float), sphereVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_sphere[1]);
		glBufferData(GL_ARRAY_BUFFER, pointCnt * 3 * sizeof(float), sphereNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_NORM);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Depth related OpenGL code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	lightEnabled = false;
	xRotEnabled = false;
	yRotEnabled = false;
	zRotEnabled = false;
	
	gPerspProjMatrix = mat4::identity();
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Core Video and Core Graphics related code
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, &myDisplayLinkCallback, self);
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	CVDisplayLinkStart(displayLink);
}
-(void)shaderErrorCheck : (GLuint)shaderObject option : (const char *)shaderOpr {
	// Variable declaration
	GLint iErrorLen = 0;
	GLint iStatus = 0;
	char *szError = NULL;

	// Code
	if(strcmp(shaderOpr, "COMPILE") == 0)
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &iStatus);
	else if(strcmp(shaderOpr, "LINK") == 0)
		glGetProgramiv(shaderObject, GL_LINK_STATUS, &iStatus);
	else {
		fprintf(gfp_log, "Invalid second parameter in ShaderErrorCheck()");
		return;
	}
	if(iStatus == GL_FALSE) {
		if(strcmp(shaderOpr, "COMPILE") == 0)
			glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &iErrorLen);
		else
			glGetProgramiv(shaderObject, GL_INFO_LOG_LENGTH, &iErrorLen);
		if(iErrorLen > 0) {
			szError = (char *)malloc(iErrorLen);
			if(szError != NULL) {
				GLsizei written;
				if(strcmp(shaderOpr, "COMPILE") == 0) {
					glGetShaderInfoLog(shaderObject, iErrorLen, &written, szError);
					fprintf(gfp_log, "Shader Compilation Error log : \n");
				}
				else if(strcmp(shaderOpr, "LINK") == 0) {
					glGetProgramInfoLog(shaderObject, iErrorLen, &written, szError);
					fprintf(gfp_log, "Shader linking Error log : \n");
				}
				fprintf(gfp_log, "%s \n", szError);
				free(szError);
				szError = NULL;
			}
		}
		else
			fprintf(gfp_log, "Error occured during compilation. No error message. \n");
		[self release];
		[NSApp terminate : self];
	}
}
-(void)reshape {
	//Code
	[super reshape];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

	NSRect rect = [self bounds];
	if(rect.size.height < 0) {
		rect.size.height = 1;
	}
//	glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
	
	gWidth = rect.size.width;
	gHeight = rect.size.height;
	
	gPerspProjMatrix = perspective(45.0f, (GLfloat)rect.size.width/(GLfloat)rect.size.height, 0.1f, 100.0f);
	
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}
-(void)drawRect : (NSRect)dirtyRect {
	// Code
	[self drawView];
}
-(void) drawView {		// Display() in windows
	// Variable declaration
	mat4 MMatrix, VMatrix, PMatrix;
	mat4 translationMatrix, rotationMatrix;
	GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[4];
	GLfloat materialAmbient[][3] =
	{
		{0.0215f, 0.1745f, 0.0215f},	// 1R 1C - Emerald
		{0.135f, 0.2225f, 0.1575f},	// 2R 1C - Jade
		{0.05375f, 0.05f, 0.06625f},	// 3R 1C - Obsidian
		{0.25f, 0.20725f, 0.20725f},	// 4R 1C - Pearl
		{0.1745f, 0.01175f, 0.01175f},	// 5R 1C - Ruby
		{0.1f, 0.18725f, 0.1745f},	// 6R 1C - Turquoise
		{0.329412f, 0.223529f, 0.027451f},// 1R 2C - Brass
		{0.2125f, 0.1275f, 0.054f},	// 2R 2C - Bronze
		{0.25f, 0.25f, 0.25f},		// 3R 2C - Chrome
		{0.19125f, 0.0735f, 0.0225f},	// 4R 2C - Copper
		{0.24725f, 0.1995f, 0.0745f},	// 5R 2C - Gold
		{0.19225f, 0.19225f, 0.19225f},	// 6R 2C - Silver
		{0.0f, 0.0f, 0.0f},		// 1R 3C - Black plastic
		{0.0f, 0.1f, 0.06f},		// 2R 3C - Cyan plastic
		{0.0f, 0.0f, 0.0f},		// 3R 3C - Green plastic
		{0.0f, 0.0f, 0.0f},		// 4R 3C - Red plastic
		{0.0f, 0.0f, 0.0f},		// 5R 3C - White plastic
		{0.0f, 0.0f, 0.0f},		// 6R 3C - Yellow plastic
		{0.02f, 0.02f, 0.02f},		// 1R 4C - Black rubber
		{0.0f, 0.05f, 0.05f},		// 2R 4C - Cyan rubber
		{0.0f, 0.05f, 0.0f},		// 3R 4C - Green rubber
		{0.05f, 0.0f, 0.0f},		// 4R 4C - Red rubber
		{0.05f, 0.05f, 0.05f},		// 5R 4C - White rubber
		{0.05f, 0.05f, 0.04f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialDiffuse[][3] =
	{
		{0.07568f, 0.61424f, 0.07568f},	// 1R 1C - Emerald
		{0.54f, 0.89f, 0.63f},		// 2R 1C - Jade
		{0.18275f, 0.17f, 0.22525f},	// 3R 1C - Obsidian
		{1.0f, 0.829f, 0.829f},		// 4R 1C - Pearl
		{0.61424f, 0.04136f, 0.04163f},	// 5R 1C - Ruby
		{0.396f, 0.74151f, 0.69102f},	// 6R 1C - Turquoise
		{0.780392f, 0.568627f, 0.113725f},// 1R 2C - Brass
		{0.714f, 0.4284f, 0.18144f},	// 2R 2C - Bronze
		{0.4f, 0.4f, 0.4f},		// 3R 2C - Chrome
		{0.7038f, 0.27048f, 0.0828f},	// 4R 2C - Copper
		{0.75164f, 0.60648f, 0.22648f},	// 5R 2C - Gold
		{0.50754f, 0.50754f, 0.50754f},	// 6R 2C - Silver
		{0.01f, 0.01f, 0.01f},		// 1R 3C - Black plastic
		{0.0f, 0.50980392f, 0.50980392f},// 2R 3C - Cyan plastic
		{0.1f, 0.35f, 0.1f},		// 3R 3C - Green plastic
		{0.5f, 0.0f, 0.0f},		// 4R 3C - Red plastic
		{0.55f, 0.55f, 0.55f},		// 5R 3C - White plastic
		{0.5f, 0.5f, 0.0f},		// 6R 3C - Yellow plastic
		{0.01f, 0.01f, 0.01f},		// 1R 4C - Black rubber
		{0.4f, 0.5f, 0.5f},		// 2R 4C - Cyan rubber
		{0.4f, 0.5f, 0.4f},		// 3R 4C - Green rubber
		{0.5f, 0.4f, 0.4f},		// 4R 4C - Red rubber
		{0.5f, 0.5f, 0.5},		// 5R 4C - White rubber
		{0.5f, 0.5f, 0.4f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialSpecular[][3] =
	{
		{0.633f, 0.727811f, 0.33f},		// 1R 1C - Emerald
		{0.316228f, 0.316228f, 0.316228f},	// 2R 1C - Jade
		{0.332741f, 0.328634f, 0.346435f},	// 3R 1C - Obsidian
		{0.296648f, 0.296648f, 0.296648f},	// 4R 1C - Pearl
		{0.727811f, 0.626959f, 0.626959f},	// 5R 1C - Ruby
		{0.297254f, 0.308290f, 0.306678f},	// 6R 1C - Turquoise
		{0.992157f, 0.941176f, 0.807843f},	// 1R 2C - Brass
		{0.393548f, 0.271906f, 0.166721f},	// 2R 2C - Bronze
		{0.774597f, 0.774597f, 0.774597f},	// 3R 2C - Chrome
		{0.256777f, 0.137622f, 0.086014f},	// 4R 2C - Copper
		{0.628281f, 0.555802f, 0.366065f},	// 5R 2C - Gold
		{0.508273f, 0.508273f, 0.508273f},	// 6R 2C - Silver
		{0.5f, 0.5f, 0.5f},			// 1R 3C - Black plastic
		{0.50196078f, 0.50196078f, 0.50196078f},// 2R 3C - Cyan plastic
		{0.45f, 0.55f, 0.45f},			// 3R 3C - Green plastic
		{0.7f, 0.6f, 0.6f},			// 4R 3C - Red plastic
		{0.7f, 0.7f, 0.7f},			// 5R 3C - White plastic
		{0.6f, 0.6f, 0.5f},			// 6R 3C - Yellow plastic
		{0.4f, 0.4f, 0.4f},			// 1R 4C - Black rubber
		{0.04f, 0.7f, 0.7f},			// 2R 4C - Cyan rubber
		{0.04f, 0.7f, 0.04f},			// 3R 4C - Green rubber
		{0.7f, 0.04f, 0.04f},			// 4R 4C - Red rubber
		{0.7f, 0.7f, 0.7f},			// 5R 4C - White rubber
		{0.7f, 0.7f, 0.04f}			// 6R 4C - Yellow rubber
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
	const GLfloat radian = M_PI / 180.0f;
	const GLfloat radius = 100.0f;

	// Code
	[[self openGLContext] makeCurrentContext];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// OpenGL Code starts here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 6; j++) {
			glViewport((gWidth / 4) * i, (gHeight / 6) * (5-j), (GLsizei)(gWidth/4), (GLsizei)(gHeight/6));
			
			if(xRotEnabled) {
				lightPosition[0] = 0.0f;
				lightPosition[1] = radius * (GLfloat)cos(angle * radian);
				lightPosition[2] = radius * (GLfloat)sin(angle * radian);
			}
			else if(yRotEnabled) {
				lightPosition[0] = radius * (GLfloat)sin(angle * radian);
				lightPosition[1] = 0.0f;
				lightPosition[2] = radius * (GLfloat)cos(angle * radian);
			}
			else if(zRotEnabled) {
				lightPosition[0] = radius * (GLfloat)cos(angle * radian);
				lightPosition[1] = radius * (GLfloat)sin(angle * radian);
				lightPosition[2] = 0.0f;
			}
			else {
				lightPosition[0] = radius;
				lightPosition[1] = radius;
				lightPosition[2] = radius;
				lightPosition[3] = 1.0f;
			}
			
			if(lightEnabled) {
				glUniform1i(gLightEnabledUniform, true);
				glUniform3fv(gLAmbUniform, 1, lightAmbient);
				glUniform3fv(gLDiffUniform, 1, lightDiffuse);
				glUniform3fv(gLSpecUniform, 1, lightSpecular);
				glUniform4fv(gLPosUniform, 1, lightPosition);
				glUniform3fv(gKAmbUniform, 1, materialAmbient[(i*6)+j]);
				glUniform3fv(gKDiffUniform, 1, materialDiffuse[(i*6)+j]);
				glUniform3fv(gKSpecUniform, 1, materialSpecular[(i*6)+j]);
				GLfloat matShine = materialShininess[(i*6)+j] * 128.0f;
				glUniform1fv(gKShineUniform, 1, &matShine);
			}
			else
				glUniform1i(gLightEnabledUniform, false);

			// Initialization
			MMatrix = mat4::identity();
			VMatrix = mat4::identity();
			PMatrix = gPerspProjMatrix;
			translationMatrix = mat4::identity();
			// Setting values
			translationMatrix = translate(0.0f, 0.0f, -3.0f);
			VMatrix *= translationMatrix;
			glUniformMatrix4fv(gMMatrixUniform, 1, GL_FALSE, MMatrix);
			glUniformMatrix4fv(gVMatrixUniform, 1, GL_FALSE, VMatrix);
			glUniformMatrix4fv(gPMatrixUniform, 1, GL_FALSE, PMatrix);
			// OpenGL Drawing
			glBindVertexArray(gVAObj);
				glDrawArrays(GL_TRIANGLES, 0, pointCnt);
			glBindVertexArray(0);
		}
	}

	// End of OpenGL shading program
	glUseProgram(0);
	
	if(xRotEnabled || yRotEnabled || zRotEnabled)
		angle += 0.5f;
	if(angle >= 360.0f)
		angle = 0.0f;
	
	CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}
-(BOOL)acceptsFirstResponder {
	// Code
	[[self window] makeFirstResponder : nil];
	return YES;
}
-(void)keyDown : (NSEvent *)theEvent {
	// Code
	int key = [[theEvent characters] characterAtIndex : 0];
	switch(key) {
		case 27 :		// ESC
			[self release];
			[NSApp terminate : self];
			break;
		case 'F' :
		case 'f' :
			[[self window] toggleFullScreen : self];
			break;
		case 'l' :
		case 'L' :
			lightEnabled = ! lightEnabled;
			break;
		case 'x' :
		case 'X' :
			xRotEnabled = true;
			yRotEnabled = false;
			zRotEnabled = false;
			angle = 0.0f;
			break;
		case 'y' :
		case 'Y' :
			xRotEnabled = false;
			yRotEnabled = true;
			zRotEnabled = false;
			angle = 0.0f;
			break;
		case 'z' :
		case 'Z' :
			xRotEnabled = false;
			yRotEnabled = false;
			zRotEnabled = true;
			angle = 0.0f;
			break;
		default :
			xRotEnabled = false;
			yRotEnabled = false;
			zRotEnabled = false;
			angle = 0.0f;
			break;
	}
}
-(void)mouseDown : (NSEvent *)theEvent {
	// Code
}
-(void)rightMouseDown : (NSEvent *)theEvent {
	// Code
}
-(void)otherMouseDown : (NSEvent *)theEvent {
	// Code
}
-(void)dealloc {
	// Code
	CVDisplayLinkStop(displayLink);
	CVDisplayLinkRelease(displayLink);
	
	// Delete sphere
	[sphere deleteSphere];
	
	// Unlink shader program (if not unlinked earlier)
	glUseProgram(0);
	
	// Destroy Vertex Array Object
	if(gVAObj) {
		glDeleteVertexArrays(1, &gVAObj);
		gVAObj = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_sphere) {
		glDeleteBuffers(2, gVBObj_sphere);
		gVBObj_sphere[0] = 0;
		gVBObj_sphere[1] = 0;
	}

	// Detach shaders
	glDetachShader(gSPObj, gVSObj);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj, gFSObj);		// Detach fragment shader from final shader program
	
	// Delete shaders
	if(gVSObj != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gFSObj != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj);
		gFSObj = 0;
	}
	if(gSPObj != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj);
		gSPObj = 0;
	}
	
	[super dealloc];
}
@end

// Definition of callback function
CVReturn myDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *curr, const CVTimeStamp *outTime, CVOptionFlags flagsIn, CVOptionFlags * flagsOut, void *displayLinkContext) {
	// Code
	CVReturn result = [(OpenGLView *)displayLinkContext getFrameForTime : outTime];
	return result;
}
