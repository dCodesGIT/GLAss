//
//  Shapes on graph paper.mm
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
	[window setTitle : @"Shapes on graph paper"];
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
	GLuint gVAObj_gp;	// Vertex Array Object - Graph paper
	GLuint gVBObj_gp[2];	// Vertex Buffer Object - Graph paper
	GLuint gVAObj_shapes[3];	// Vertex Array Object - Shapes
	GLuint gVBObj_triangle[2];	// Vertex Buffer Object
	GLuint gVBObj_rectangle[2];	// Vertex Buffer Object
	GLuint gVBObj_circle[2];	// Vertex Buffer Object
	GLuint gMVPMatrixUniform;	// Model View Projection Matrix Uniform
	
	GLfloat triangleSide;
	GLfloat rectangleWidth, rectangleHeight;
	GLfloat circleRadius;
	bool gbAnimationEnabled;
	char shapeSelected;
	
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

	// Variable initialization - graph paper
	GLfloat GraphPaperVertices[164][2];
	GLfloat GraphPaperColor[164][3];
	GLfloat vert = -2.0f;
	for(int i = 0; i < 164 && vert <= 2.1f; i++, vert += 0.1f) {
		// Left coordinate of horizontal line
		GraphPaperVertices[i][0] = -2.0f;
		GraphPaperVertices[i][1] = vert;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
		// Right coordinate of horizontal line
		i++;
		GraphPaperVertices[i][0] = 2.0f;
		GraphPaperVertices[i][1] = vert;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
		// Bottom coordinate of Vertical line
		i++;
		GraphPaperVertices[i][0] = vert;
		GraphPaperVertices[i][1] = -2.0f;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
		// Top coordinate of Vertical line
		i++;
		GraphPaperVertices[i][0] = vert;
		GraphPaperVertices[i][1] = 2.0f;
			GraphPaperColor[i][0] = 0.0f;
			GraphPaperColor[i][1] = 0.0f;
			GraphPaperColor[i][2] = 1.0f;
	}
	// Red color of X Axis - left point
	GraphPaperColor[164/2][0] = 1.0f;
	GraphPaperColor[164/2][1] = 0.0f;
	GraphPaperColor[164/2][2] = 0.0f;
	// Red color of X Axis - right point
	GraphPaperColor[(164/2)+1][0] = 1.0f;
	GraphPaperColor[(164/2)+1][1] = 0.0f;
	GraphPaperColor[(164/2)+1][2] = 0.0f;
	// Green color of Y Axis - Bottom point
	GraphPaperColor[(164/2)-1][0] = 0.0f;
	GraphPaperColor[(164/2)-1][1] = 1.0f;
	GraphPaperColor[(164/2)-1][2] = 0.0f;
	// Green color of Y Axis - Top point
	GraphPaperColor[(164/2)-2][0] = 0.0f;
	GraphPaperColor[(164/2)-2][1] = 1.0f;
	GraphPaperColor[(164/2)-2][2] = 0.0f;
	
	// For graph paper
	glGenVertexArrays(1, &gVAObj_gp);
	glBindVertexArray(gVAObj_gp);
		glGenBuffers(2, gVBObj_gp);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_gp[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GraphPaperVertices), GraphPaperVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_gp[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GraphPaperColor), GraphPaperColor, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// For Shapes
	glGenVertexArrays(3, gVAObj_shapes);	// Generate 3 vertex arrays to draw triangle, rectangle and circle
	glBindVertexArray(gVAObj_shapes[0]);	// For triangle
		glGenBuffers(2, gVBObj_triangle);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[0]);
		glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[1]);
		glBufferData(GL_ARRAY_BUFFER, 3 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
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
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
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
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	triangleSide = 0.5f;
	rectangleWidth = 0.7f;
	rectangleHeight = 0.5f;
	circleRadius = 0.5f;
	
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
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	translationMatrix = translate(0.0f, 0.0f, -5.5f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj_gp);
	for(int i = 0; i < 164; i += 2)
		glDrawArrays(GL_LINES, i, 2);
	glBindVertexArray(0);
	
	if(shapeSelected == 't') {		// Triangle
		const GLfloat triangleVertex[] = {
			0.0f, triangleSide, 0.0f,		// Apex
			-triangleSide, -triangleSide, 0.0f,	// left bottom
			triangleSide, -triangleSide, 0.0f	// right bottom
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
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_LINE_LOOP, 0, 3);
		glBindVertexArray(0);
	}
	else if(shapeSelected == 'r') {		// rectangle
		const GLfloat rectangleVertices[] = {
			-rectangleWidth,  rectangleHeight, 0.0f,	// Top left
			-rectangleWidth, -rectangleHeight, 0.0f,	// Bottom left
			 rectangleWidth, -rectangleHeight, 0.0f,	// Bottom right
			 rectangleWidth,  rectangleHeight, 0.0f		// Top right
		};
		const GLfloat rectangleColor[] = {
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f
		};
		glBindVertexArray(gVAObj_shapes[1]);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColor), rectangleColor, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		glBindVertexArray(0);
	}
	else if(shapeSelected == 'c') {		// Circle
		GLfloat circleVertices[3600*2];
		GLfloat circleColor[3600*3];
		for(int i = 0; i < 3600*2; i += 2) {
			circleVertices[i] = circleRadius * cos(((GLfloat)i/20.0f)*(M_PI/180.0f));	// X = r * cos(theta)
			circleVertices[i+1] = circleRadius * sin(((GLfloat)i/20.0f)*(M_PI/180.0f));	// Y = r * sin(theta)
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
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_POINTS, 0, 3600);
		glBindVertexArray(0);
	}

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
		case 't' :
		case 'T' :
			shapeSelected = 't';
			break;
		case 'r' :
		case 'R' :
			shapeSelected = 'r';
			break;
		case 'c' :
		case 'C' :
			shapeSelected = 'c';
			break;
		case NSUpArrowFunctionKey :			// Up Arrow key
			if(shapeSelected == 't' && triangleSide < 1.7f)
				triangleSide += 0.1f;
			else if(shapeSelected == 'r' && rectangleHeight < 1.7f)
				rectangleHeight += 0.1f;
			else if(shapeSelected == 'c' && circleRadius < 1.7f)
				circleRadius += 0.1f;
			break;
		case NSDownArrowFunctionKey :			// Down Arrow key
			if(shapeSelected == 't' && triangleSide > 0.2f)
				triangleSide -= 0.1f;
			else if(shapeSelected == 'r' && rectangleHeight > 0.2f)
				rectangleHeight -= 0.1f;
			else if(shapeSelected == 'c' && circleRadius > 0.2f)
				circleRadius -= 0.1f;
			break;
		case NSRightArrowFunctionKey :			// Right Arrow key
			if(shapeSelected == 't' && triangleSide < 1.7f)
				triangleSide += 0.1f;
			else if(shapeSelected == 'r' && rectangleWidth < 1.7f)
				rectangleWidth += 0.1f;
			else if(shapeSelected == 'c' && circleRadius < 1.7f)
				circleRadius += 0.1f;
			break;
		case NSLeftArrowFunctionKey :			// Left Arrow key
			if(shapeSelected == 't' && triangleSide > 0.2f)
				triangleSide -= 0.1f;
			else if(shapeSelected == 'r' && rectangleWidth > 0.2f)
				rectangleWidth -= 0.1f;
			else if(shapeSelected == 'c' && circleRadius > 0.2f)
				circleRadius -= 0.1f;
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
	if(gVAObj_gp) {
		glDeleteVertexArrays(1, &gVAObj_gp);
		gVAObj_gp = 0;
	}
	if(gVAObj_shapes) {
		glDeleteVertexArrays(3, gVAObj_shapes);
		gVAObj_shapes[0] = 0;
		gVAObj_shapes[1] = 0;
		gVAObj_shapes[2] = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_gp) {
		glDeleteBuffers(2, gVBObj_gp);
		gVBObj_gp[0] = 0;
		gVBObj_gp[1] = 0;
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
