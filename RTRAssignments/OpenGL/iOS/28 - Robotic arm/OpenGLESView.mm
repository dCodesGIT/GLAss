//
//  OpenGLESView.m
//  Robotic arm
//
//  Created by Darshan Vikam on 13/08/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "OpenGLESView.h"
#import "vmath.h"
#import "sphere.h"
#import "PushPop.h"

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
			"uniform mat4 u_mvpMatrix;" \
			"void main(void) {" \
				"gl_Position = u_mvpMatrix * vPosition;" \
			"}";
		glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
		glCompileShader(gVSObj);
		[self shaderErrorCheck : gVSObj option : "COMPILE"];
		printf("Vertex shader compiled successfully...\n");
		
		// Fragment Shader
		gFSObj = glCreateShader(GL_FRAGMENT_SHADER);
		const GLchar *FSSrcCode =
			"#version 300 es\n" \
			"precision highp float;" \
			"uniform vec4 u_color;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
				"FragColor = u_color;" \
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
		colorUniform = glGetUniformLocation(gSPObj, "u_color");

		// other global variable initialization
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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearDepthf(1.0f);
		
		gPerspProjMatrix = mat4::identity();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
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
	mat4 translationMatrix, rotationMatrix;
	
	// Code
	[EAGLContext setCurrentContext: eaglContext];
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	
	// OpenGL code
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

	elbow += 3;
	if(elbow > 360) {
		elbow = 0;
		shoulder++;
	}
	shoulder++;
	if(shoulder > 360)
		shoulder = 0;
	
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
	
	// Similar to swapBuffer(Windows) / glXSwapBuffers(Linux)
	[eaglContext presentRenderbuffer: GL_RENDERBUFFER];
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
