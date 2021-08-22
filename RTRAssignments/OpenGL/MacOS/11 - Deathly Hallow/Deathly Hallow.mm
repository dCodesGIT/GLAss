//
//  Deathly Hallow.mm
//  
//  Created by Darshan Vikam on 02/08/21.
//

// Importing required headers
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <OpenGL/gl3.h>			// gl.h - for legacy OpenGL (FFP)
#import "../Include/vmath.h"
#import <math.h>

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
	[window setTitle : @"Deathly Hallow"];
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
	
	GLuint gVSObj;		// Vertex Shader Object
	GLuint gFSObj;		// Fragment Shader Object
	GLuint gSPObj;		// Shader Program Object
	GLuint gVAObj_DeathlyHallow[3];		// Vertex Array Object
	GLuint gVBObj_CloakOfInvisibility[2];	// Vertex Buffer Object
	GLuint gVBObj_ResurrectionStone[2];	// Vertex Buffer Object
	GLuint gVBObj_ElderWand[2];		// Vertex Buffer Object
	GLuint gMVPMatrixUniform;	// Model View Projection Matrix Uniform
	
	GLfloat trans;
	GLfloat angle;
	
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
		"in vec3 vColor;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec3 out_color;"\
		"void main(void) {" \
			"gl_Position = u_mvpMatrix * vPosition;" \
			"out_color = vColor;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);				// Compile Shader
	[self shaderErrorCheck : gVSObj option : "COMPILE"];
	fprintf(gfp_log, "Vertex Shader Compiled successfully...\n");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 410 core\n" \
		"in vec3 out_color;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = vec4(out_color, 1.0);" \
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
	glBindAttribLocation(gSPObj, DV_ATTRIB_COL, "vColor");
	glLinkProgram(gSPObj);
	[self shaderErrorCheck : gSPObj option : "LINK"];
	fprintf(gfp_log, "Shader Program Compiled successfully...\n");

	// get uniform location(s)
	gMVPMatrixUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");

	// other global variable initialization
	const GLfloat radian = 22.0f / (7.0f * 180.0f);
	const GLfloat TriangleSide = 1.5f;
	const GLfloat Triangle_radius = (2.0f / sqrt(3.0f)) * TriangleSide;
	const GLfloat inCircle_radius = TriangleSide / sqrt(3.0f);

	const GLfloat CloakOfInvisibilityVertex[] = {
		(GLfloat)(Triangle_radius * cos(90.0f * radian)), (GLfloat)(Triangle_radius * sin(90.0f * radian)), 0.0f,	// Apex
		(GLfloat)(Triangle_radius * cos(210.0f * radian)), (GLfloat)(Triangle_radius * sin(210.0f * radian)), 0.0f,	// left bottom
		(GLfloat)(Triangle_radius * cos(330.0f * radian)), (GLfloat)(Triangle_radius * sin(330.0f * radian)), 0.0f	// right bottom
	};
	const GLfloat CloakOfInvisibilityColor[] = {
		1.0f, 1.0f, 1.0f,	// Apex
		1.0f, 1.0f, 1.0f,	// left bottom
		1.0f, 1.0f, 1.0f	// Right bottom
	};

	GLfloat ResurrectionStoneVertex[3600*2];
	for(int i = 0; i < 3600*2; i += 2) {
		ResurrectionStoneVertex[i] = (GLfloat)(inCircle_radius * cos(i * radian / 20.0f));
		ResurrectionStoneVertex[i+1] = (GLfloat)(inCircle_radius * sin(i * radian / 20.0f));
	}
	GLfloat ResurrectionStoneColor[3600*3];
	for(int i = 0; i < 3600*3; i++) {
		ResurrectionStoneColor[i] = 1.0f;
	}

	const GLfloat ElderWandVertex[] = {
		0.0f, (GLfloat)(inCircle_radius * 2), 0.0f,
		0.0f, (GLfloat)(-inCircle_radius), 0.0f
	};
	const GLfloat ElderWandColor[] = {
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	
	glGenVertexArrays(3, gVAObj_DeathlyHallow);
	glBindVertexArray(gVAObj_DeathlyHallow[0]);
		glGenBuffers(2, gVBObj_CloakOfInvisibility);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_CloakOfInvisibility[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CloakOfInvisibilityVertex), CloakOfInvisibilityVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_CloakOfInvisibility[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CloakOfInvisibilityColor), CloakOfInvisibilityColor, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_DeathlyHallow[1]);
		glGenBuffers(2, gVBObj_ResurrectionStone);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_ResurrectionStone[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ResurrectionStoneVertex), ResurrectionStoneVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_ResurrectionStone[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ResurrectionStoneColor), ResurrectionStoneColor, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_DeathlyHallow[2]);
		glGenBuffers(2, gVBObj_ElderWand);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_ElderWand[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ElderWandVertex), ElderWandVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_ElderWand[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ElderWandColor), ElderWandColor, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	trans = 5.0f;
	angle = 720.0f;
	
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
	mat4 rotationMatrix;
	
	// Code
	[[self openGLContext] makeCurrentContext];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// OpenGL Code starts here
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	// Initializing
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting Values
	translationMatrix = translate(-trans, -trans, -5.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 1.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	// Setting uniform
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj_DeathlyHallow[0]);
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	glBindVertexArray(0);
	
	// Initializing
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting Values
	translationMatrix = translate(trans, -trans, -5.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 1.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	// Setting uniform
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj_DeathlyHallow[1]);
		glDrawArrays(GL_POINTS, 0, 3600);
	glBindVertexArray(0);
	
	// Initializing
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting Values
	translationMatrix = translate(0.0f, trans, -5.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 1.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	// Setting uniform
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj_DeathlyHallow[2]);
		glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);
	
	if(angle > 0.0f)
		angle -= 1.0f;
	else
		angle = 0.0f;
	if(trans > 0.0f)
		trans -= 0.01f;
	else
		trans = 0.0f;
	
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
	if(gVAObj_DeathlyHallow) {
		glDeleteVertexArrays(3, gVAObj_DeathlyHallow);
		gVAObj_DeathlyHallow[0] = 0;
		gVAObj_DeathlyHallow[1] = 0;
		gVAObj_DeathlyHallow[2] = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_CloakOfInvisibility) {
		glDeleteBuffers(2, gVBObj_CloakOfInvisibility);
		gVBObj_CloakOfInvisibility[0] = 0;
		gVBObj_CloakOfInvisibility[1] = 0;
	}
	if(gVBObj_ResurrectionStone) {
		glDeleteBuffers(2, gVBObj_ResurrectionStone);
		gVBObj_ResurrectionStone[0] = 0;
		gVBObj_ResurrectionStone[1] = 0;
	}
	if(gVBObj_ElderWand) {
		glDeleteBuffers(2, gVBObj_ElderWand);
		gVBObj_ElderWand[0] = 0;
		gVBObj_ElderWand[1] = 0;
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
