//
//  Robotic arm.mm
//  
//  Created by Darshan Vikam on 01/08/21.
//

// Importing required headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>			// gl.h - for legacy OpenGL (FFP)
#import "../Include/vmath.h"
#import "../Include/sphere.h"
#import "../Include/PushPop.h"

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
	[window setTitle : @"Robotic arm"];
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
	
	// enum declaration
	enum {
		DV_ATTRIB_POS = 0,
		DV_ATTRIB_COL,
		DV_ATTRIB_NORM,
		DV_ATTRIB_TEX,
	};
	
	GLuint gVSObj, gFSObj;
	GLuint gSPObj;
	GLuint gVAObj, gVBObj;
	GLuint gMVPMatrixUniform;
	GLuint colorUniform;
	
	int elbow, shoulder;
	
	MySphere *sphere;
	GLfloat *sphereVertex;
	int pointCnt;
	
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
	gVSObj = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *VSSrcCode =
		"#version 410 core\n" \
		"in vec3 vPosition;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec3 out_color;"\
		"void main(void) {" \
			"gl_Position = u_mvpMatrix * vec4(vPosition, 1.0);" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);
	[self shaderErrorCheck : gVSObj option : "COMPILE"];
	fprintf(gfp_log, "Vertex Shader Compiled successfully...\n");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *FSSrcCode =
		"#version 410 core\n" \
		"uniform vec4 u_color;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = u_color;" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);
	[self shaderErrorCheck : gFSObj option : "COMPILE"];
	fprintf(gfp_log, "Fragment Shader Compiled successfully...\n");

	// Shader program
	gSPObj = glCreateProgram();
	glAttachShader(gSPObj, gVSObj);
	glAttachShader(gSPObj, gFSObj);
	glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
	glLinkProgram(gSPObj);
	[self shaderErrorCheck : gSPObj option : "LINK"];
	fprintf(gfp_log, "Shader Program Compiled successfully...\n");

	// get uniform location(s)
	gMVPMatrixUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");
	colorUniform = glGetUniformLocation(gSPObj, "u_color");

	// other variable initialization
	sphere = [[MySphere alloc] init];
	pointCnt = [sphere GenSphere : 0.5f stack : 30 slices : 30];
	sphereVertex = (GLfloat *)[sphere getSphereVertices];
	
	glGenVertexArrays(1, &gVAObj);
	glBindVertexArray(gVAObj);
		glGenBuffers(1, &gVBObj);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj);
		glBufferData(GL_ARRAY_BUFFER, pointCnt * 3 * sizeof(GLfloat), sphereVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Depth related OpenGL code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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
	mat4 MVMatrix, MVPMatrix;
	mat4 translationMatrix;
	
	// Code
	[[self openGLContext] makeCurrentContext];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// OpenGL Code starts here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();

	MVMatrix *= translate(0.0f, 0.0f, -15.0f);

	glUseProgram(gSPObj);
	PushMatrix4x4(MVMatrix);
		MVMatrix *= rotate((GLfloat)shoulder, 0.0f, 0.0f, 1.0f);
		MVMatrix *= translate(1.0f, 0.0f, 0.0f);
		PushMatrix4x4(MVMatrix);
			MVMatrix *= scale(2.0f, 0.65f, 1.0f);
			MVPMatrix = gPerspProjMatrix * MVMatrix;
			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
			glUniform4f(colorUniform, 0.5f, 0.35f, 0.05f, 1.0f);
			glBindVertexArray(gVAObj);
				glDrawArrays(GL_TRIANGLES, 0, pointCnt);
			glBindVertexArray(0);
		MVMatrix = PopMatrix4x4();
		MVMatrix *= translate(1.0f, 0.0f, 0.0f);
		MVMatrix *= rotate((GLfloat)elbow, 0.0f, 0.0f, 1.0f);
		MVMatrix *= translate(1.0f, 0.0f, 0.0f);
		PushMatrix4x4(MVMatrix);
			MVMatrix *= scale(2.0f, 0.5f, 1.0f);
			MVPMatrix = gPerspProjMatrix * MVMatrix;
			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
			glUniform4f(colorUniform, 0.5f, 0.35f, 0.05f, 1.0f);
			glBindVertexArray(gVAObj);
				glDrawArrays(GL_TRIANGLES, 0, pointCnt);
			glBindVertexArray(0);
		MVMatrix = PopMatrix4x4();
	MVMatrix = PopMatrix4x4();
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
		case 'e' :
			elbow += 6;
			if(elbow > 180)
				elbow = 180;
			break;
		case 'E' :
			elbow -= 6;
			if(elbow < 0)
				elbow = 0;
			break;
		case 's' :
			shoulder += 3;
			break;
		case 'S':
			shoulder -= 3;
			break;
		default :
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
	
	// Unlink shader program (if not unlinked earlier)
	glUseProgram(0);
	
	// Free Sphere
	[sphere deleteSphere];
	
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
	if(gVSObj != 0) {
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gFSObj != 0) {
		glDeleteShader(gFSObj);
		gFSObj = 0;
	}
	if(gSPObj != 0) {
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
