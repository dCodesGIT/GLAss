//
//  OpenGLESView.m
//  Shapes on graph paper
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
	GLuint gVAObj_gp;
	GLuint gVBObj_gp[2];
	GLuint gVAObj_shapes[3];
	GLuint gVBObj_triangle[2];
	GLuint gVBObj_rectangle[2];
	GLuint gVBObj_circle[2];
	GLuint gMVPMatrixUniform;
	
	GLfloat triangleSide;
	GLfloat rectangleWidth, rectangleHeight;
	GLfloat circleRadius;
	bool gbAnimate;
	float rotAngle;
	int shapeSelected;
	
	GLfloat circleVertices[3600*3];
	GLfloat circleColor[3600*3];
	
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
				"gl_PointSize = 1.0;" \
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
		glGenVertexArrays(3, gVAObj_shapes);
		triangleSide = 1.0f;
		const GLfloat triangleVertices[] = {
			0.0f, triangleSide, 0.0f,		// Apex
			-triangleSide, -triangleSide, 0.0f,	// left bottom
			triangleSide, -triangleSide, 0.0f	// right bottom
		};
		const GLfloat triangleColors[] = {
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f
		};
		glBindVertexArray(gVAObj_shapes[0]);	// For triangle
			glGenBuffers(2, gVBObj_triangle);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_triangle[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColors), triangleColors, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		rectangleWidth = 1.0f;
		rectangleHeight = 1.0f;
		const GLfloat rectangleVertices[] = {
			-rectangleWidth,  rectangleHeight, 0.0f,	// Top left
			-rectangleWidth, -rectangleHeight, 0.0f,	// Bottom left
			 rectangleWidth, -rectangleHeight, 0.0f,	// Bottom right
			 rectangleWidth,  rectangleHeight, 0.0f		// Top right
		};
		const GLfloat rectangleColors[] = {
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f
		};
		glBindVertexArray(gVAObj_shapes[1]);	// For rectangle
			glGenBuffers(2, gVBObj_rectangle);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_rectangle[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColors), rectangleColors, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		circleRadius = 1.0f;
		for(int i = 0; i < 3600*3; i += 3) {
			circleVertices[i] = circleRadius * cos(((GLfloat)i/30.0f)*(M_PI/180.0f));	// X = r * cos(theta)
			circleVertices[i+1] = circleRadius * sin(((GLfloat)i/30.0f)*(M_PI/180.0f));	// Y = r * sin(theta)
			circleVertices[i+2] = 0.0f;
		}
		for(int i = 0; i < 3600*3; i += 3) {
			circleColor[i] = 1.0f;
			circleColor[i+1] = 1.0f;
			circleColor[i+2] = 0.0f;
		}
		glBindVertexArray(gVAObj_shapes[2]);	// For circle
			glGenBuffers(2, gVBObj_circle);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[0]);
			glBufferData(GL_ARRAY_BUFFER, 3600 * 3 * sizeof(GLfloat), circleVertices, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[1]);
			glBufferData(GL_ARRAY_BUFFER, 3600 * 3 * sizeof(GLfloat), circleColor, GL_DYNAMIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		shapeSelected = 0;
		rotAngle = 0.0f;
		gbAnimate = NO;
		
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
	shapeSelected++;
	if(shapeSelected > 3)
		shapeSelected = -1;
}
-(void)onDoubleTap: (UITapGestureRecognizer *)gesture {
	// Code
	gbAnimate = !gbAnimate;
	rotAngle = 0.0f;
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
		glDrawArrays(GL_LINES, 0, 164*2);
	glBindVertexArray(0);
	
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	translationMatrix = translate(0.0f, 0.0f, -5.5f);
	MVMatrix *= translationMatrix;
	MVPMatrix = gPerspProjMatrix * MVMatrix;
	glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
	if(shapeSelected == 1 || shapeSelected == -1) {		// Triangle
		if(gbAnimate) {
			rotationMatrix = rotate(rotAngle, 0.0f, 1.0f, 0.0f);
			MVPMatrix = gPerspProjMatrix * MVMatrix * rotationMatrix;
			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
		}
		glBindVertexArray(gVAObj_shapes[0]);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		glBindVertexArray(0);
	}
	if(shapeSelected == 2 || shapeSelected == -1) {		// rectangle
		if(gbAnimate) {
			rotationMatrix = rotate(rotAngle, 1.0f, 0.0f, 0.0f);
			MVPMatrix = gPerspProjMatrix * MVMatrix * rotationMatrix;
			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
		}
		glBindVertexArray(gVAObj_shapes[1]);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
		glBindVertexArray(0);
	}
	if(shapeSelected == 3 || shapeSelected == -1) {		// Circle
		if(gbAnimate) {
			rotationMatrix = rotate(rotAngle, 1.0f, 0.0f, 0.0f) * rotate(rotAngle * 2, 0.0f, 1.0f, 0.0f) * rotate(rotAngle * 3, 0.0f, 0.0f, 1.0f);
			MVPMatrix = gPerspProjMatrix * MVMatrix * rotationMatrix;
			glUniformMatrix4fv(gMVPMatrixUniform, 1, GL_FALSE, MVPMatrix);
		}
		glBindVertexArray(gVAObj_shapes[2]);
//			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[0]);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_DYNAMIC_DRAW);
//			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//			glEnableVertexAttribArray(DV_ATTRIB_POS);
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_circle[1]);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_DYNAMIC_DRAW);
//			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//			glEnableVertexAttribArray(DV_ATTRIB_COL);
//			glBindBuffer(GL_ARRAY_BUFFER, 0);
//
			glDrawArrays(GL_POINTS, 0, 3600);
		glBindVertexArray(0);
	}
	
	// End of OpenGL shading program
	glUseProgram(0);
	
	if(gbAnimate)
		rotAngle += 0.5f;
	if(rotAngle >= 360.0f)
		rotAngle = 0.0f;
	
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
	[self Uninitialize];
}
@end
