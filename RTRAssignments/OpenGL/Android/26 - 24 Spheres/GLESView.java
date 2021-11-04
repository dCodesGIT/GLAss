package com.rtr3_android.materialandlights;

import android.content.Context;		// For drawing context
import android.opengl.GLSurfaceView;	// for OpenGL Surface view
import javax.microedition.khronos.opengles.GL10;	// For OpenGLES 1.0 (required)
import javax.microedition.khronos.egl.EGLConfig;	// For EGLConfig (as needed)
import android.opengl.GLES32;		// For OpenGL-ES 3.2

import android.view.MotionEvent;	// For Motion event
import android.view.GestureDetector;	// For GestureDetector
import android.view.GestureDetector.OnGestureListener;		// For Gestures
import android.view.GestureDetector.OnDoubleTapListener;	// For Taps

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import java.lang.Math;

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
	private GestureDetector gestureDetector;

	private int VSObj;
	private int FSObj;
	private int SPObj;

	private int[] VAObj_shapes = new int[1];	// [0]-sphere
	private int[] VBObj_sphere = new int[3];	// [0]-Position, [1]-Normals(lighting), [2]-element

	private int MMatUniform, VMatUniform, PMatUniform;
	private int LAmbUniform, LDiffUniform, LSpecUniform, LPosUniform;
	private int KAmbUniform, KDiffUniform, KSpecUniform, KShineUniform;
	private int LEnabledUniform;
	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private boolean lightEnabled = false;
	private int rotDirection = 0;			// 0-No Rotation; 1-X Axis Rotation; 2-Y Axis Rotation; 3-Z Axis Rotation
	private float fAngle = 0.0f;

	private int numElements, numVertices;

	private int gWidth, gHeight;

	public GLESView(Context drawingContext) {
		super(drawingContext);

		context = drawingContext;

		// Set EGLContext to current supported version of OpenGLES
		setEGLContextClientVersion(3);

		// Set Renderer for drawing on GLSurfaceView
		setRenderer(this);

		// Render the view only when there is a change in the drawing data
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

		gestureDetector = new GestureDetector(context, this, null, false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		// OpenGL-ES version check
		String glesVersion = gl.glGetString(GL10.GL_VERSION);
		System.out.println("OpenGL-ES version : "+glesVersion);

		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("OpenGL Shading Language version : "+glslVersion);

		initialize(gl);
	}

	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height) {
		gWidth = width;
		gHeight = height;
		resize(width, height);
	}

	@Override
	public void onDrawFrame(GL10 unused) {
		draw();
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		int eventAction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return (true);
	}

	@Override
	public boolean onDoubleTap(MotionEvent event) {
		rotDirection++;
		if(rotDirection > 3)
			rotDirection = 1;
		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent event) {
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent event) {
		lightEnabled = !lightEnabled;
		return (true);
	}

	@Override
	public boolean onDown(MotionEvent event) {
		return (true);
	}

	@Override
	public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY) {
		return (true);
	}

	@Override
	public void onLongPress(MotionEvent event) {
	}

	@Override
	public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
		Uninitialize();
		System.exit(0);
		return (true);
	}

	@Override
	public void onShowPress(MotionEvent event) {
	}

	@Override
	public boolean onSingleTapUp(MotionEvent event) {
		return (true);
	}

	private void initialize(GL10 gl) {
		// Vertex shader
		VSObj = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		final String VSSrcCode = String.format(
			"#version 300 es" +
			"\n" +
			"in vec4 vPosition;" +
			"in vec3 vNormal;" +
			"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;" +
			"uniform vec4 u_LPos;" +
			"uniform bool u_LEnabled;" +
			"out vec3 tNorm, lSrc, viewVec;" +
			"void main(void) {" +
				"if(u_LEnabled) {" +
					"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" +
					"tNorm = mat3(u_VMatrix * u_MMatrix) * vNormal;" +
					"lSrc = vec3(u_LPos - eyeCoords);" +
					"viewVec = -eyeCoords.xyz;" +
				"}" +
					"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" +
			"}"
		);
		GLES32.glShaderSource(VSObj, VSSrcCode);
		GLES32.glCompileShader(VSObj);
		ShaderErrorCheck(VSObj, GLES32.GL_VERTEX_SHADER);

		// Fragment shader
		FSObj = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		final String FSSrcCode = String.format(
			"#version 300 es" +
			"\n" +
			"precision highp float;" +
			"in vec3 tNorm, lSrc, viewVec;" +
			"uniform vec3 u_LAmb, u_LDiff, u_LSpec;" +
			"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" +
			"uniform float u_KShine;" +
			"uniform bool u_LEnabled;" +
			"out vec4 FragColor;" +
			"void main(void) {" +
				"vec3 light;" +
				"if(u_LEnabled) {" +
					"vec3 transformedNormal = normalize(tNorm);" +
					"vec3 lightSource = normalize(lSrc);" +
					"vec3 viewVector = normalize(viewVec);" +
					"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" +
					"vec3 ambient = u_LAmb * u_KAmb;" +
					"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0f);" +
					"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" +
					"light = ambient + diffuse + specular;" +
				"}" +
				"else {" +
					"light = vec3(0.0, 0.0, 0.0);" +
				"}" +
				"FragColor = vec4(light, 1.0);" +
			"}"
		);
		GLES32.glShaderSource(FSObj, FSSrcCode);
		GLES32.glCompileShader(FSObj);
		ShaderErrorCheck(FSObj, GLES32.GL_FRAGMENT_SHADER);

		SPObj = GLES32.glCreateProgram();		// create shader program
		GLES32.glAttachShader(SPObj, VSObj);		// Attach vertex shader
		GLES32.glAttachShader(SPObj, FSObj);		// Attach fragment shader
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_VERTEX, "vPosition");
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_NORMAL, "vNormal");
		GLES32.glLinkProgram(SPObj);
		ShaderErrorCheck(SPObj, 0);

		// get uniforms
		MMatUniform = GLES32.glGetUniformLocation(SPObj, "u_MMatrix");
		VMatUniform = GLES32.glGetUniformLocation(SPObj, "u_VMatrix");
		PMatUniform = GLES32.glGetUniformLocation(SPObj, "u_PMatrix");
		LAmbUniform = GLES32.glGetUniformLocation(SPObj, "u_LAmb");
		LDiffUniform = GLES32.glGetUniformLocation(SPObj, "u_LDiff");
		LSpecUniform = GLES32.glGetUniformLocation(SPObj, "u_LSpec");
		LPosUniform = GLES32.glGetUniformLocation(SPObj, "u_LPos");
		KAmbUniform = GLES32.glGetUniformLocation(SPObj, "u_KAmb");
		KDiffUniform = GLES32.glGetUniformLocation(SPObj, "u_KDiff");
		KSpecUniform = GLES32.glGetUniformLocation(SPObj, "u_KSpec");
		KShineUniform = GLES32.glGetUniformLocation(SPObj, "u_KShine");
		LEnabledUniform = GLES32.glGetUniformLocation(SPObj, "u_LEnabled");

		// vertices, color, shader attribs, VAOs, VBOs initialization
		Sphere sphere=new Sphere();
		float sphereVertices[]=new float[1146];
		float sphereNormals[]=new float[1146];
		float sphereTextures[]=new float[764];
		short sphereElements[]=new short[2280];
		sphere.getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
		numVertices = sphere.getNumberOfSphereVertices();
		numElements = sphere.getNumberOfSphereElements();

		GLES32.glGenVertexArrays(1, VAObj_shapes, 0);
		GLES32.glBindVertexArray(VAObj_shapes[0]);		// For sphere
			GLES32.glGenBuffers(3, VBObj_sphere, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_sphere[0]);		// Vertices of sphere
			ByteBuffer byteBuffer_v = ByteBuffer.allocateDirect(sphereVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer_v.asFloatBuffer();
			verticesBuffer.put(sphereVertices);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_sphere[1]);		// Normals of sphere
			ByteBuffer byteBuffer_n = ByteBuffer.allocateDirect(sphereNormals.length * 4);
			byteBuffer_n.order(ByteOrder.nativeOrder());
			FloatBuffer normalBuffer = byteBuffer_n.asFloatBuffer();
			normalBuffer.put(sphereNormals);
			normalBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereNormals.length * 4, normalBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_NORMAL);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_sphere[2]);		// Elements of sphere
			ByteBuffer byteBuffer_e = ByteBuffer.allocateDirect(sphereElements.length * 4);
			byteBuffer_e.order(ByteOrder.nativeOrder());
			ShortBuffer elementsBuffer = byteBuffer_e.asShortBuffer();
			elementsBuffer.put(sphereElements);
			elementsBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphereElements.length * 2, elementsBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		// Enable depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Enable culling
	//	GLES32.glEnable(GLES32.GL_CULL_FACE);

		// Set background frame color
		GLES32.glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		// set projection matrix to identity
		Matrix.setIdentityM(perspProjMatrix, 0);
	}

	private boolean ShaderErrorCheck(int shaderObject, int type) {
		int[] iStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog = null;

		if(type == GLES32.GL_VERTEX_SHADER || type == GLES32.GL_FRAGMENT_SHADER)
			GLES32.glGetShaderiv(shaderObject, GLES32.GL_COMPILE_STATUS, iStatus, 0);
		else if(type == 0)
			GLES32.glGetProgramiv(shaderObject, GLES32.GL_LINK_STATUS, iStatus, 0);
		else {
			System.out.println("Invalid 2nd parameter in ShaderErrorCheck() ...");
			return (false);
		}
		if(iStatus[0] == GLES32.GL_FALSE) {
			if(type == 0)
				GLES32.glGetProgramiv(shaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			else
				GLES32.glGetShaderiv(shaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0) {
				if(type == 0)
					szInfoLog = GLES32.glGetProgramInfoLog(shaderObject);
				else
					szInfoLog = GLES32.glGetShaderInfoLog(shaderObject);
				System.out.println("Error log : "+szInfoLog);
				Uninitialize();
				System.exit(0);
			}
		}
		return (true);
	}

	private void resize(int width, int height) {
		// Adjust viewport based on geometry changes such as screen rotation
//		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspProjMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void draw() {
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// For sphere
		float M_matrix[] = new float[16];
		float V_matrix[] = new float[16];
		float P_matrix[];
		float lightPosition[] = new float[4];
		float materialAmbient[] = new float[3];
		float materialDiffuse[] = new float[3];
		float materialSpecular[] = new float[3];
		float materialShininess[] = new float[1];
		float radius = 10.0f;

		Matrix.setIdentityM(M_matrix, 0);
		Matrix.setIdentityM(V_matrix, 0);
		Matrix.translateM(M_matrix, 0, 0.0f, 0.0f, -2.5f);
		P_matrix = perspProjMatrix;
		
		// Use shader program
		GLES32.glUseProgram(SPObj);
				
		for(int i = 0; i < 4; i++) {
			for(int j = 0; j < 6; j++) {
				GLES32.glViewport((gWidth / 6) * (i + 1), (gHeight / 6) * (5 - j), gWidth / 6, gHeight / 6);
				
				if(rotDirection == 1) {
					lightPosition[0] = 0.0f; 
					lightPosition[1] = radius * (float)Math.cos(fAngle);
					lightPosition[2] = radius * (float)Math.sin(fAngle);
				}
				else if(rotDirection == 2) {
					lightPosition[0] = radius * (float)Math.sin(fAngle);
					lightPosition[1] = 0.0f;
					lightPosition[2] = radius * (float)Math.cos(fAngle);
				}
				else if(rotDirection == 3) {
					lightPosition[0] = radius * (float)Math.cos(fAngle);
					lightPosition[1] = radius * (float)Math.sin(fAngle);
					lightPosition[2] = 0.0f;
				}
				else {
					lightPosition[0] = radius;
					lightPosition[1] = radius;
					lightPosition[2] = radius;
					lightPosition[3] = 1.0f;
				}
				getMaterial((i*6)+j, materialAmbient, materialDiffuse, materialSpecular, materialShininess);

				if(lightEnabled) {
					GLES32.glUniform1i(LEnabledUniform, 1);
					GLES32.glUniform3f(LAmbUniform, 0.0f, 0.0f, 0.0f);
					GLES32.glUniform3f(LDiffUniform, 1.0f, 1.0f, 1.0f);
					GLES32.glUniform3f(LSpecUniform, 1.0f, 1.0f, 1.0f);
					GLES32.glUniform4fv(LPosUniform, 1, lightPosition, 0);
					GLES32.glUniform3fv(KAmbUniform, 1, materialAmbient, 0);
					GLES32.glUniform3fv(KDiffUniform, 1, materialDiffuse, 0);
					GLES32.glUniform3fv(KSpecUniform, 1, materialSpecular, 0);
					GLES32.glUniform1fv(KShineUniform, 1, materialShininess, 0);
				}
				else
					GLES32.glUniform1i(LEnabledUniform, 0);

				GLES32.glUniformMatrix4fv(MMatUniform, 1, false, M_matrix, 0);
				GLES32.glUniformMatrix4fv(VMatUniform, 1, false, V_matrix, 0);
				GLES32.glUniformMatrix4fv(PMatUniform, 1, false, P_matrix, 0);

				GLES32.glBindVertexArray(VAObj_shapes[0]);
					GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, VBObj_sphere[2]);
					GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
				GLES32.glBindVertexArray(0);
			}
		}
		GLES32.glUseProgram(0);

		if(lightEnabled)
			fAngle += 0.025f;
		if(fAngle >= 360.0f)
			fAngle = 0.0f;

		// render or flush
		requestRender();
	}

	private void getMaterial(int n, float[] ambient, float[] diffuse, float[] specular, float[] shininess) {
		final float materialAmb[] = {
			0.0215f, 0.1745f, 0.0215f, 1.0f,	// 1R 1C - Emerald
			0.135f, 0.2225f, 0.1575f, 1.0f,		// 2R 1C - Jade
			0.05375f, 0.05f, 0.06625f, 1.0f,	// 3R 1C - Obsidian
			0.25f, 0.20725f, 0.20725f, 1.0f,	// 4R 1C - Pearl
			0.1745f, 0.01175f, 0.01175f, 1.0f,	// 5R 1C - Ruby
			0.1f, 0.18725f, 0.1745f, 1.0f,		// 6R 1C - Turquoise
			0.329412f, 0.223529f, 0.027451f, 1.0f,	// 1R 2C - Brass
			0.2125f, 0.1275f, 0.054f, 1.0f,		// 2R 2C - Bronze
			0.25f, 0.25f, 0.25f, 1.0f,		// 3R 2C - Chrome
			0.19125f, 0.0735f, 0.0225f, 1.0f,	// 4R 2C - Copper
			0.24725f, 0.1995f, 0.0745f, 1.0f,	// 5R 2C - Gold
			0.19225f, 0.19225f, 0.19225f, 1.0f,	// 6R 2C - Silver
			0.0f, 0.0f, 0.0f, 1.0f,			// 1R 3C - Black plastic
			0.0f, 0.1f, 0.06f, 1.0f,		// 2R 3C - Cyan plastic
			0.0f, 0.0f, 0.0f, 1.0f,			// 3R 3C - Green plastic
			0.0f, 0.0f, 0.0f, 1.0f,			// 4R 3C - Red plastic
			0.0f, 0.0f, 0.0f, 1.0f,			// 5R 3C - White plastic
			0.0f, 0.0f, 0.0f, 1.0f,			// 6R 3C - Yellow plastic
			0.02f, 0.02f, 0.02f, 1.0f,		// 1R 4C - Black rubber
			0.0f, 0.05f, 0.05f, 1.0f,		// 2R 4C - Cyan rubber
			0.0f, 0.05f, 0.0f, 1.0f,		// 3R 4C - Green rubber
			0.05f, 0.0f, 0.0f, 1.0f,		// 4R 4C - Red rubber
			0.05f, 0.05f, 0.05f, 1.0f,		// 5R 4C - White rubber
			0.05f, 0.05f, 0.04f, 1.0f		// 6R 4C - Yellow rubber
		};
		final float materialDiff[] = {
			0.07568f, 0.61424f, 0.07568f, 1.0f,	// 1R 1C - Emerald
			0.54f, 0.89f, 0.63f, 1.0f,		// 2R 1C - Jade
			0.18275f, 0.17f, 0.22525f, 1.0f,	// 3R 1C - Obsidian
			1.0f, 0.829f, 0.829f, 1.0f,		// 4R 1C - Pearl
			0.61424f, 0.04136f, 0.04163f, 1.0f,	// 5R 1C - Ruby
			0.396f, 0.74151f, 0.69102f, 1.0f,	// 6R 1C - Turquoise
			0.780392f, 0.568627f, 0.113725f, 1.0f,	// 1R 2C - Brass
			0.714f, 0.4284f, 0.18144f, 1.0f,	// 2R 2C - Bronze
			0.4f, 0.4f, 0.4f, 1.0f,			// 3R 2C - Chrome
			0.7038f, 0.27048f, 0.0828f, 1.0f,	// 4R 2C - Copper
			0.75164f, 0.60648f, 0.22648f, 1.0f,	// 5R 2C - Gold
			0.50754f, 0.50754f, 0.50754f, 1.0f,	// 6R 2C - Silver
			0.01f, 0.01f, 0.01f, 1.0f,		// 1R 3C - Black plastic
			0.0f, 0.50980392f, 0.50980392f, 1.0f,	// 2R 3C - Cyan plastic
			0.1f, 0.35f, 0.1f, 1.0f,		// 3R 3C - Green plastic
			0.5f, 0.0f, 0.0f, 1.0f,			// 4R 3C - Red plastic
			0.55f, 0.55f, 0.55f, 1.0f,		// 5R 3C - White plastic
			0.5f, 0.5f, 0.0f, 1.0f,			// 6R 3C - Yellow plastic
			0.01f, 0.01f, 0.01f, 1.0f,		// 1R 4C - Black rubber
			0.4f, 0.5f, 0.5f, 1.0f,			// 2R 4C - Cyan rubber
			0.4f, 0.5f, 0.4f, 1.0f,			// 3R 4C - Green rubber
			0.5f, 0.4f, 0.4f, 1.0f,			// 4R 4C - Red rubber
			0.5f, 0.5f, 0.5f, 1.0f,			// 5R 4C - White rubber
			0.5f, 0.5f, 0.4f, 1.0f			// 6R 4C - Yellow rubber
		};
		final float materialSpec[] = {
			0.633f, 0.727811f, 0.33f, 1.0f,			// 1R 1C - Emerald
			0.316228f, 0.316228f, 0.316228f, 1.0f,		// 2R 1C - Jade
			0.332741f, 0.328634f, 0.346435f, 1.0f,		// 3R 1C - Obsidian
			0.296648f, 0.296648f, 0.296648f, 1.0f,		// 4R 1C - Pearl
			0.727811f, 0.626959f, 0.626959f, 1.0f,		// 5R 1C - Ruby
			0.297254f, 0.308290f, 0.306678f, 1.0f,		// 6R 1C - Turquoise
			0.992157f, 0.941176f, 0.807843f, 1.0f,		// 1R 2C - Brass
			0.393548f, 0.271906f, 0.166721f, 1.0f,		// 2R 2C - Bronze
			0.774597f, 0.774597f, 0.774597f, 1.0f,		// 3R 2C - Chrome
			0.256777f, 0.137622f, 0.086014f, 1.0f,		// 4R 2C - Copper
			0.628281f, 0.555802f, 0.366065f, 1.0f,		// 5R 2C - Gold
			0.508273f, 0.508273f, 0.508273f, 1.0f,		// 6R 2C - Silver
			0.5f, 0.5f, 0.5f, 1.0f,				// 1R 3C - Black plastic
			0.50196078f, 0.50196078f, 0.50196078f, 1.0f,	// 2R 3C - Cyan plastic
			0.45f, 0.55f, 0.45f, 1.0f,			// 3R 3C - Green plastic
			0.7f, 0.6f, 0.6f, 1.0f,				// 4R 3C - Red plastic
			0.7f, 0.7f, 0.7f, 1.0f,				// 5R 3C - White plastic
			0.6f, 0.6f, 0.5f, 1.0f,				// 6R 3C - Yellow plastic
			0.4f, 0.4f, 0.4f, 1.0f,				// 1R 4C - Black rubber
			0.04f, 0.7f, 0.7f, 1.0f,			// 2R 4C - Cyan rubber
			0.04f, 0.7f, 0.04f, 1.0f,			// 3R 4C - Green rubber
			0.7f, 0.04f, 0.04f, 1.0f,			// 4R 4C - Red rubber
			0.7f, 0.7f, 0.7f, 1.0f,			// 5R 4C - White rubber
			0.7f, 0.7f, 0.04f, 1.0f			// 6R 4C - Yellow rubber
		};
		final float materialShine[] = {
			0.6f,		// 1R 1C - Emerald
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
		ambient[0] = materialAmb[(n*4)+0];
		ambient[1] = materialAmb[(n*4)+1];
		ambient[2] = materialAmb[(n*4)+2];
		diffuse[0] = materialDiff[(n*4)+0];
		diffuse[1] = materialDiff[(n*4)+1];
		diffuse[2] = materialDiff[(n*4)+2];
		specular[0] = materialSpec[(n*4)+0];
		specular[1] = materialSpec[(n*4)+1];
		specular[2] = materialSpec[(n*4)+2];
		shininess[0] = materialShine[n] * 128.0f;
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj_shapes[0] != 0) {
			GLES32.glDeleteVertexArrays(1, VAObj_shapes, 0);
			VAObj_shapes[0] = 0;
		}

		// Delete Vertex Buffer Object
		if(VBObj_sphere[0] != 0) {
			GLES32.glDeleteBuffers(3, VBObj_sphere, 0);
			VBObj_sphere[0] = 0;
			VBObj_sphere[1] = 0;
			VBObj_sphere[2] = 0;
		}

		if(SPObj != 0) {
			if(VSObj != 0) {
				GLES32.glDetachShader(SPObj, VSObj);
				GLES32.glDeleteShader(VSObj);
				VSObj = 0;
			}
			if(FSObj != 0) {
				GLES32.glDetachShader(SPObj, FSObj);
				GLES32.glDeleteShader(FSObj);
				FSObj = 0;
			}
			GLES32.glDeleteProgram(SPObj);
			SPObj = 0;
		}
	}
}
