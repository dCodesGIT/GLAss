//
//  Per Vertex vs Per Fragment Lighting.mm
//  
//  Created by Darshan Vikam on 04/08/21.
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
	[window setTitle : @"PV vs PF Lighting"];
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
	
	GLuint gVSObj_PV;		// Vertex Shader Object
	GLuint gFSObj_PV;		// Fragment Shader Object
	GLuint gSPObj_PV;		// Shader Program Object
	GLuint gVSObj_PF;		// Vertex Shader Object
	GLuint gFSObj_PF;		// Fragment Shader Object
	GLuint gSPObj_PF;		// Shader Program Object
	GLuint gVAObj;			// Vertex Array Object
	GLuint gVBObj_sphere[2];	// Vertex Buffer Object

	GLuint gMMatrixUniform[2], gVMatrixUniform[2], gPMatrixUniform[2];	// Model, View, Projection Matrix Uniform
	GLuint gLAmbUniform[2], gLDiffUniform[2], gLSpecUniform[2], gLPosUniform[2];
	GLuint gKAmbUniform[2], gKDiffUniform[2], gKSpecUniform[2], gKShineUniform[2];
	GLuint gLightEnabledUniform[2];
	
	int pointCnt;
	GLfloat *sphereVertex, *sphereNormal;
	
	bool toggleLighting;		// false(PVL), true(PFL)
	bool lightEnabled;
	bool albedoEnabled;
	
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
	
	// Per Fragment Lighting
	// Vertex Shader
	gVSObj_PF = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode_PF = 				// Source code of shader
		"#version 410 core \n" \
		"in vec4 vPosition; \n" \
		"in vec3 vNormal; \n" \
		"uniform mat4 u_MMatrix; \n" \
		"uniform mat4 u_VMatrix; \n" \
		"uniform mat4 u_PMatrix; \n" \
		"uniform bool u_LightEnabled; \n" \
		"uniform vec4 u_LPos; \n" \
		"out vec3 out_tNorm, out_lSrc, out_viewVec; \n"\
		"void main(void) {" \
			"if(u_LightEnabled) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"out_tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" \
				"out_lSrc = vec3(u_LPos - eyeCoords);" \
				"out_viewVec = -eyeCoords.xyz;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj_PF, 1, (const GLchar**)&VSSrcCode_PF, NULL);
	glCompileShader(gVSObj_PF);				// Compile Shader
	[self shaderErrorCheck : gVSObj_PF option : "COMPILE"];
	fprintf(gfp_log, "PFL : Vertex Shader Compiled successfully...\n");

	// Fragment Shader
	gFSObj_PF = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode_PF = 			// Source code of shader
		"#version 410 core\n" \
		"in vec3 out_tNorm, out_lSrc, out_viewVec;" \
		"uniform bool u_LightEnabled;" \
		"uniform vec3 u_LAmb;" \
		"uniform vec3 u_LDiff;" \
		"uniform vec3 u_LSpec;" \
		"uniform vec3 u_KAmb;" \
		"uniform vec3 u_KDiff;" \
		"uniform vec3 u_KSpec;" \
		"uniform float u_KShine;" \
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
	glShaderSource(gFSObj_PF, 1, (const GLchar**)&FSSrcCode_PF, NULL);
	glCompileShader(gFSObj_PF);				// Compile Shader
	[self shaderErrorCheck : gFSObj_PF option : "COMPILE"];
	fprintf(gfp_log, "PFL : Fragment Shader Compiled successfully...\n");

	// Shader program
	gSPObj_PF = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj_PF, gVSObj_PF);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj_PF, gFSObj_PF);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj_PF, DV_ATTRIB_POS, "vPosition");
	glBindAttribLocation(gSPObj_PF, DV_ATTRIB_NORM, "vNormal");
	glLinkProgram(gSPObj_PF);
	[self shaderErrorCheck : gSPObj_PF option : "LINK"];
	fprintf(gfp_log, "PFL : Shader Program Compiled successfully...\n");
	
	// Per Vertex Lighting
	// Vertex Shader
	gVSObj_PV = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode_PV = 				// Source code of shader
		"#version 410 core\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_MMatrix;" \
		"uniform mat4 u_VMatrix;" \
		"uniform mat4 u_PMatrix;" \
		"uniform bool u_LightEnabled;" \
		"uniform vec4 u_LPos;" \
		"uniform vec3 u_LAmb;" \
		"uniform vec3 u_LDiff;" \
		"uniform vec3 u_LSpec;" \
		"uniform vec3 u_KAmb;" \
		"uniform vec3 u_KDiff;" \
		"uniform vec3 u_KSpec;" \
		"uniform float u_KShine;" \
		"out vec3 out_light;"\
		"void main(void) {" \
			"out_light = vec3(0.0);" \
			"if(u_LightEnabled) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" \
				"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));" \
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
				"vec3 viewVector = normalize(-eyeCoords.xyz);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0);" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
				"out_light = ambient + diffuse + specular;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj_PV, 1, (const GLchar**)&VSSrcCode_PV, NULL);
	glCompileShader(gVSObj_PV);				// Compile Shader
	[self shaderErrorCheck : gVSObj_PV option : "COMPILE"];
	fprintf(gfp_log, "PVL : Vertex Shader Compiled successfully...\n");

	// Fragment Shader
	gFSObj_PV = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode_PV = 			// Source code of shader
		"#version 410 core\n" \
		"in vec3 out_light;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = vec4(out_light, 1.0);" \
		"}";
	glShaderSource(gFSObj_PV, 1, (const GLchar**)&FSSrcCode_PV, NULL);
	glCompileShader(gFSObj_PV);				// Compile Shader
	[self shaderErrorCheck : gFSObj_PV option : "COMPILE"];
	fprintf(gfp_log, "PVL : Fragment Shader Compiled successfully...\n");

	// Shader program
	gSPObj_PV = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj_PV, gVSObj_PV);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj_PV, gFSObj_PV);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj_PV, DV_ATTRIB_POS, "vPosition");
	glBindAttribLocation(gSPObj_PV, DV_ATTRIB_NORM, "vNormal");
	glLinkProgram(gSPObj_PV);
	[self shaderErrorCheck : gSPObj_PV option : "LINK"];
	fprintf(gfp_log, "PVL : Shader Program Compiled successfully...\n");

	// get uniform location(s)
	gMMatrixUniform[0] = glGetUniformLocation(gSPObj_PV, "u_MMatrix");
	gVMatrixUniform[0] = glGetUniformLocation(gSPObj_PV, "u_VMatrix");
	gPMatrixUniform[0] = glGetUniformLocation(gSPObj_PV, "u_PMatrix");
	gLAmbUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LAmb");
	gLDiffUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LDiff");
	gLSpecUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LSpec");
	gLPosUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LPos");
	gKAmbUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KAmb");
	gKDiffUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KDiff");
	gKSpecUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KSpec");
	gKShineUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KShine");
	gLightEnabledUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LightEnabled");

	gMMatrixUniform[1] = glGetUniformLocation(gSPObj_PF, "u_MMatrix");
	gVMatrixUniform[1] = glGetUniformLocation(gSPObj_PF, "u_VMatrix");
	gPMatrixUniform[1] = glGetUniformLocation(gSPObj_PF, "u_PMatrix");
	gLAmbUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LAmb");
	gLDiffUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LDiff");
	gLSpecUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LSpec");
	gLPosUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LPos");
	gKAmbUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KAmb");
	gKDiffUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KDiff");
	gKSpecUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KSpec");
	gKShineUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KShine");
	gLightEnabledUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LightEnabled");

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
	
	toggleLighting = true;
	lightEnabled = false;
	albedoEnabled = false;
	
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
	glViewport(0, 0, (GLsizei)rect.size.width, (GLsizei)rect.size.height);
	
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
	vec3 lightAmbient, lightDiffuse, lightSpecular;
	vec4 lightPosition;
	vec3 materialAmbient, materialDiffuse, materialSpecular;
	GLfloat materialShininess;
	
	// Code
	[[self openGLContext] makeCurrentContext];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// OpenGL Code starts here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(toggleLighting)
		glUseProgram(gSPObj_PV);
	else
		glUseProgram(gSPObj_PF);
	
	if(albedoEnabled) {
		lightAmbient = vec3(0.1f, 0.1f, 0.1f);
		lightDiffuse = vec3(1.0f, 1.0f, 1.0f);
		lightSpecular = vec3(1.0f, 1.0f, 1.0f);
		lightPosition = vec4(100.0f, 100.0f, 100.0f, 1.0f);
		materialAmbient = vec3(0.0f, 0.0f, 0.0f);
		materialDiffuse = vec3(0.5f, 0.2f, 0.7f);
		materialSpecular = vec3(0.7f, 0.7f, 0.7f);
		materialShininess = 128.0f;
	}
	else {
		lightAmbient = vec3(0.0f, 0.0f, 0.0f);
		lightDiffuse = vec3(1.0f, 1.0f, 1.0f);
		lightSpecular = vec3(1.0f, 1.0f, 1.0f);
		lightPosition = vec4(100.0f, 100.0f, 100.0f, 1.0f);
		materialAmbient = vec3(0.0f, 0.0f, 0.0f);
		materialDiffuse = vec3(1.0f, 1.0f, 1.0f);
		materialSpecular = vec3(1.0f, 1.0f, 1.0f);
		materialShininess = 50.0f;
	}
	int ch = toggleLighting ? 0 : 1;
	if(lightEnabled) {
		glUniform1i(gLightEnabledUniform[ch], true);
		glUniform3fv(gLAmbUniform[ch], 1, lightAmbient);
		glUniform3fv(gLDiffUniform[ch], 1, lightDiffuse);
		glUniform3fv(gLSpecUniform[ch], 1, lightSpecular);
		glUniform4fv(gLPosUniform[ch], 1, lightPosition);
		glUniform3fv(gKAmbUniform[ch], 1, materialAmbient);
		glUniform3fv(gKDiffUniform[ch], 1, materialDiffuse);
		glUniform3fv(gKSpecUniform[ch], 1, materialSpecular);
		glUniform1fv(gKShineUniform[ch], 1, &materialShininess);
	}
	else
		glUniform1i(gLightEnabledUniform[ch], false);

	// Initialization
	MMatrix = mat4::identity();
	VMatrix = mat4::identity();
	PMatrix = gPerspProjMatrix;
	translationMatrix = mat4::identity();
	// Setting values
	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	VMatrix *= translationMatrix;
	glUniformMatrix4fv(gMMatrixUniform[ch], 1, GL_FALSE, MMatrix);
	glUniformMatrix4fv(gVMatrixUniform[ch], 1, GL_FALSE, VMatrix);
	glUniformMatrix4fv(gPMatrixUniform[ch], 1, GL_FALSE, PMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj);
		glDrawArrays(GL_TRIANGLES, 0, pointCnt);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);
	
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
		case 'a' :
		case 'A' :
			albedoEnabled = ! albedoEnabled;
			break;
		case 't' :
		case 'T' :
			toggleLighting = ! toggleLighting;
//			if(toggleLighting) {
//				gMMatrixUniform = glGetUniformLocation(gSPObj_PF, "u_MMatrix");
//				gVMatrixUniform = glGetUniformLocation(gSPObj_PF, "u_VMatrix");
//				gPMatrixUniform = glGetUniformLocation(gSPObj_PF, "u_PMatrix");
//				gLAmbUniform = glGetUniformLocation(gSPObj_PF, "u_LAmb");
//				gLDiffUniform = glGetUniformLocation(gSPObj_PF, "u_LDiff");
//				gLSpecUniform = glGetUniformLocation(gSPObj_PF, "u_LSpec");
//				gLPosUniform = glGetUniformLocation(gSPObj_PF, "u_LPos");
//				gKAmbUniform = glGetUniformLocation(gSPObj_PF, "u_KAmb");
//				gKDiffUniform = glGetUniformLocation(gSPObj_PF, "u_KDiff");
//				gKSpecUniform = glGetUniformLocation(gSPObj_PF, "u_KSpec");
//				gKShineUniform = glGetUniformLocation(gSPObj_PF, "u_KShine");
//				gLightEnabledUniform = glGetUniformLocation(gSPObj_PF, "u_LightEnabled");
//				fprintf(gfp_log, "Per Fragment : %d \t %d \t %d \t %d", gMMatrixUniform, gVMatrixUniform, gPMatrixUniform, gLightEnabledUniform);
//				fprintf(gfp_log, "\t %d \t %d \t %d \t %d", gLAmbUniform, gLDiffUniform, gLSpecUniform, gLPosUniform);
//				fprintf(gfp_log, "\t %d \t %d \t %d \t %d \n", gKAmbUniform, gKDiffUniform, gKSpecUniform, gKShineUniform);
//			}
//			else {
//				gMMatrixUniform = glGetUniformLocation(gSPObj_PV, "u_MMatrix");
//				gVMatrixUniform = glGetUniformLocation(gSPObj_PV, "u_VMatrix");
//				gPMatrixUniform = glGetUniformLocation(gSPObj_PV, "u_PMatrix");
//				gLAmbUniform = glGetUniformLocation(gSPObj_PV, "u_LAmb");
//				gLDiffUniform = glGetUniformLocation(gSPObj_PV, "u_LDiff");
//				gLSpecUniform = glGetUniformLocation(gSPObj_PV, "u_LSpec");
//				gLPosUniform = glGetUniformLocation(gSPObj_PV, "u_LPos");
//				gKAmbUniform = glGetUniformLocation(gSPObj_PV, "u_KAmb");
//				gKDiffUniform = glGetUniformLocation(gSPObj_PV, "u_KDiff");
//				gKSpecUniform = glGetUniformLocation(gSPObj_PV, "u_KSpec");
//				gKShineUniform = glGetUniformLocation(gSPObj_PV, "u_KShine");
//				gLightEnabledUniform = glGetUniformLocation(gSPObj_PV, "u_LightEnabled");
//				fprintf(gfp_log, "Per Vertex : %d \t %d \t %d \t %d", gMMatrixUniform, gVMatrixUniform, gPMatrixUniform, gLightEnabledUniform);
//				fprintf(gfp_log, "\t %d \t %d \t %d \t %d", gLAmbUniform, gLDiffUniform, gLSpecUniform, gLPosUniform);
//				fprintf(gfp_log, "\t %d \t %d \t %d \t %d \n", gKAmbUniform, gKDiffUniform, gKSpecUniform, gKShineUniform);
//			}
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

	// Detach shaders - Per Fragment Lighting
	glDetachShader(gSPObj_PF, gVSObj_PF);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj_PF, gFSObj_PF);		// Detach fragment shader from final shader program
	
	// Delete shaders - Per Fragment Lighting
	if(gVSObj_PF != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj_PF);
		gVSObj_PF = 0;
	}
	if(gFSObj_PF != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj_PF);
		gFSObj_PF = 0;
	}
	if(gSPObj_PF != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj_PF);
		gSPObj_PF = 0;
	}
	
	// Detach shaders - Per Vertex Lighting
	glDetachShader(gSPObj_PV, gVSObj_PV);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj_PV, gFSObj_PV);		// Detach fragment shader from final shader program
	
	// Delete shaders - Per Vertex Lighting
	if(gVSObj_PV != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj_PV);
		gVSObj_PV = 0;
	}
	if(gFSObj_PV != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj_PV);
		gFSObj_PV = 0;
	}
	if(gSPObj_PV != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj_PV);
		gSPObj_PV = 0;
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
