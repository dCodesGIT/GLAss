//
//  OpenGLESView.m
//  Pyramid with 2 lights
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
	GLuint gVAObj;
	GLuint gVBObj_pyramid[2];
	
	GLuint gMMatrixUniform, gVMatrixUniform, gPMatrixUniform;
	GLuint gLAmbUniform, gLDiffUniform, gLSpecUniform, gLPosUniform;
	GLuint gKAmbUniform, gKDiffUniform, gKSpecUniform, gKShineUniform;
	GLuint gLightEnabledUniform;
	
	bool lightEnabled;
	bool modelAnimationEnabled;
	float rotAngle;
	
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
			"in vec4 vPosition;" \
			"in vec3 vNormal;" \
			"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
			"uniform vec3 u_LAmb[2], u_LDiff[2], u_LSpec[2];" \
			"uniform vec4 u_LPos[2];" \
			"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
			"uniform float u_KShine;" \
			"uniform bool u_LightEnabled;" \
			"out vec3 out_light;"\
			"void main(void) {" \
				"out_light = vec3(0.0);" \
				"if(u_LightEnabled) {" \
					"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
					"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" \
					"vec3 viewVector = normalize(-eyeCoords.xyz);" \
					"vec3 lightSource, reflectionVector;" \
					"vec3 ambient, diffuse, specular;" \
					"for(int i = 0; i < 2; i++) {" \
						"lightSource = normalize(vec3(u_LPos[i] - eyeCoords));" \
						"reflectionVector = reflect(-lightSource, transformedNormal);" \
						"ambient = u_LAmb[i] * u_KAmb;" \
						"diffuse = u_LDiff[i] * u_KDiff * max(dot(lightSource, transformedNormal), 0.0);" \
						"specular = u_LSpec[i] * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
						"out_light += (ambient + diffuse + specular);" \
					"}" \
				"}" \
				"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
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
			"in vec3 out_light;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
				"FragColor = vec4(out_light, 1.0f);" \
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
		glBindAttribLocation(gSPObj, DV_ATTRIB_NORM, "vNormal");
		glLinkProgram(gSPObj);
		[self shaderErrorCheck : gSPObj option : "LINK"];
		printf("Shader program linked successfully...\n");
		
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
		gLightEnabledUniform = glGetUniformLocation(gSPObj, "u_LightEnabled");
		
		// other global variable initialization
		const GLfloat PyramidVertex[] = {
			// Front face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	-1.0f, -1.0f, 1.0f,	1.0f, -1.0f, 1.0f,
			// Right face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	1.0f, -1.0f, 1.0f,	1.0f, -1.0f, -1.0f,
			// Back face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,
			// Left face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, 1.0f
		};
		const GLfloat PyramidNormal[] = {
			// Front face - Apex, Left bottom, Right bottom
			0.0f, 0.447214f, 0.894427f,	0.0f, 0.447214f, 0.894427f,	0.0f, 0.447214f, 0.894427f,
			// Right face - Apex, Left bottom, Right bottom
			0.894427f, 0.447214f, 0.0f,	0.894427f, 0.447214f, 0.0f,	0.894427f, 0.447214f, 0.0f,
			// Back face - Apex, Left bottom, Right bottom
			0.0f, 0.447214f, -0.894427f,	0.0f, 0.447214f, -0.894427f,	0.0f, 0.447214f, -0.894427f,
			// Left face - Apex, Left bottom, Right bottom
			-0.894427f, 0.447214f, 0.0f,	-0.894427f, 0.447214f, 0.0f,	-0.894427f, 0.447214f, 0.0f
		};
		
		glGenVertexArrays(1, &gVAObj);
		glBindVertexArray(gVAObj);
			glGenBuffers(2, gVBObj_pyramid);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pyramid[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidVertex), PyramidVertex, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pyramid[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidNormal), PyramidNormal, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_NORM);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Depth related OpenGL code
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearDepthf(1.0f);
		
		lightEnabled = false;
		modelAnimationEnabled = false;
		
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
	lightEnabled = !lightEnabled;
}
-(void)onDoubleTap: (UITapGestureRecognizer *)gesture {
	// Code
	modelAnimationEnabled = !modelAnimationEnabled;
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
	mat4 MMatrix, VMatrix, PMatrix;
	mat4 translationMatrix, rotationMatrix;
	vec3 materialAmbient, materialDiffuse, materialSpecular;
	GLfloat materialShininess;

	// Code
	[EAGLContext setCurrentContext: eaglContext];
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	
	// OpenGL code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	if(lightEnabled) {
		GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f,
			0.25f, 0.25f, 0.25f };
		GLfloat lightDiffuse[] = { 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f };
		GLfloat lightSpecular[] = { 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f };
		GLfloat lightPosition[] = { 2.0f, 0.0f, 0.0f, 1.0f,
			-2.0f, 0.0f, 0.0f, 1.0f };
		materialAmbient = vec3(0.0f, 0.0f, 0.0f);
		materialDiffuse = vec3(1.0f, 1.0f, 1.0f);
		materialSpecular = vec3(1.0f, 1.0f, 1.0f);
		materialShininess = 50.0f;
		
		glUniform1i(gLightEnabledUniform, true);
		glUniform3fv(gLAmbUniform, 2, lightAmbient);
		glUniform3fv(gLDiffUniform, 2, lightDiffuse);
		glUniform3fv(gLSpecUniform, 2, lightSpecular);
		glUniform4fv(gLPosUniform, 2, lightPosition);
		glUniform3fv(gKAmbUniform, 1, materialAmbient);
		glUniform3fv(gKDiffUniform, 1, materialDiffuse);
		glUniform3fv(gKSpecUniform, 1, materialSpecular);
		glUniform1fv(gKShineUniform, 1, &materialShininess);
	}
	else
		glUniform1i(gLightEnabledUniform, false);

	// Initialization
	MMatrix = mat4::identity();
	VMatrix = mat4::identity();
	PMatrix = gPerspProjMatrix;
	translationMatrix = mat4::identity();
	// Setting values
	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	VMatrix *= translationMatrix;
	rotationMatrix = rotate(rotAngle, 0.0f, 1.0f, 0.0f);
	VMatrix *= rotationMatrix;
	glUniformMatrix4fv(gMMatrixUniform, 1, GL_FALSE, MMatrix);
	glUniformMatrix4fv(gVMatrixUniform, 1, GL_FALSE, VMatrix);
	glUniformMatrix4fv(gPMatrixUniform, 1, GL_FALSE, PMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj);
		glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);
	
	// End of OpenGL shading program
	glUseProgram(0);
	
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
	
	if(modelAnimationEnabled)
		rotAngle += 0.5f;
	if(rotAngle >= 360.0f)
		rotAngle = 0.0f;
	
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
		glDeleteVertexArrays(1, &gVAObj);
		gVAObj = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_pyramid) {
		glDeleteBuffers(2, gVBObj_pyramid);
		gVBObj_pyramid[0] = 0;
		gVBObj_pyramid[1] = 0;
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
