//
//  OpenGLESView.m
//  24 Spheres
//
//  Created by Darshan Vikam on 13/08/21.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import "OpenGLESView.h"
#import "vmath.h"
#import "sphere.h"

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
	GLuint gVBObj_sphere[2];
	
	GLuint gMMatrixUniform, gVMatrixUniform, gPMatrixUniform;
	GLuint gLAmbUniform, gLDiffUniform, gLSpecUniform, gLPosUniform;
	GLuint gKAmbUniform, gKDiffUniform, gKSpecUniform, gKShineUniform;
	GLuint gLightEnabledUniform;
	
	int pointCnt;
	MySphere *sphere;
	GLfloat *sphereVertex, *sphereNormal;
	
	bool lightEnabled;
	bool XRotationEnabled, YRotationEnabled, ZRotationEnabled;
	float rotAngle;
	int rotationDirection;
	
	float gWidth, gHeight;
	
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
			"uniform vec4 u_LPos;" \
			"uniform bool u_LightEnabled;" \
			"out vec3 out_tNorm, out_lSrc, out_viewVec;"\
			"void main(void) {" \
				"if(u_LightEnabled) {" \
					"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
					"out_tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" \
					"out_lSrc = vec3(u_LPos - eyeCoords);" \
					"out_viewVec = -eyeCoords.xyz;" \
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
			"in vec3 out_tNorm, out_lSrc, out_viewVec;" \
			"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" \
			"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
			"uniform float u_KShine;" \
			"uniform bool u_LightEnabled;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
				"vec3 light = vec3(0.0);" \
				"if(u_LightEnabled) {" \
					"vec3 transformedNormal = normalize(out_tNorm);" \
					"vec3 lightSource = normalize(out_lSrc);" \
					"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
					"vec3 viewVector = normalize(out_viewVec);" \
					"vec3 ambient = u_LAmb * u_KAmb;" \
					"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0);" \
					"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
					"light = ambient + diffuse + specular;" \
				"}" \
			"FragColor = vec4(light, 1.0);" \
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
		sphere = [[MySphere alloc] init];
		pointCnt = [sphere GenSphere : 1.0f stack : 45 slices : 45];
		sphereVertex = (GLfloat *)[sphere getSphereVertices];
		sphereNormal = (GLfloat *)[sphere getSphereNormals];
		
		glGenVertexArrays(1, &gVAObj);
		glBindVertexArray(gVAObj);
			glGenBuffers(2, gVBObj_sphere);
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_sphere[0]);
			glBufferData(GL_ARRAY_BUFFER, pointCnt * 3 * sizeof(GLfloat), sphereVertex, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_POS, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_POS);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		
			glBindBuffer(GL_ARRAY_BUFFER, gVBObj_sphere[1]);
			glBufferData(GL_ARRAY_BUFFER, pointCnt * 3 * sizeof(float), sphereNormal, GL_STATIC_DRAW);
			glVertexAttribPointer(DV_ATTRIB_NORM, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(DV_ATTRIB_NORM);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Depth related OpenGL code
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearDepthf(1.0f);
		
		lightEnabled = false;
		XRotationEnabled = false;
		YRotationEnabled = false;
		ZRotationEnabled = false;
		
		gPerspProjMatrix = mat4::identity();

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		
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
	rotationDirection++;
	if(rotationDirection > 3)
		rotationDirection = 0;
	if(rotationDirection == 0) {
		XRotationEnabled = true;
		YRotationEnabled = false;
		ZRotationEnabled = false;
	}
	else if(rotationDirection == 2) {
		XRotationEnabled = false;
		YRotationEnabled = true;
		ZRotationEnabled = false;
	}
	else if(rotationDirection == 3) {
		XRotationEnabled = false;
		YRotationEnabled = false;
		ZRotationEnabled = true;
	}
	else {
		XRotationEnabled = false;
		YRotationEnabled = false;
		ZRotationEnabled = false;
	}
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
	
//	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gWidth = width;
	gHeight = height;
	
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
	mat4 translationMatrix;
	GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[4];
	GLfloat materialAmbient[][3] =
	{
		{0.0215f, 0.1745f, 0.0215f},	// 1R 1C - Emerald
		{0.135f, 0.2225f, 0.1575f},	// 2R 1C - Jade
		{0.05375f, 0.05f, 0.06625f},	// 3R 1C - Obsidian
		{0.25f, 0.20725f, 0.20725f},	// 4R 1C - Pearl
		{0.1745f, 0.01175f, 0.01175f},	// 5R 1C - Ruby
		{0.1f, 0.18725f, 0.1745f},	// 6R 1C - Turquoise
		{0.329412f, 0.223529f, 0.027451f},// 1R 2C - Brass
		{0.2125f, 0.1275f, 0.054f},	// 2R 2C - Bronze
		{0.25f, 0.25f, 0.25f},		// 3R 2C - Chrome
		{0.19125f, 0.0735f, 0.0225f},	// 4R 2C - Copper
		{0.24725f, 0.1995f, 0.0745f},	// 5R 2C - Gold
		{0.19225f, 0.19225f, 0.19225f},	// 6R 2C - Silver
		{0.0f, 0.0f, 0.0f},		// 1R 3C - Black plastic
		{0.0f, 0.1f, 0.06f},		// 2R 3C - Cyan plastic
		{0.0f, 0.0f, 0.0f},		// 3R 3C - Green plastic
		{0.0f, 0.0f, 0.0f},		// 4R 3C - Red plastic
		{0.0f, 0.0f, 0.0f},		// 5R 3C - White plastic
		{0.0f, 0.0f, 0.0f},		// 6R 3C - Yellow plastic
		{0.02f, 0.02f, 0.02f},		// 1R 4C - Black rubber
		{0.0f, 0.05f, 0.05f},		// 2R 4C - Cyan rubber
		{0.0f, 0.05f, 0.0f},		// 3R 4C - Green rubber
		{0.05f, 0.0f, 0.0f},		// 4R 4C - Red rubber
		{0.05f, 0.05f, 0.05f},		// 5R 4C - White rubber
		{0.05f, 0.05f, 0.04f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialDiffuse[][3] =
	{
		{0.07568f, 0.61424f, 0.07568f},	// 1R 1C - Emerald
		{0.54f, 0.89f, 0.63f},		// 2R 1C - Jade
		{0.18275f, 0.17f, 0.22525f},	// 3R 1C - Obsidian
		{1.0f, 0.829f, 0.829f},		// 4R 1C - Pearl
		{0.61424f, 0.04136f, 0.04163f},	// 5R 1C - Ruby
		{0.396f, 0.74151f, 0.69102f},	// 6R 1C - Turquoise
		{0.780392f, 0.568627f, 0.113725f},// 1R 2C - Brass
		{0.714f, 0.4284f, 0.18144f},	// 2R 2C - Bronze
		{0.4f, 0.4f, 0.4f},		// 3R 2C - Chrome
		{0.7038f, 0.27048f, 0.0828f},	// 4R 2C - Copper
		{0.75164f, 0.60648f, 0.22648f},	// 5R 2C - Gold
		{0.50754f, 0.50754f, 0.50754f},	// 6R 2C - Silver
		{0.01f, 0.01f, 0.01f},		// 1R 3C - Black plastic
		{0.0f, 0.50980392f, 0.50980392f},// 2R 3C - Cyan plastic
		{0.1f, 0.35f, 0.1f},		// 3R 3C - Green plastic
		{0.5f, 0.0f, 0.0f},		// 4R 3C - Red plastic
		{0.55f, 0.55f, 0.55f},		// 5R 3C - White plastic
		{0.5f, 0.5f, 0.0f},		// 6R 3C - Yellow plastic
		{0.01f, 0.01f, 0.01f},		// 1R 4C - Black rubber
		{0.4f, 0.5f, 0.5f},		// 2R 4C - Cyan rubber
		{0.4f, 0.5f, 0.4f},		// 3R 4C - Green rubber
		{0.5f, 0.4f, 0.4f},		// 4R 4C - Red rubber
		{0.5f, 0.5f, 0.5},		// 5R 4C - White rubber
		{0.5f, 0.5f, 0.4f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialSpecular[][3] =
	{
		{0.633f, 0.727811f, 0.33f},		// 1R 1C - Emerald
		{0.316228f, 0.316228f, 0.316228f},	// 2R 1C - Jade
		{0.332741f, 0.328634f, 0.346435f},	// 3R 1C - Obsidian
		{0.296648f, 0.296648f, 0.296648f},	// 4R 1C - Pearl
		{0.727811f, 0.626959f, 0.626959f},	// 5R 1C - Ruby
		{0.297254f, 0.308290f, 0.306678f},	// 6R 1C - Turquoise
		{0.992157f, 0.941176f, 0.807843f},	// 1R 2C - Brass
		{0.393548f, 0.271906f, 0.166721f},	// 2R 2C - Bronze
		{0.774597f, 0.774597f, 0.774597f},	// 3R 2C - Chrome
		{0.256777f, 0.137622f, 0.086014f},	// 4R 2C - Copper
		{0.628281f, 0.555802f, 0.366065f},	// 5R 2C - Gold
		{0.508273f, 0.508273f, 0.508273f},	// 6R 2C - Silver
		{0.5f, 0.5f, 0.5f},			// 1R 3C - Black plastic
		{0.50196078f, 0.50196078f, 0.50196078f},// 2R 3C - Cyan plastic
		{0.45f, 0.55f, 0.45f},			// 3R 3C - Green plastic
		{0.7f, 0.6f, 0.6f},			// 4R 3C - Red plastic
		{0.7f, 0.7f, 0.7f},			// 5R 3C - White plastic
		{0.6f, 0.6f, 0.5f},			// 6R 3C - Yellow plastic
		{0.4f, 0.4f, 0.4f},			// 1R 4C - Black rubber
		{0.04f, 0.7f, 0.7f},			// 2R 4C - Cyan rubber
		{0.04f, 0.7f, 0.04f},			// 3R 4C - Green rubber
		{0.7f, 0.04f, 0.04f},			// 4R 4C - Red rubber
		{0.7f, 0.7f, 0.7f},			// 5R 4C - White rubber
		{0.7f, 0.7f, 0.04f}			// 6R 4C - Yellow rubber
	};
	GLfloat materialShininess[] =
	{	0.6f,		// 1R 1C - Emerald
		0.1f,		// 2R 1C - Jade
		0.3f,		// 3R 1C - Obsidian
		0.088f,		// 4R 1C - Pearl
		0.6f,		// 5R 1C - Ruby
		0.1f,		// 6R 1C - Turquoise
		0.21794872f,	// 1R 2C - Brass
		0.2f,		// 2R 2C - Bronze
		0.6f,		// 3R 2C - Chrome
		0.1f,		// 4R 2C - Copper
		0.4f,		// 5R 2C - Gold
		0.4f,		// 6R 2C - Silver
		0.25f,		// 1R 3C - Black plastic
		0.25f,		// 2R 3C - Cyan plastic
		0.25f,		// 3R 3C - Green plastic
		0.25f,		// 4R 3C - Red plastic
		0.25f,		// 5R 3C - White plastic
		0.25f,		// 6R 3C - Yellow plastic
		0.078125f,	// 1R 4C - Black rubber
		0.078125f,	// 2R 4C - Cyan rubber
		0.078125f,	// 3R 4C - Green rubber
		0.078125f,	// 4R 4C - Red rubber
		0.078125f,	// 5R 4C - White rubber
		0.078125f	// 6R 4C - Yellow rubber
	};
	const GLfloat radian = M_PI / 180.0f;
	const GLfloat radius = 100.0f;

	// Code
	[EAGLContext setCurrentContext: eaglContext];
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	
	// OpenGL code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(gSPObj);
	
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 6; j++) {
			glViewport((gWidth / 4) * i, (gHeight / 6) * (5-j), (GLsizei)(gWidth/4), (GLsizei)(gHeight/6));
			
			if(XRotationEnabled) {
				lightPosition[0] = 0.0f;
				lightPosition[1] = radius * (GLfloat)cos(rotAngle * radian);
				lightPosition[2] = radius * (GLfloat)sin(rotAngle * radian);
			}
			else if(YRotationEnabled) {
				lightPosition[0] = radius * (GLfloat)sin(rotAngle * radian);
				lightPosition[1] = 0.0f;
				lightPosition[2] = radius * (GLfloat)cos(rotAngle * radian);
			}
			else if(ZRotationEnabled) {
				lightPosition[0] = radius * (GLfloat)cos(rotAngle * radian);
				lightPosition[1] = radius * (GLfloat)sin(rotAngle * radian);
				lightPosition[2] = 0.0f;
			}
			else {
				lightPosition[0] = radius;
				lightPosition[1] = radius;
				lightPosition[2] = radius;
				lightPosition[3] = 1.0f;
			}
			
			if(lightEnabled) {
				glUniform1i(gLightEnabledUniform, true);
				glUniform3fv(gLAmbUniform, 1, lightAmbient);
				glUniform3fv(gLDiffUniform, 1, lightDiffuse);
				glUniform3fv(gLSpecUniform, 1, lightSpecular);
				glUniform4fv(gLPosUniform, 1, lightPosition);
				glUniform3fv(gKAmbUniform, 1, materialAmbient[(i*6)+j]);
				glUniform3fv(gKDiffUniform, 1, materialDiffuse[(i*6)+j]);
				glUniform3fv(gKSpecUniform, 1, materialSpecular[(i*6)+j]);
				GLfloat matShine = materialShininess[(i*6)+j] * 128.0f;
				glUniform1fv(gKShineUniform, 1, &matShine);
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
			glUniformMatrix4fv(gMMatrixUniform, 1, GL_FALSE, MMatrix);
			glUniformMatrix4fv(gVMatrixUniform, 1, GL_FALSE, VMatrix);
			glUniformMatrix4fv(gPMatrixUniform, 1, GL_FALSE, PMatrix);
			// OpenGL Drawing
			glBindVertexArray(gVAObj);
				glDrawArrays(GL_TRIANGLES, 0, pointCnt);
			glBindVertexArray(0);
		}
	}
	
	// End of OpenGL shading program
	glUseProgram(0);
	
	if(XRotationEnabled || YRotationEnabled || ZRotationEnabled)
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
	
	// Free Sphere
	[sphere deleteSphere];
	
	// Destroy Vertex Array Object
	if(gVAObj) {
		glDeleteVertexArrays(1, &gVAObj);
		gVAObj = 0;
	}
	
	// Destroy Vertex Buffer Object
	if(gVBObj_sphere) {
		glDeleteBuffers(2, gVBObj_sphere);
		gVBObj_sphere[0] = 0;
		gVBObj_sphere[1] = 0;
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
