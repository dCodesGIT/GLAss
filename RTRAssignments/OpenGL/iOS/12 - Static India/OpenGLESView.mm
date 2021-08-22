//
//  OpenGLESView.m
//  Static India
//
//  Created by Darshan Vikam on 13/08/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "OpenGLESView.h"
#import "vmath.h"

using namespace vmath;

@implementation OpenGLESView {
	@private
	EAGLContext *eaglContext;

	GLuint defaultFrameBuffer;
	GLuint colorRenderBuffer;
	GLuint depthRenderBuffer;
	
	id displayLink;
	
	NSInteger animationFrameInterval;
	bool animationEnabled;
	
	enum {
		DV_ATTRIB_POS = 0,
		DV_ATTRIB_COL,
		DV_ATTRIB_NORM,
		DV_ATTRIB_TEX,
	};
	
	GLuint gVSObj, gFSObj, gSPObj;
	GLuint gVAObj_INDIA[4];
	GLuint gVBObj_I[2];
	GLuint gVBObj_N[2];
	GLuint gVBObj_D[2];
	GLuint gVBObj_A[2];
	GLuint gMVPMatrixUniform;
	
	mat4 gPerspProjMatrix;
}
-(id)initWithFrame: (CGRect)frame {
	// Code
	self = [super initWithFrame : frame];
	
	if(self) {
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)[super layer];
		[eaglLayer setOpaque: YES];
		[eaglLayer setDrawableProperties: [NSDictionary dictionaryWithObjectsAndKeys:
						[NSNumber numberWithBool: NO], kEAGLDrawablePropertyRetainedBacking,
						kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
						nil]];
		
		eaglContext = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES3];
		if(eaglContext == nil) {
			printf("OpenGL ES context creation failed !!!");
			return nil;
		}
		[EAGLContext setCurrentContext: eaglContext];
		
		// Creating Frame buffer and Color buffer
		glGenFramebuffers(1, &defaultFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
		glGenRenderbuffers(1, &colorRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
		[eaglContext renderbufferStorage: GL_RENDERBUFFER fromDrawable: eaglLayer];
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);
		
		GLint backingWidth, backingHeight;
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
		
		// Creating depth buffer
		glGenRenderbuffers(1, &depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
		
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Frame buffer is not complete !!!\n");
			[self Uninitialize];
			return nil;
		}
		
//		printf("%s\n", glGetString(GL_RENDERER));
		printf("%s\n", glGetString(GL_VERSION));
		printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		
		animationFrameInterval = 60;
		animationEnabled = false;
		
		// Vertex Shader
		gVSObj = glCreateShader(GL_VERTEX_SHADER);
		const GLchar *VSSrcCode =
			"#version 300 es\n" \
			"precision highp float;" \
			"in vec4 vPosition;" \
			"in vec3 vColor;" \
			"uniform mat4 u_mvpMatrix;" \
			"out vec3 out_color;" \
			"void main(void) {" \
				"gl_Position = u_mvpMatrix * vPosition;" \
				"out_color = vColor;" \
			"}";
		glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
		glCompileShader(gVSObj);
		[self shaderErrorCheck : gVSObj option : "COMPILE"];
		printf("Vertx shader compiled successfully...\n");
		
		// Fragment Shader
		gFSObj = glCreateShader(GL_FRAGMENT_SHADER);
		const GLchar *FSSrcCode =
			"#version 300 es\n" \
			"precision highp float;" \
			"in vec3 out_color;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
				"FragColor = vec4(out_color, 1.0f);" \
			"}";
		glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
		glCompileShader(gFSObj);
		[self shaderErrorCheck : gFSObj option : "COMPILE"];
		printf("Fragment shader compiled successfully...\n");
		
		// Shader program
		gSPObj = glCreateProgram();
		glAttachShader(gSPObj, gVSObj);
		glAttachShader(gSPObj, gFSObj);
		glBindAttribLocation(gSPObj, DV_ATTRIB_POS, "vPosition");
		glLinkProgram(gSPObj);
		[self shaderErrorCheck : gSPObj option : "LINK"];
		printf("Shader program linked successfully...\n");
		
		// get uniform location(s)
		gMVPMatrixUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");

		// other global variable initialization
		const GLfloat Alphabet_I_vertex[] = {
			// Top quad of I
			-0.35f, 0.5f,	 0.35f, 0.5f,	 0.35f, 0.4f,	-0.35f, 0.4f,
			// Vertical quad (Upper half) of I
			-0.05f, 0.4f,	 0.05f, 0.4f,	 0.05f, 0.0f,	-0.05f, 0.0f,
			// Vertical quad (Lower half) of I
			 0.05f, 0.0f,	-0.05f, 0.0f,	-0.05f, -0.4f,	 0.05f, -0.4f,
			// Bottom quad of I
			-0.35f, -0.5f,	 0.35f, -0.5f,	 0.35f, -0.4f,	-0.35f, -0.4f
		};
		const GLfloat Alphabet_I_color[] = {
			// Top quad of I - Saffron
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,
			// Vertical quad (Upper half) of I - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// Vertical quad (Lower half) of I - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// Bottom quad of I - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};
		const GLfloat Alphabet_N_vertex[] = {
			// 1st Vertical quad (Upper half)
			-0.35f, 0.5f,	-0.25f, 0.5f,	-0.25f, 0.0f,	-0.35f, 0.0f,
			// 1st Vertical quad (Lower half)
			-0.35f, 0.0f,	-0.25f, 0.0f,	-0.25f, -0.5f,	-0.35f, -0.5f,
			// Slant quad (Upper half)
			-0.35f, 0.5f,	-0.25f, 0.5f,	0.05f, 0.0f,	-0.05f, 0.0f,
			// Slant quad (lower half)
			0.05f, 0.0f,	-0.05f, 0.0f,	0.25f, -0.5f,	0.35f, -0.5f,
			// 2nd Vertical quad (upper half)
			0.35f, 0.5f,	0.25f, 0.5f,	0.25f, 0.0f,	0.35f, 0.0f,
			// 2nd Vertical quad (lower half)
			0.35f, 0.0f,	0.25f, 0.0f,	0.25f, -0.5f,	0.35f, -0.5f
		};
		const GLfloat Alphabet_N_color[] = {
			// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 1st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// Slant quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// Slant quad (lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// 2nd Vertical quad (upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 2nd Vertical quad (lower half) - White, White, Green Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};
		const GLfloat Alphabet_D_vertex[] = {
			// 1st Vertical quad (Upper half)
			-0.25f, 0.4f,	-0.15f, 0.4f,	-0.15f, 0.0f,	-0.25f, 0.0f,
			// 1st Vertical quad (Lower half)
			-0.25f, 0.0f,	-0.15f, 0.0f,	-0.15f, -0.4f,	-0.25f, -0.4f,
			// Upper horizontal quad
			-0.35f, 0.5f,	0.35f, 0.5f,	0.35f, 0.4f,	-0.35f, 0.4f,
			// 2nd Vertical quad (Upper half)
			0.35f, 0.4f,	0.25f, 0.4f,	0.25f, 0.0f,	0.35f, 0.0f,
			// 2nd Vertical quad (Lower half)
			0.35f, 0.0f,	0.25f, 0.0f,	0.25f, -0.4f,	0.35f, -0.4f,
			// Lower horizontal quad
			-0.35f, -0.5f,	0.35f, -0.5f,	0.35f, -0.4f,	-0.35f, -0.4f
		};
		const GLfloat Alphabet_D_color[] = {
			// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 1st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// Upper horizontal quad - Saffron
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,
			// 2nd Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 2nd Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// Lower horizontal quad - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};
		const GLfloat Alphabet_A_vertex[] = {
			// Central Horizontal
			0.25f, 0.05f,	0.25f, -0.05f,	-0.25f, -0.05f,	-0.25f, 0.05f,
			// 1st Vertical quad (Upper half)
			-0.35f, 0.4f,	-0.25f, 0.4f,	-0.25f, 0.0f,	-0.35f, 0.0f,
			// 1st Vertical quad (Lower half)
			-0.35f, 0.0f,	-0.25f, 0.0f,	-0.25f, -0.4f,	-0.35f, -0.4f,
			// Upper Horizontal quad
			0.35f, 0.5f,	0.35f, 0.4f,	-0.35f, 0.4f,	-0.35f, 0.5f,
			// 2nd Vertical quad (Upper half)
			0.35f, 0.4f,	0.25f, 0.4f,	0.25f, 0.0f,	0.35f, 0.0f,
			// 2nd Vertical quad (Lower half)
			0.35f, 0.0f,	0.25f, 0.0f,	0.25f, -0.4f,	0.35f, -0.4f,
			// 1st Vertical quad (Lowest part square)
			-0.25f, -0.4f,	-0.35f, -0.4f,	-0.35f, -0.5f,	-0.25f, -0.5f,
			// 2nd Vertical quad (Lowest part square)
			0.25f, -0.4f,	0.35f, -0.4f,	0.35f, -0.5f,	0.25f, -0.5f
		};
		const GLfloat Alphabet_A_color[] = {
			// Central Horizontal - White
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 1st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// Upper Horizontal quad - Saffron
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,
			// 2st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,
			// 2st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// 1st Vertical quad (Lowest part square) - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,
			// 2st Vertical quad (Lowest part square) - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};

		glGenVertexArrays(4, gVAObj_INDIA);
		glBindVertexArray(gVAObj_INDIA[0]);
			glGenBuffers(2, gVBObj_I);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_I[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_I_vertex), Alphabet_I_vertex, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_I[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_I_color), Alphabet_I_color, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindVertexArray(gVAObj_INDIA[1]);
			glGenBuffers(2, gVBObj_N);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_N[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_N_vertex), Alphabet_N_vertex, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_N[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_N_color), Alphabet_N_color, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindVertexArray(gVAObj_INDIA[2]);
			glGenBuffers(2, gVBObj_D);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_D[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_D_vertex), Alphabet_D_vertex, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_D[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_D_color), Alphabet_D_color, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindVertexArray(gVAObj_INDIA[3]);
			glGenBuffers(2, gVBObj_A);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_A[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_A_vertex), Alphabet_A_vertex, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_A[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_A_color), Alphabet_A_color, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		gPerspProjMatrix = mat4::identity();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepthf(1.0f);
		
		// Gesture
		UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(onSingleTap:)];
		[singleTap setNumberOfTapsRequired: 1];
		[singleTap setNumberOfTouchesRequired: 1];
		[singleTap setDelegate: self];
		[self addGestureRecognizer: singleTap];
		
		UITapGestureRecognizer *doubleTap = [[UITapGestureRecognizer alloc] initWithTarget: self action: @selector(onDoubleTap:)];
		[doubleTap setNumberOfTapsRequired: 2];
		[doubleTap setNumberOfTouchesRequired: 1];
		[doubleTap setDelegate: self];
		[self addGestureRecognizer: doubleTap];
		
		[singleTap requireGestureRecognizerToFail: doubleTap];
		
		UISwipeGestureRecognizer *swipe = [[UISwipeGestureRecognizer alloc] initWithTarget: self action: @selector(onSwipe:)];
		[swipe setNumberOfTouchesRequired: 1];
		[swipe setDelegate: self];
		[self addGestureRecognizer: swipe];
		
		UILongPressGestureRecognizer *longPress = [[UILongPressGestureRecognizer alloc] initWithTarget: self action: @selector(onLongPress:)];
		[longPress setNumberOfTouchesRequired: 1];
		[longPress setDelegate: self];
		[self addGestureRecognizer: longPress];
	}
	return self;
}
+(Class)layerClass {
	// Code
	return [CAEAGLLayer class];
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
		printf("Invalid second parameter in ShaderErrorCheck()");
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
					printf("Shader Compilation Error log : \n");
				}
				else if(strcmp(shaderOpr, "LINK") == 0) {
					glGetProgramInfoLog(shaderObject, iErrorLen, &written, szError);
					printf("Shader linking Error log : \n");
				}
				printf("%s \n", szError);
				free(szError);
				szError = NULL;
			}
		}
		else
			printf("Error occured during compilation. No error message. \n");
		[self Uninitialize];
		[self release];
		exit(0);
	}
}
-(void)onSingleTap: (UITapGestureRecognizer *)gesture {
	// Code
}
-(void)onDoubleTap: (UITapGestureRecognizer *)gesture {
	// Code
}
-(void)onLongPress: (UILongPressGestureRecognizer *)gesture {
	// Code
}
-(void)onSwipe: (UISwipeGestureRecognizer *)gesture {
	// Code
	[self Uninitialize];
	[self release];
	exit(0);
}
/*		// Only for immediate mode rendering
-(void)drawRect: (CGRect)rect {
    // Drawing code
}
*/
-(void)layoutSubviews {
	// Code
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
	[eaglContext renderbufferStorage: GL_RENDERBUFFER fromDrawable: (CAEAGLLayer *)[self layer]];
	
	GLint width, height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
	
	// Creating depth buffer
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("Frame buffer is not complete in layoutSubviewa() !!!\n");
	
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	gPerspProjMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	
	[self display];
}
-(void)startAnimation {
	// Code
	if(animationEnabled == NO) {
		displayLink = (CADisplayLink *)[NSClassFromString(@"CADisplayLink") displayLinkWithTarget: self selector: @selector(display)];
		[displayLink setPreferredFramesPerSecond: animationFrameInterval];
		[displayLink addToRunLoop: [NSRunLoop currentRunLoop] forMode: NSDefaultRunLoopMode];
		
		animationEnabled = !animationEnabled;
	}
}
-(void)stopAnimation {
	// Code
	if(animationEnabled) {
		[displayLink invalidate];
		displayLink = nil;

		animationEnabled = !animationEnabled;
	}
}
-(void)display {
	// Variable declaration
	mat4 MVMatrix, MVPMatrix;
	mat4 translationMatrix;
	
	// Code
	[EAGLContext setCurrentContext: eaglContext];
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	
	// OpenGL code
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	// Initialization
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	// Setting Values
	translationMatrix = translate(0.0f, 0.0f, -4.0f);
	MVMatrix = MVMatrix * translationMatrix;
	
	// For I
	translationMatrix = translate(-2.0f, 0.0f, 0.0f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	[self I];
	
	// For N
	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	[self N];
	
	// For D
	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	[self D];
	
	// For I
	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	[self I];
	
	// For A
	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	MVMatrix = MVMatrix * translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	[self A];
	
	// End of OpenGL shading program
	glUseProgram(0);
	
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
	
	// Similar to swapBuffer(Windows) / glXSwapBuffers(Linux)
	[eaglContext presentRenderbuffer: GL_RENDERBUFFER];
}
-(void)I {
	// Code
	glBindVertexArray(gVAObj_INDIA[0]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glBindVertexArray(0);
}
-(void)N {
	// Code
	glBindVertexArray(gVAObj_INDIA[1]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
}
-(void)D {
	// Code
	glBindVertexArray(gVAObj_INDIA[2]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
}
-(void)A {
	// Code
	glBindVertexArray(gVAObj_INDIA[3]);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 28, 4);
	glBindVertexArray(0);
}
-(void)Uninitialize {
	if(depthRenderBuffer) {
		glDeleteRenderbuffers(1, &depthRenderBuffer);
		depthRenderBuffer = 0;
	}
	if(colorRenderBuffer) {
		glDeleteRenderbuffers(1, &colorRenderBuffer);
		colorRenderBuffer = 0;
	}
	if(defaultFrameBuffer) {
		glDeleteRenderbuffers(1, &defaultFrameBuffer);
		defaultFrameBuffer = 0;
	}
	if([EAGLContext currentContext] == eaglContext) {
		[EAGLContext setCurrentContext: nil];
		[eaglContext release];
		eaglContext = nil;
	}
}
-(void)dealloc {
	// Code
	
	// Unlink shader program (if not unlinked earlier)
	glUseProgram(0);
	
	// Destroy Vertex Array Object
	if(gVAObj_INDIA) {
		glDeleteVertexArrays(4, gVAObj_INDIA);
		gVAObj_INDIA[0] = 0;
		gVAObj_INDIA[1] = 0;
		gVAObj_INDIA[2] = 0;
		gVAObj_INDIA[3] = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_I) {
		glDeleteBuffers(2, gVBObj_I);
		gVBObj_I[0] = 0;
		gVBObj_I[1] = 0;
	}
	if(gVBObj_N) {
		glDeleteBuffers(2, gVBObj_N);
		gVBObj_N[0] = 0;
		gVBObj_N[1] = 0;
	}
	if(gVBObj_D) {
		glDeleteBuffers(2, gVBObj_D);
		gVBObj_D[0] = 0;
		gVBObj_D[1] = 0;
	}
	if(gVBObj_A) {
		glDeleteBuffers(2, gVBObj_A);
		gVBObj_A[0] = 0;
		gVBObj_A[1] = 0;
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
	[self Uninitialize];
}
@end
