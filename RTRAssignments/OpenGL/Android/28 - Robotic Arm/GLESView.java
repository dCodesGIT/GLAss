package com.rtr3_android.roboticarm;

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

	private int[] VAObj = new int[1];		// [0]-sphere
	private int[] VBObj_sphere = new int[3];	// [0]-Position, [1]-Normals(lighting), [2]-element

	private int MVPMatUniform;
	private int ColorUniform;
	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private int numElements, numVertices, numNormals;

	Stack s;
	private int elbow, shoulder;

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
		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent event) {
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent event) {
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
			"uniform mat4 u_MVPMatrix;" +
			"uniform vec3 u_Color;" +
			"out vec3 out_color;" +
			"void main(void) {" +
				"gl_Position = u_MVPMatrix * vPosition;" +
				"out_color = u_Color;" +
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
			"in vec3 out_color;" +
			"out vec4 FragColor;" +
			"void main(void) {" +
				"FragColor = vec4(out_color, 1.0);" +
			"}"
		);
		GLES32.glShaderSource(FSObj, FSSrcCode);
		GLES32.glCompileShader(FSObj);
		ShaderErrorCheck(FSObj, GLES32.GL_FRAGMENT_SHADER);

		SPObj = GLES32.glCreateProgram();		// create shader program
		GLES32.glAttachShader(SPObj, VSObj);		// Attach vertex shader
		GLES32.glAttachShader(SPObj, FSObj);	// Attach fragment shader
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_VERTEX, "vPosition");
		GLES32.glLinkProgram(SPObj);
		ShaderErrorCheck(SPObj, 0);

		// get uniforms
		MVPMatUniform = GLES32.glGetUniformLocation(SPObj, "u_MVPMatrix");
		ColorUniform = GLES32.glGetUniformLocation(SPObj, "u_Color");

		// vertices, color, shader attribs, VAOs, VBOs initialization
		Sphere sphere=new Sphere();
		float sphereVertices[]=new float[1146];
		float sphereNormals[]=new float[1146];
		float sphereTextures[]=new float[764];
		short sphereElements[]=new short[2280];
		sphere.getSphereVertexData(sphereVertices, sphereNormals, sphereTextures, sphereElements);
		numVertices = sphere.getNumberOfSphereVertices();
		numElements = sphere.getNumberOfSphereElements();

		GLES32.glGenVertexArrays(1, VAObj, 0);
		GLES32.glBindVertexArray(VAObj[0]);		// For sphere
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

		// Stack for Push Pop
		s = new Stack(5);
		elbow = 0;
		shoulder = 0;

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
		float MV_matrix[] = new float[16];
		float MVP_matrix[] = new float[16];
		float translation_matrix[] = new float[16];
		float rotation_matrix[] = new float[16];
		float scaling_matrix[] = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);
		Matrix.setIdentityM(scaling_matrix, 0);

		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -10.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, rotation_matrix, 0, shoulder, 0.0f, 0.0f, 1.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		s.PushMatrix(MV_matrix);
			Matrix.setIdentityM(scaling_matrix, 0);
			Matrix.scaleM(scaling_matrix, 0, scaling_matrix, 0, 2.0f, 0.65f, 1.0f);
			Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, scaling_matrix, 0);
			Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
			GLES32.glUniformMatrix4fv(MVPMatUniform, 1, false, MVP_matrix, 0);
			GLES32.glUniform3f(ColorUniform, 0.5f, 0.35f, 0.05f);
			GLES32.glBindVertexArray(VAObj[0]);
				GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, VBObj_sphere[2]);
				GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
			GLES32.glBindVertexArray(0);
		MV_matrix = s.PopMatrix();
		s.PushMatrix(MV_matrix);
			Matrix.setIdentityM(translation_matrix, 0);
			Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
			Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
			Matrix.setIdentityM(rotation_matrix, 0);
			Matrix.rotateM(rotation_matrix, 0, rotation_matrix, 0, elbow, 0.0f, 0.0f, 1.0f);
			Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
			Matrix.setIdentityM(translation_matrix, 0);
			Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
			Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
			s.PushMatrix(MV_matrix);
				Matrix.setIdentityM(scaling_matrix, 0);
				Matrix.scaleM(scaling_matrix, 0, scaling_matrix, 0, 2.0f, 0.5f, 1.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, scaling_matrix, 0);
				Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
				GLES32.glUniformMatrix4fv(MVPMatUniform, 1, false, MVP_matrix, 0);
				GLES32.glUniform3f(ColorUniform, 0.5f, 0.35f, 0.05f);
				GLES32.glBindVertexArray(VAObj[0]);
					GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, VBObj_sphere[2]);
					GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
				GLES32.glBindVertexArray(0);
			MV_matrix = s.PopMatrix();
		MV_matrix = s.PopMatrix();
		GLES32.glUseProgram(0);

		elbow += 6;
		if(elbow > 360) {
			shoulder++;
			elbow = 0;
		}
		shoulder++;
		if(shoulder > 360)
			shoulder = 0;

		// render or flush
		requestRender();
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj[0] != 0) {
			GLES32.glDeleteVertexArrays(1, VAObj, 0);
			VAObj[0] = 0;
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
