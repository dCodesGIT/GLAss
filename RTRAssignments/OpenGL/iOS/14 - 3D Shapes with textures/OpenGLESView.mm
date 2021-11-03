//
//  OpenGLESView.m
//  3D Shapes with textures
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
	GLuint gVAObj[2];
	GLuint gVBObj_pyramid[2];
	GLuint gVBObj_cube[2];
	GLuint gMVPMatrixUniform;
	GLuint gTextureSamplerUniform;
	
	GLuint stone_texture, kundali_texture;
	GLfloat rotAngle;
	
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
			"in vec2 vTexCoords;" \
			"uniform mat4 u_mvpMatrix;" \
			"out vec3 out_color;" \
			"out vec2 out_texCoord;" \
			"void main(void) {" \
				"gl_Position = u_mvpMatrix * vPosition;" \
				"out_color = vColor;" \
				"out_texCoord = vTexCoords;" \
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
			"in vec3 out_color;" \
			"in vec2 out_texCoord;" \
			"uniform sampler2D u_texture_sampler;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
/*				"FragColor = vec4(out_color, 1.0f);" \
*/				"FragColor = texture(u_texture_sampler, out_texCoord);" \
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
		glBindAttribLocation(gSPObj, DV_ATTRIB_COL, "vColor");
		glBindAttribLocation(gSPObj, DV_ATTRIB_TEX, "vTexCoords");
		glLinkProgram(gSPObj);
		[self shaderErrorCheck : gSPObj option : "LINK"];
		printf("Shader program linked successfully...\n");
		
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
			/*
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pyramid[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidColors), PyramidColors, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			*/
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
			/*
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_cube[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(CubeColors), CubeColors, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_COL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_COL);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			*/
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_cube[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(CubeTexture), CubeTexture, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_TEX);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Depth related OpenGL code
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearDepthf(1.0f);
		
		// Loading texture
		stone_texture = [self loadGLTexture: @"Stone" extension: @"png"];
		if(stone_texture == 0) {
			[self release];
			[self Uninitialize];
			exit(0);
		}
		kundali_texture = [self loadGLTexture: @"Kundali" extension: @"png"];
		if(kundali_texture == 0) {
			[self release];
			[self Uninitialize];
			exit(0);
		}
		
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
-(GLuint)loadGLTexture: (NSString *)imgFileName extension: (NSString *)ext {
	// Code
	NSString *imgPath = [[NSBundle mainBundle] pathForResource:imgFileName ofType:ext];
	
	UIImage *bmpImage = [[UIImage alloc] initWithContentsOfFile : imgPath];
	if(!bmpImage) {
		printf("UIImage convertion of image file failed !!!\n");
		return 0;
	}
	CGImageRef cgImage = [bmpImage CGImage];
	int imgWidth = (int)CGImageGetWidth(cgImage);
	int imgHeight = (int)CGImageGetHeight(cgImage);
	CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
	void *pixels = (void *)CFDataGetBytePtr(imageData);
	
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
	
	glUseProgram(gSPObj);
	
	// Initialization
	MVMatrix = mat4::identity();
	MVPMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	// Setting values
	translationMatrix = translate(-2.0f, 0.0f, -6.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(rotAngle, 0.0f, 1.0f, 0.0f);
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
	translationMatrix = translate(2.0f, 0.0f, -6.0f);
	MVMatrix *= translationMatrix;
	rotationMatrix = rotate(rotAngle, 1.0f, 0.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	rotationMatrix = rotate(rotAngle, 0.0f, 1.0f, 0.0f);
	MVMatrix *= rotationMatrix;
	rotationMatrix = rotate(rotAngle, 0.0f, 0.0f, 1.0f);
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
	[self Uninitialize];
}
@end