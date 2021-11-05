//
//  Tessellation.m
//  
//  Created by Darshan Vikam on 01/08/21.
//

// Importing required headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>			// gl.h - for legacy OpenGL (FFP)
#import "../Include/vmath.h"

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
	[window setTitle : @"Tessellation"];
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
	GLuint gTCSObj, gTESObj;
	GLuint gSPObj;
	GLuint gVAObj, gVBObj;
	GLuint gMVPMatrixUniform;
	GLuint SegmentCountUniform;
	GLuint StripCountUniform;
	GLuint colorUniform;
	
	int segmentCnt;
	
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
		"in vec2 vPosition;" \
		"void main(void) {" \
			"gl_Position = vec4(vPosition, 0.0, 1.0);" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);
	[self shaderErrorCheck : gVSObj option : "COMPILE"];
	fprintf(gfp_log, "Vertex Shader Compiled successfully...\n");

	// Tessellation Control Shader
	gTCSObj = glCreateShader(GL_TESS_CONTROL_SHADER);
	const GLchar *TCSSrcCode =
		"#version 410 core\n" \
		"layout(vertices = 4)out;" \
		"uniform int numberOfSegments;" \
		"uniform int numberOfStrips;" \
		"void main(void) {" \
			"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
			"gl_TessLevelOuter[0] = float(numberOfStrips);" \
			"gl_TessLevelOuter[1] = float(numberOfSegments);" \
		"}";
	glShaderSource(gTCSObj, 1, (const GLchar **)&TCSSrcCode, NULL);
	glCompileShader(gTCSObj);
	[self shaderErrorCheck : gTCSObj option : "COMPILE"];
	fprintf(gfp_log, "Tessellation Control shader compiled successfully...\n");

	// Tessellation Evaluation Shader
	gTESObj = glCreateShader(GL_TESS_EVALUATION_SHADER);
	const GLchar *TESSrcCode =
		"#version 410 core\n" \
		"layout(isolines)in;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void) {" \
			"float tessCoords = gl_TessCoord.x;" \
			"vec3 p0 = gl_in[0].gl_Position.xyz;" \
			"vec3 p1 = gl_in[1].gl_Position.xyz;" \
			"vec3 p2 = gl_in[2].gl_Position.xyz;" \
			"vec3 p3 = gl_in[3].gl_Position.xyz;" \
			"vec3 p = (p0 * (1.0-tessCoords) * (1.0-tessCoords) * (1.0-tessCoords)) + (3.0 * p1 * (1.0-tessCoords) * (1.0-tessCoords) * tessCoords) + (3.0 * p2 * (1.0-tessCoords) * tessCoords * tessCoords) + (p3 * tessCoords * tessCoords * tessCoords);" \
			"gl_Position = u_mvpMatrix * vec4(p, 1.0);" \
		"}";
	glShaderSource(gTESObj, 1, (const GLchar**)&TESSrcCode, NULL);
	glCompileShader(gTESObj);
	[self shaderErrorCheck : gTESObj option : "COMPILE"];
	fprintf(gfp_log, "Tessellation Evaluation Shader compiled successfully...\n");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar *FSSrcCode =
		"#version 410 core\n" \
		"uniform vec4 color;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = color;" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);
	[self shaderErrorCheck : gFSObj option : "COMPILE"];
	fprintf(gfp_log, "Fragment Shader Compiled successfully...\n");

	// Shader program
	gSPObj = glCreateProgram();
	glAttachShader(gSPObj, gVSObj);
	glAttachShader(gSPObj, gTCSObj);
	glAttachShader(gSPObj, gTESObj);
	glAttachShader(gSPObj, gFSObj);
	glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
	glLinkProgram(gSPObj);
	[self shaderErrorCheck : gSPObj option : "LINK"];
	fprintf(gfp_log, "Shader Program Compiled successfully...\n");

	// get uniform location(s)
	gMVPMatrixUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");
	SegmentCountUniform = glGetUniformLocation(gSPObj, "numberOfSegments");
	StripCountUniform = glGetUniformLocation(gSPObj, "numberOfStrips");
	colorUniform = glGetUniformLocation(gSPObj, "color");

	// other global variable initialization
	const GLfloat bezierControlPoints[] = {
		-1.0f, -1.0f,
		-0.5f, 1.0f,
		0.5f, -1.0f,
		1.0f, 1.0f
	};

	glGenVertexArrays(1, &gVAObj);
	glBindVertexArray(gVAObj);
		glGenBuffers(1, &gVBObj);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bezierControlPoints), bezierControlPoints, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	gPerspProjMatrix = mat4::identity();
	segmentCnt = 1;

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
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	
	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	glUniform1i(SegmentCountUniform, segmentCnt);
	glUniform1i(StripCountUniform, 1);
	glUniform4f(colorUniform, 1.0f, 1.0f, 0.0f, 0.0f);
	
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	// OpenGL Drawing
	glBindVertexArray(gVAObj);
		glDrawArrays(GL_PATCHES, 0, 4);
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
		case 126 :		// Up Arrow
		case 'w':
		case 'W':
			if(segmentCnt < 30)
				segmentCnt++;
			break;
		case 125 :		// Down Arrow
		case 's':
		case 'S':
			if(segmentCnt > 1)
				segmentCnt--;
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
	glDetachShader(gSPObj, gTCSObj);
	glDetachShader(gSPObj, gTESObj);
	glDetachShader(gSPObj, gFSObj);
	
	// Delete shaders
	if(gVSObj != 0) {
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gTCSObj != 0) {
		glDeleteShader(gTCSObj);
		gTCSObj = 0;
	}
	if(gTESObj != 0) {
		glDeleteShader(gTESObj);
		gTESObj = 0;
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
