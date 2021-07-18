package com.rtr3_android.threedanimation;

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

import android.opengl.Matrix;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
	private GestureDetector gestureDetector;

	private int VSObj;
	private int FSObj;
	private int SPObj;

	private int[] VAObj_shapes = new int[2];	// [0]-Pyramid, [1]-Cube
	private int[] VBObj_pyramid = new int[2];	// [0]-Position, [1]-Color
	private int[] VBObj_cube = new int[2];		// [0]-Position, [1]-Color
	private int MVPUniform;

	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private float gfAngle = 0.0f;

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
			"#version 300 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec4 vColor;"+
			"uniform mat4 u_mvp_matrix;"+
			"out vec4 out_color;"+
			"void main(void) {"+
				"gl_Position = u_mvp_matrix * vPosition;"+
				"out_color = vColor;"+
			"}"
		);
		GLES32.glShaderSource(VSObj, VSSrcCode);
		GLES32.glCompileShader(VSObj);
		ShaderErrorCheck(VSObj, GLES32.GL_VERTEX_SHADER);

		// Fragment shader
		FSObj = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		final String FSSrcCode = String.format(
			"#version 300 es"+
			"\n"+
			"precision highp float;"+
			"in vec4 out_color;"+
			"out vec4 FragColor;"+
			"void main(void) {"+
				"FragColor = out_color;"+
			"}"
		);
		GLES32.glShaderSource(FSObj, FSSrcCode);
		GLES32.glCompileShader(FSObj);
		ShaderErrorCheck(FSObj, GLES32.GL_FRAGMENT_SHADER);

		SPObj = GLES32.glCreateProgram();		// create shader program
		GLES32.glAttachShader(SPObj, VSObj);		// Attach vertex shader
		GLES32.glAttachShader(SPObj, FSObj);	// Attach fragment shader
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_VERTEX, "vPosition");
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_COLOR, "vColor");
		GLES32.glLinkProgram(SPObj);
		ShaderErrorCheck(SPObj, 0);

		// get uniforms
		MVPUniform = GLES32.glGetUniformLocation(SPObj, "u_mvp_matrix");

		// vertices, color, shader attribs, VAOs, VBOs initialization
		final float pyramidVertices[] = {
			// Front face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	-1.0f, -1.0f, 1.0f,	1.0f, -1.0f, 1.0f,

			// Right face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	1.0f, -1.0f, 1.0f,	1.0f, -1.0f, -1.0f,

			// Back face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,

			// Left face - Apex, Left bottom, Right bottom
			0.0f, 2.0f, 0.0f,	-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, 1.0f
		};
		final float pyramidColor[] = {
			// Front face - Apex, Left bottom, Right bottom
			1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,

			// Right face - Apex, Left bottom, Right bottom
			1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,

			// Back face - Apex, Left bottom, Right bottom
			1.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,

			// Left face - Apex, Left bottom, Right bottom
			1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f
		};
		final float cubeVertices[] = {
			// Front face - Left top, Left bottom, Right bottom, Right top
			-1.0f, 1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	1.0f, -1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// Right face - Left top, Left bottom, Right bottom, Right top
			1.0f, 1.0f, 1.0f,	1.0f, -1.0f, 1.0f,	1.0f, -1.0f, -1.0f,	1.0f, 1.0f, -1.0f,

			// Bottom face - Left top, Left bottom, Right bottom, Right top
			1.0f, -1.0f, 1.0f,	-1.0f, -1.0f, 1.0f,	-1.0f, -1.0f, -1.0f,	1.0f, -1.0f, -1.0f,

			// Left face - Left top, Left bottom, Right bottom, Right top
			-1.0f, 1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, 1.0f,	-1.0f, 1.0f, 1.0f,

			// Back face - Left top, Left bottom, Right bottom, Right top
			1.0f, 1.0f, -1.0f,	1.0f, -1.0f, -1.0f,	-1.0f, -1.0f, -1.0f,	-1.0f, 1.0f, -1.0f,

			// Top face - Left top, Left bottom, Right bottom, Right top
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, -1.0f,	-1.0f, 1.0f, -1.0f,	-1.0f, 1.0f, 1.0f
		};
		final float cubeColor[] = {
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

		GLES32.glGenVertexArrays(2, VAObj_shapes, 0);
		GLES32.glBindVertexArray(VAObj_shapes[0]);		// For Pyramid
			GLES32.glGenBuffers(2, VBObj_pyramid, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_pyramid[0]);		// Vertices of pyramid
			ByteBuffer byteBuffer_v = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer_v.asFloatBuffer();
			verticesBuffer.put(pyramidVertices);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_pyramid[1]);		// Color of pyramid
			ByteBuffer byteBuffer_c = ByteBuffer.allocateDirect(pyramidColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			FloatBuffer colorBuffer = byteBuffer_c.asFloatBuffer();
			colorBuffer.put(pyramidColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		GLES32.glBindVertexArray(VAObj_shapes[1]);		// For cube
			GLES32.glGenBuffers(2, VBObj_cube, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_cube[0]);		// Vertices of cube
			byteBuffer_v = ByteBuffer.allocateDirect(cubeVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer_v.asFloatBuffer();
			verticesBuffer.put(cubeVertices);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_cube[1]);		// Color of cube
			byteBuffer_c = ByteBuffer.allocateDirect(cubeColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			colorBuffer = byteBuffer_c.asFloatBuffer();
			colorBuffer.put(cubeColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
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

		System.out.println("Resize : Perspective matrix ");
		for(int i = 0; i < 16; i++)
			System.out.println(i + " : " + perspProjMatrix[i]);
		System.out.println("\n Resize method executed successfully...");
	}

	private void draw() {
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// Use shader program
		GLES32.glUseProgram(SPObj);

		// For pyramid
		float MV_matrix[] = new float[16];
		float MVP_matrix[] = new float[16];
		float translation_matrix[] = new float[16];
		float rotation_matrix[] = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, -2.0f, 0.0f, -5.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glBindVertexArray(VAObj_shapes[0]);
		for(int i = 0; i < 12; i += 3)
			GLES32.glDrawArrays(GLES32.GL_TRIANGLES, i, 3);
		GLES32.glBindVertexArray(0);

		// For cube
		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 2.0f, 0.0f, -5.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 0.0f, 1.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glBindVertexArray(VAObj_shapes[1]);
		for(int i = 0; i < 24; i += 4)
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, i, 4);
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);
	
		gfAngle += 0.5f;
		if(gfAngle >= 360.0f)
			gfAngle = 0.0f;

		// render or flush
		requestRender();
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj_shapes[0] != 0) {
			GLES32.glDeleteVertexArrays(2, VAObj_shapes, 0);
			VAObj_shapes[0] = 0;
			VAObj_shapes[1] = 0;
		}

		// Delete Vertex Buffer Object
		if(VBObj_pyramid[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_pyramid, 0);
			VBObj_pyramid[0] = 0;
			VBObj_pyramid[1] = 0;
		}
		if(VBObj_cube[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_cube, 0);
			VBObj_cube[0] = 0;
			VBObj_cube[1] = 0;
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