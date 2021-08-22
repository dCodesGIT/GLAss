//
//  Cube using interleaved arary.mm
//  
//  Created by Darshan Vikam on 06/08/21.
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
	[window setTitle : @"Cube with light"];
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
	GLuint gVAObj;		// Vertex Array Object
	GLuint gVBObj_cube;		// Vertex Buffer Object

	GLuint gMMatrixUniform, gVMatrixUniform, gPMatrixUniform;	// Model, View, Projection Matrix Uniform
	GLuint gLAmbUniform, gLDiffUniform, gLSpecUniform, gLPosUniform;
	GLuint gKAmbUniform, gKDiffUniform, gKSpecUniform, gKShineUniform;
	GLuint gInterleavedEnabledUniform;
	
	GLuint gTextureSamplerUniform;
	GLuint marble_texture;
	
	bool InterleavedEnabled;
	bool animationEnabled;
	
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
		"#version 410 core \n" \
		"in vec4 vPosition;" \
		"in vec3 vColor, vNormal;" \
		"in vec2 vTexCoord;" \
		"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
		"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" \
		"uniform vec4 u_LPos;" \
		"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
		"uniform float u_KShine;" \
		"uniform bool u_InterleavedEnabled;" \
		"out vec3 out_light;" \
		"out vec2 out_texCoord;" \
		"void main(void) {" \
			"out_light = vec3(0.0);" \
			"out_texCoord = vec2(0.0);" \
			"if(u_InterleavedEnabled) {" \
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
	glCompileShader(gVSObj);				// Compile Shader
	[self shaderErrorCheck : gVSObj option : "COMPILE"];
	fprintf(gfp_log, "Vertex Shader Compiled successfully...\n");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 410 core\n" \
		"in vec3 out_light;" \
		"in vec2 out_texCoord;" \
		"uniform sampler2D u_textureSampler;"
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 out_texture = texture(u_textureSampler, out_texCoord).rgb;" \
			"FragColor = vec4(out_light * out_texture, 1.0);" \
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
	glBindAttribLocation(gSPObj, DV_ATTRIB_NORM, "vNormal");
	glBindAttribLocation(gSPObj, DV_ATTRIB_TEX, "vTexCoord");
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
	gTextureSamplerUniform = glGetUniformLocation(gSPObj, "u_textureSampler");
	gInterleavedEnabledUniform = glGetUniformLocation(gSPObj, "u_InterleavedEnabled");
	
	// other global variable initialization
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
		-1.0f, 1.0, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f
	};

	glGenVertexArrays(1, &gVAObj);
	glBindVertexArray(gVAObj);
		glGenBuffers(1, &gVBObj_cube);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_cube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Cube), Cube, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)0);
		glEnableVertexAttribArray(DV_ATTRIB_POS);
		glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_COL);
		glVertexAttribPointer(DV_ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_NORM);
		glVertexAttribPointer(DV_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, (3+3+3+2)*sizeof(float), (void *)(9 * sizeof(float)));
		glEnableVertexAttribArray(DV_ATTRIB_TEX);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Depth related OpenGL code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	InterleavedEnabled = false;
	
	marble_texture = [self loadGLTexture : "marble.bmp"];
	if(marble_texture == 0) {
		[self release];
		[NSApp terminate : self];
	}
	
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
	mat4 MMatrix, VMatrix, PMatrix;
	mat4 translationMatrix, rotationMatrix;
	
	// Code
	[[self openGLContext] makeCurrentContext];
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
	
	// OpenGL Code starts here
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	if(InterleavedEnabled) {
		glUniform1i(gInterleavedEnabledUniform, true);
		glUniform3f(gLAmbUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(gLDiffUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(gLSpecUniform, 0.0f, 0.0f, 0.0f);
		glUniform4f(gLPosUniform, 0.0f, 0.0f, 2.0f, 1.0f);
		glUniform3f(gKAmbUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(gKDiffUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(gKSpecUniform, 1.0f, 1.0f, 1.0f);
		glUniform1i(gKShineUniform, 50.0f);
	}
	else
		glUniform1i(gInterleavedEnabledUniform, false);

	// Initialization
	MMatrix = mat4::identity();
	VMatrix = mat4::identity();
	PMatrix = gPerspProjMatrix;
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting values and transformation
	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	VMatrix *= translationMatrix;
	rotationMatrix = rotate(angle, 1.0f, 0.0f, 0.0f);
	VMatrix *= rotationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 1.0f, 0.0f);
	VMatrix *= rotationMatrix;
	rotationMatrix = rotate(angle, 0.0f, 0.0f, 1.0f);
	VMatrix *= rotationMatrix;
	glUniformMatrix4fv(gMMatrixUniform, 1, GL_FALSE, MMatrix);
	glUniformMatrix4fv(gVMatrixUniform, 1, GL_FALSE, VMatrix);
	glUniformMatrix4fv(gPMatrixUniform, 1, GL_FALSE, PMatrix);
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
	
	if(animationEnabled)
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
		case 'i' :
		case 'I' :
			InterleavedEnabled = ! InterleavedEnabled;
			break;
		case 'a' :
		case 'A' :
			animationEnabled = ! animationEnabled;
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
	if(gVBObj_cube) {
		glDeleteBuffers(1, &gVBObj_cube);
		gVBObj_cube = 0;
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
