//
//  3D Shapes with textures.mm
//  
//  Created by Darshan Vikam on 03/08/21.
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
	[window setTitle : @"3D Shapes with textures"];
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
	GLuint gVAObj[2];		// Vertex Array Object
	GLuint gVBObj_pyramid[2];	// Vertex Buffer Object
	GLuint gVBObj_cube[2];		// Vertex Buffer Object
	GLuint gMVPMatrixUniform;	// Model View Projection Matrix Uniform
	GLuint gTextureSamplerUniform;	// TextureSampler Uniform
	
	GLfloat angle;
	GLuint stone_texture, kundali_texture;
	
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
		"in vec2 vTexCoord;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec3 out_color;"\
		"out vec2 out_texcoord;" \
		"void main(void) {" \
			"gl_Position = u_mvpMatrix * vPosition;" \
			"out_color = vColor;" \
			"out_texcoord = vTexCoord;" \
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
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_texture_sampler;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
/*			"FragColor = vec4(out_color, 1.0);" \
*/			"FragColor = texture(u_texture_sampler, out_texcoord);" \
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
	glBindAttribLocation(gSPObj, DV_ATTRIB_TEX, "vTexCoord");
	glLinkProgram(gSPObj);
	[self shaderErrorCheck : gSPObj option : "LINK"];
	fprintf(gfp_log, "Shader Program Compiled successfully...\n");

	// get uniform location(s)
	gMVPMatrixUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");
	gTextureSamplerUniform = glGetUniformLocation(gSPObj, "u_texture_sampler");

	// other global variable initialization
	const GLfloat PyramidVertex[] = {
		// Front face - Apex, left bottom, right bottom
		0.0f, 2.0f, 0.0f,	-1.0f, -1.0f, 1.0f,	1.0f, -1.0f, 1.0f,
		// Right face - Apex, left bottom, right bottom
		0.0f, 2.0f, 0.0f,	1.0f, -1.0f, 1.0f,	1.0f, -1.0f, -1.0f,
		// Back face - Apex, left bottom, right bottom
		0.0f, 2.0f, 0.0f,	1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,
		// Left face - Apex, left bottom, right bottom
		0.0f, 2.0f, 0.0f,	-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, 1.0f
	};
	const GLfloat PyramidColors[] = {
		// Front face - red, green, blue
		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		// Right face - red, green, blue
		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,
		// Back face - red, green, blue
		1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,
		// Left face - red, green, blue
		1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f
	};
	const GLfloat PyramidTexture[] = {
		// Front face
		0.5f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,
		// Right face
		0.5f, 1.0f,	1.0f, 0.0f,	0.0f, 0.0f,
		// Back face
		0.5f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,
		// Left face
		0.5f, 1.0f,	1.0f, 0.0f,	0.0f, 0.0f
	};
	const GLfloat CubeVertex[] = {
		// Front face - top left, bottom left, bottom right, top right
		-1.0f, 1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	1.0f, -1.0, 1.0f,	1.0f, 1.0, 1.0f,
		// Right face - top left, bottom left, bottom right, top right
		1.0f, 1.0f, 1.0f,	1.0f, -1.0f, 1.0f,	1.0f, -1.0, -1.0f,	1.0f, 1.0, -1.0f,
		// Bottom face - top left, bottom left, bottom right, top right
		1.0f, -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	-1.0f, -1.0, -1.0f,	1.0f, -1.0, -1.0f,
		// Left face - top left, bottom left, bottom right, top right
		-1.0f, 1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,	-1.0f, -1.0, 1.0f,	-1.0f, 1.0, 1.0f,
		// Back face - top left, bottom left, bottom right, top right
		1.0f, 1.0f, -1.0f,	1.0f, -1.0f, -1.0f,	-1.0f, -1.0, -1.0f,	-1.0f, 1.0, -1.0f,
		// Top face - top left, bottom left, bottom right, top right
		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, -1.0f,	-1.0f, 1.0, -1.0f,	-1.0f, 1.0, 1.0f
	};
	const GLfloat CubeColors[] = {
		// Front face - Red
		1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		// Right face - Green
		0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		// Bottom face - Blue
		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,
		// Left face - Green
		0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,
		// Back face - Red
		1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
		// Top face - Blue
		0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f, 1.0f
	};
	const GLfloat CubeTexture[] = {
		// Front face
		0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	1.0f, 1.0f,
		// Right face
		0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	1.0f, 1.0f,
		// Bottom face
		0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	1.0f, 1.0f,
		// Left face
		0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	1.0f, 1.0f,
		// Back face
		0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	1.0f, 1.0f,
		// Top face
		0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	1.0f, 1.0f
	};


	glGenVertexArrays(2, gVAObj);
	glBindVertexArray(gVAObj[0]);
		glGenBuffers(2, gVBObj_pyramid);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pyramid[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidVertex), PyramidVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pyramid[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidColors), PyramidColors, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pyramid[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidTexture), PyramidTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_TEX);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj[1]);
		glGenBuffers(2, gVBObj_cube);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_cube[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertex), CubeVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_cube[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeColors), CubeColors, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_cube[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeTexture), CubeTexture, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIB_TEX);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	stone_texture = [self loadGLTexture : "Stone.png"];
	if(stone_texture == 0) {
		[self release];
		[NSApp terminate : self];
	}
	kundali_texture = [self loadGLTexture : "Kundali.png"];
	if(kundali_texture == 0) {
		[self release];
		[NSApp terminate : self];
	}
	
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
-(GLuint)loadGLTexture: (const char *)image {
	// Code
	NSBundle *appBundle = [NSBundle mainBundle];
	NSString *appLocn = [appBundle bundlePath];
	NSString *absPathOfImage = [NSString stringWithFormat : @"%@/%s", [appLocn stringByDeletingLastPathComponent], image];

	NSImage *bmpImage = [[NSImage alloc] initWithContentsOfFile : absPathOfImage];	// Get NSImage representation of our image
	if(!bmpImage) {
		fprintf(gfp_log, "NSImage convertion of image file failed !!!\n");
		return 0;
	}
	CGImageRef cgImage = [bmpImage CGImageForProposedRect : nil context : nil hints : nil];	// Get CGImage representation of NSImage
	int imgWidth = (int)CGImageGetWidth(cgImage);		// Get width and height of image
	int imgHeight = (int)CGImageGetHeight(cgImage);
	CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));	// Get Core Foundation Data(CFData) representation of our image
	void *pixels = (void *)CFDataGetBytePtr(imageData);	// Convert CFDataRef data into generic foramt(void *)
	
	GLuint texture;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	CFRelease(imageData);
	
	return texture;
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
	mat4 translationMatrix, rotationMatrix;
	
	// Code
	[[self openGLContext] makeCurrentContext];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// OpenGL Code starts here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	// Initialization
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting values
	translationMatrix = translate(-2.0f, 0.0f, -7.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 1.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	// Activating texture and sending it to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, stone_texture);
	glUniform1i(gTextureSamplerUniform, 0);
	// OpenGL Drawing
	glBindVertexArray(gVAObj[0]);
		glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);
	
	// Initialization
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting values
	translationMatrix = translate(2.0f, 0.0f, -7.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(angle, 1.0f, 0.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 1.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 0.0f, 1.0f);
	MVMatrix *= rotationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	// Activating texture and sending it to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, kundali_texture);
	glUniform1i(gTextureSamplerUniform, 0);
	// OpenGL Drawing
	glBindVertexArray(gVAObj[1]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);
	
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
		glDeleteVertexArrays(2, gVAObj);
		gVAObj[0] = 0;
		gVAObj[1] = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_pyramid) {
		glDeleteBuffers(2, gVBObj_pyramid);
		gVBObj_pyramid[0] = 0;
		gVBObj_pyramid[1] = 0;
	}
	if(gVBObj_cube) {
		glDeleteBuffers(2, gVBObj_cube);
		gVBObj_cube[0] = 0;
		gVBObj_cube[1] = 0;
	}

	// Delete Textures
	glDeleteTextures(1, &stone_texture);
	glDeleteTextures(1, &kundali_texture);

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
