package com.rtr3_android.pfl;

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

	private boolean AlbedoEnabled = false;
	private boolean lightEnabled = false;

	private int numElements, numVertices;

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
		AlbedoEnabled = !AlbedoEnabled;
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
					"light = ambient +diffuse + specular;" +
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
		GLES32.glAttachShader(SPObj, FSObj);	// Attach fragment shader
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
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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
		GLES32.glViewport(0, 0, width, height);

		Matrix.perspectiveM(perspProjMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
	}

	private void draw() {
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// Use shader program
		GLES32.glUseProgram(SPObj);

		// For sphere
		float M_matrix[] = new float[16];
		float V_matrix[] = new float[16];
		float P_matrix[] = new float[16];
		float translation_matrix[] = new float[16];
		float lightAmbient[], lightDiffuse[], lightSpecular[], lightPosition[];
		float materialAmbient[], materialDiffuse[], materialSpecular[], materialShininess[];

		Matrix.setIdentityM(M_matrix, 0);
		Matrix.setIdentityM(V_matrix, 0);
		Matrix.setIdentityM(P_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);

		if(AlbedoEnabled) {
			lightAmbient = new float[] { 0.1f, 0.1f, 0.1f };
			lightDiffuse = new float[] { 1.0f, 1.0f, 1.0f };
			lightSpecular = new float[] { 1.0f, 1.0f, 1.0f };
			lightPosition = new float[] { 100.0f, 100.0f, 100.0f, 1.0f };
			materialAmbient = new float[] { 0.0f, 0.0f, 0.0f };
			materialDiffuse = new float[] { 0.5f, 0.2f, 0.7f };
			materialSpecular = new float[] { 0.7f, 0.7f, 0.7f };
			materialShininess = new float[] { 128.0f };
		}
		else {
			lightAmbient = new float[] { 0.0f, 0.0f, 0.0f };
			lightDiffuse = new float[] { 1.0f, 1.0f, 1.0f };
			lightSpecular = new float[] { 1.0f, 1.0f, 1.0f };
			lightPosition = new float[] { 100.0f, 100.0f, 100.0f, 1.0f };
			materialAmbient = new float[] { 0.0f, 0.0f, 0.0f };
			materialDiffuse = new float[] { 1.0f, 1.0f, 1.0f };
			materialSpecular = new float[] { 1.0f, 1.0f, 1.0f };
			materialShininess = new float[] { 50.0f };
		}
		if(lightEnabled) {
			GLES32.glUniform1i(LEnabledUniform, 1);
			GLES32.glUniform3fv(LAmbUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(LDiffUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(LSpecUniform, 1, lightSpecular, 0);
			GLES32.glUniform4fv(LPosUniform, 1, lightPosition, 0);
			GLES32.glUniform3fv(KAmbUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(KDiffUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(KSpecUniform, 1, materialSpecular, 0);
			GLES32.glUniform1fv(KShineUniform, 1, materialShininess, 0);
		}
		else
			GLES32.glUniform1i(LEnabledUniform, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -1.5f);
		Matrix.multiplyMM(M_matrix, 0, M_matrix, 0, translation_matrix, 0);
		P_matrix = perspProjMatrix;

		GLES32.glUniformMatrix4fv(MMatUniform, 1, false, M_matrix, 0);
		GLES32.glUniformMatrix4fv(VMatUniform, 1, false, V_matrix, 0);
		GLES32.glUniformMatrix4fv(PMatUniform, 1, false, P_matrix, 0);

		GLES32.glBindVertexArray(VAObj_shapes[0]);
			GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, VBObj_sphere[2]);
			GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		// render or flush
		requestRender();
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
