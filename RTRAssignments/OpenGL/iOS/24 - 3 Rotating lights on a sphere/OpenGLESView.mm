//
//  OpenGLESView.m
//  3 Rotating lights on a sphere
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
	
	GLuint gVSObj_PV, gFSObj_PV, gSPObj_PV;
	GLuint gVSObj_PF, gFSObj_PF, gSPObj_PF;
	GLuint gVAObj;
	GLuint gVBObj_sphere[2];
	
	GLuint gMMatrixUniform[2], gVMatrixUniform[2], gPMatrixUniform[2];
	GLuint gLAmbUniform[2], gLDiffUniform[2], gLSpecUniform[2], gLPosUniform[2];
	GLuint gKAmbUniform[2], gKDiffUniform[2], gKSpecUniform[2], gKShineUniform[2];
	GLuint gLightEnabledUniform[2];
	
	int pointCnt;
	MySphere *sphere;
	GLfloat *sphereVertex, *sphereNormal;
	
	bool toggleLighting;
	bool lightEnabled;
	bool lightRotationEnabled;
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
		
		// Per Vertex Lighting
		// Vertex Shader
		gVSObj_PV = glCreateShader(GL_VERTEX_SHADER);		// Create shader
		const GLchar *VSSrcCode_PV = 				// Source code of shader
			"#version 300 es\n" \
			"in vec4 vPosition;" \
			"in vec3 vNormal;" \
			"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
			"uniform bool u_LightEnabled;" \
			"uniform vec3 u_LAmb[3], u_LDiff[3], u_LSpec[3];" \
			"uniform vec4 u_LPos[3];" \
			"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
			"uniform float u_KShine;" \
			"out vec3 out_light;"\
			"void main(void) {" \
				"out_light = vec3(0.0);" \
				"if(u_LightEnabled) {" \
					"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
					"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" \
					"vec3 viewVector = normalize(-eyeCoords.xyz);" \
					"vec3 lightSource, reflectionVector;" \
					"vec3 ambient, diffuse, specular;" \
					"for(int i = 0; i < 3; i++) {" \
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
		glShaderSource(gVSObj_PV, 1, (const GLchar**)&VSSrcCode_PV, NULL);
		glCompileShader(gVSObj_PV);				// Compile Shader
		[self shaderErrorCheck : gVSObj_PV option : "COMPILE"];
		printf("PVL : Vertex Shader Compiled successfully...\n");

		// Fragment Shader
		gFSObj_PV = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
		const GLchar *FSSrcCode_PV = 			// Source code of shader
			"#version 300 es\n" \
			"precision highp float;" \
			"in vec3 out_light;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
				"FragColor = vec4(out_light, 1.0);" \
			"}";
		glShaderSource(gFSObj_PV, 1, (const GLchar**)&FSSrcCode_PV, NULL);
		glCompileShader(gFSObj_PV);				// Compile Shader
		[self shaderErrorCheck : gFSObj_PV option : "COMPILE"];
		printf("PVL : Fragment Shader Compiled successfully...\n");

		// Shader program
		gSPObj_PV = glCreateProgram();		// Create final shader
		glAttachShader(gSPObj_PV, gVSObj_PV);		// Add Vertex shader code to final shader
		glAttachShader(gSPObj_PV, gFSObj_PV);		// Add Fragment shader code to final shader
		glBindAttribLocation(gSPObj_PV, DV_ATTRIB_POS, "vPosition");
		glBindAttribLocation(gSPObj_PV, DV_ATTRIB_NORM, "vNormal");
		glLinkProgram(gSPObj_PV);
		[self shaderErrorCheck : gSPObj_PV option : "LINK"];
		
		// Per Fragment Lighting
		// Vertex Shader
		gVSObj_PF = glCreateShader(GL_VERTEX_SHADER);
		const GLchar *VSSrcCode_PF =
			"#version 300 es\n" \
			"in vec4 vPosition;" \
			"in vec3 vNormal;" \
			"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" \
			"uniform vec4 u_LPos[3];" \
			"uniform bool u_LightEnabled;" \
			"out vec3 out_tNorm, out_lSrc[3], out_viewVec;"\
			"void main(void) {" \
				"if(u_LightEnabled) {" \
					"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
					"out_tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" \
					"for(int i = 0; i < 3; i++) {" \
						"out_lSrc[i] = vec3(u_LPos[i] - eyeCoords);" \
					"}" \
					"out_viewVec = -eyeCoords.xyz;" \
				"}" \
				"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
			"}";
		glShaderSource(gVSObj_PF, 1, (const GLchar**)&VSSrcCode_PF, NULL);
		glCompileShader(gVSObj_PF);
		[self shaderErrorCheck : gVSObj_PF option : "COMPILE"];
		printf("PFL : Vertex shader compiled successfully...\n");
		
		// Fragment Shader
		gFSObj_PF = glCreateShader(GL_FRAGMENT_SHADER);
		const GLchar *FSSrcCode_PF =
			"#version 300 es\n" \
			"precision highp float;" \
			"in vec3 out_tNorm, out_lSrc[3], out_viewVec;" \
			"uniform vec3 u_LAmb[3], u_LDiff[3], u_LSpec[3];" \
			"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" \
			"uniform float u_KShine;" \
			"uniform bool u_LightEnabled;" \
			"out vec4 FragColor;" \
			"void main(void) {" \
				"vec3 light = vec3(0.0);" \
				"if(u_LightEnabled) {" \
					"vec3 transformedNormal = normalize(out_tNorm);" \
					"vec3 viewVector = normalize(out_viewVec);" \
					"vec3 lightSource, reflectionVector;" \
					"vec3 ambient, diffuse, specular;" \
					"for(int i = 0; i < 3; i++) {" \
						"lightSource = normalize(out_lSrc[i]);" \
						"reflectionVector = reflect(-lightSource, transformedNormal);" \
						"ambient = u_LAmb[i] * u_KAmb;" \
						"diffuse = u_LDiff[i] * u_KDiff * max(dot(lightSource, transformedNormal), 0.0);" \
						"specular = u_LSpec[i] * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
						"light += (ambient + diffuse + specular);" \
					"}" \
				"}" \
			"FragColor = vec4(light, 1.0);" \
			"}";
		glShaderSource(gFSObj_PF, 1, (const GLchar**)&FSSrcCode_PF, NULL);
		glCompileShader(gFSObj_PF);
		[self shaderErrorCheck : gFSObj_PF option : "COMPILE"];
		printf("PFL : Fragment shader compiled successfully...\n");
		
		// Shader program
		gSPObj_PF = glCreateProgram();
		glAttachShader(gSPObj_PF, gVSObj_PF);
		glAttachShader(gSPObj_PF, gFSObj_PF);
		glBindAttribLocation(gSPObj_PF, DV_ATTRIB_POS, "vPosition");
		glBindAttribLocation(gSPObj_PF, DV_ATTRIB_NORM, "vNormal");
		glLinkProgram(gSPObj_PF);
		[self shaderErrorCheck : gSPObj_PF option : "LINK"];
		printf("Shader program linked successfully...\n");
		
		// get uniform location(s)
		gMMatrixUniform[0] = glGetUniformLocation(gSPObj_PV, "u_MMatrix");
		gVMatrixUniform[0] = glGetUniformLocation(gSPObj_PV, "u_VMatrix");
		gPMatrixUniform[0] = glGetUniformLocation(gSPObj_PV, "u_PMatrix");
		gLAmbUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LAmb");
		gLDiffUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LDiff");
		gLSpecUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LSpec");
		gLPosUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LPos");
		gKAmbUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KAmb");
		gKDiffUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KDiff");
		gKSpecUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KSpec");
		gKShineUniform[0] = glGetUniformLocation(gSPObj_PV, "u_KShine");
		gLightEnabledUniform[0] = glGetUniformLocation(gSPObj_PV, "u_LightEnabled");

		gMMatrixUniform[1] = glGetUniformLocation(gSPObj_PF, "u_MMatrix");
		gVMatrixUniform[1] = glGetUniformLocation(gSPObj_PF, "u_VMatrix");
		gPMatrixUniform[1] = glGetUniformLocation(gSPObj_PF, "u_PMatrix");
		gLAmbUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LAmb");
		gLDiffUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LDiff");
		gLSpecUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LSpec");
		gLPosUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LPos");
		gKAmbUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KAmb");
		gKDiffUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KDiff");
		gKSpecUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KSpec");
		gKShineUniform[1] = glGetUniformLocation(gSPObj_PF, "u_KShine");
		gLightEnabledUniform[1] = glGetUniformLocation(gSPObj_PF, "u_LightEnabled");
		
		// other global variable initialization
		sphere = [[MySphere alloc] init];
		pointCnt = [sphere GenSphere : 1.0f stack : 30 slices : 30];
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
		
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		toggleLighting = true;
		lightEnabled = false;
		lightRotationEnabled = false;
		
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
	lightRotationEnabled = !lightRotationEnabled;
}
-(void)onLongPress: (UILongPressGestureRecognizer *)gesture {
	// Code
	if(gesture.state == UIGestureRecognizerStateEnded)
		toggleLighting = !toggleLighting;
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
	vec3 lightAmbient, lightDiffuse, lightSpecular;
	vec4 lightPosition;
	vec3 materialAmbient, materialDiffuse, materialSpecular;
	GLfloat materialShininess;

	// Code
	[EAGLContext setCurrentContext: eaglContext];
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
	
	// OpenGL code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(toggleLighting)
		glUseProgram(gSPObj_PV);
	else
		glUseProgram(gSPObj_PF);
	int ch = toggleLighting ? 0 : 1;
	if(lightEnabled) {
		GLfloat rad = M_PI / 180.0f;
		GLfloat radius = 100.0f;
		GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f };
		GLfloat lightDiffuse[] = { 1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f };
	/*	GLfloat lightSpecular[] = { 1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f };
	*/	GLfloat lightSpecular[] = { 0.25f, 0.0f, 0.0f,
			0.0f, 0.25f, 0.0f,
			0.0f, 0.0f, 0.25f };
		GLfloat lightPosition[] = {
			0.0f, radius * (GLfloat)cos(rotAngle * rad), radius * (GLfloat)sin(rotAngle * rad), 1.0f,
			radius * (GLfloat)sin(2 * rotAngle * rad), 0.0f, radius * (GLfloat)cos(2 * rotAngle * rad), 1.0f,
			radius * (GLfloat)cos(3 * rotAngle * rad), radius * (GLfloat)sin(3 * rotAngle * rad), 0.0f, 1.0f };
		materialAmbient = vec3(0.0f, 0.0f, 0.0f);
		materialDiffuse = vec3(1.0f, 1.0f, 1.0f);
	//	materialSpecular = vec3(1.0f, 1.0f, 1.0f);
		materialSpecular = vec3(0.5f, 0.5f, 0.5f);
		materialShininess = 50.0f;
		
		glUniform1i(gLightEnabledUniform[ch], true);
		glUniform3fv(gLAmbUniform[ch], 3, lightAmbient);
		glUniform3fv(gLDiffUniform[ch], 3, lightDiffuse);
		glUniform3fv(gLSpecUniform[ch], 3, lightSpecular);
		glUniform4fv(gLPosUniform[ch], 3, lightPosition);
		glUniform3fv(gKAmbUniform[ch], 1, materialAmbient);
		glUniform3fv(gKDiffUniform[ch], 1, materialDiffuse);
		glUniform3fv(gKSpecUniform[ch], 1, materialSpecular);
		glUniform1fv(gKShineUniform[ch], 1, &materialShininess);
	}
	else
		glUniform1i(gLightEnabledUniform[ch], false);

	// Initialization
	MMatrix = mat4::identity();
	VMatrix = mat4::identity();
	PMatrix = gPerspProjMatrix;
	translationMatrix = mat4::identity();
	// Setting values
	translationMatrix = translate(0.0f, 0.0f, -3.0f);
	VMatrix *= translationMatrix;
	glUniformMatrix4fv(gMMatrixUniform[ch], 1, GL_FALSE, MMatrix);
	glUniformMatrix4fv(gVMatrixUniform[ch], 1, GL_FALSE, VMatrix);
	glUniformMatrix4fv(gPMatrixUniform[ch], 1, GL_FALSE, PMatrix);
	// OpenGL Drawing
	glBindVertexArray(gVAObj);
		glDrawArrays(GL_TRIANGLES, 0, pointCnt);
	glBindVertexArray(0);
	
	// End of OpenGL shading program
	glUseProgram(0);
	
	if(lightRotationEnabled)
		rotAngle += 0.25f;
	if(rotAngle >= 360.0f)
		rotAngle =0.0f;
	
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

	// Detach shaders - Per Fragment Lighting
	glDetachShader(gSPObj_PF, gVSObj_PF);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj_PF, gFSObj_PF);		// Detach fragment shader from final shader program
	
	// Delete shaders - Per Fragment Lighting
	if(gVSObj_PF != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj_PF);
		gVSObj_PF = 0;
	}
	if(gFSObj_PF != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj_PF);
		gFSObj_PF = 0;
	}
	if(gSPObj_PF != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj_PF);
		gSPObj_PF = 0;
	}
	
	// Detach shaders - Per Vertex Lighting
	glDetachShader(gSPObj_PV, gVSObj_PV);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj_PV, gFSObj_PV);		// Detach fragment shader from final shader program
	
	// Delete shaders - Per Vertex Lighting
	if(gVSObj_PV != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj_PV);
		gVSObj_PV = 0;
	}
	if(gFSObj_PV != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj_PV);
		gFSObj_PV = 0;
	}
	if(gSPObj_PV != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj_PV);
		gSPObj_PV = 0;
	}

	[super dealloc];
	[self Uninitialize];
}
@end
