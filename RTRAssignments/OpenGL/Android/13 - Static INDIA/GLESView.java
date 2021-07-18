package com.rtr3_android.staticindia;

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

	private int[] VAObj_INDIA = new int[4];		// [0]-I, [1]-N, [2]-D, [3]-A
	private int[] VBObj_alpha_I = new int[2];	// [0]-Position, [1]-Color
	private int[] VBObj_alpha_N = new int[2];	// [0]-Position, [1]-Color
	private int[] VBObj_alpha_D = new int[2];	// [0]-Position, [1]-Color
	private int[] VBObj_alpha_A = new int[2];	// [0]-Position, [1]-Color
	private int MVPUniform;

	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

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
		final float I_alphabetVertices[] = {
			// Top quad of I
			-0.35f, 0.5f,	 0.35f, 0.5f,	 0.35f, 0.4f,	-0.35f, 0.4f,

			// Vertical quad (Upper half) of I
			-0.05f, 0.4f,	 0.05f, 0.4f,	 0.05f, 0.0f,	-0.05f, 0.0f,

			// Vertical quad (Lower half) of I
			 0.05f, 0.0f,	-0.05f, 0.0f,	-0.05f, -0.4f,	 0.05f, -0.4f,

			// Bottom quad of I
			-0.35f, -0.5f,	 0.35f, -0.5f,	 0.35f, -0.4f,	-0.35f, -0.4f
		};
		final float I_alphabetColor[] = {
			// Top quad of I - Saffron
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,

			// Vertical quad (Upper half) of I - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// Vertical quad (Lower half) of I - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// Bottom quad of I - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};
		final float N_alphabetVertices[] = {
			// 1st Vertical quad (Upper half)
			-0.35f, 0.5f,	-0.25f, 0.5f,	-0.25f, 0.0f,	-0.35f, 0.0f,

			// 1st Vertical quad (Lower half)
			-0.35f, 0.0f,	-0.25f, 0.0f,	-0.25f, -0.5f,	-0.35f, -0.5f,

			// Slant quad (Upper half)
			-0.35f, 0.5f,	-0.25f, 0.5f,	0.05f, 0.0f,	-0.05f, 0.0f,

			// Slant quad (lower half)
			0.05f, 0.0f,	-0.05f, 0.0f,	0.25f, -0.5f,	0.35f, -0.5f,

			// 2nd Vertical quad (upper half)
			0.35f, 0.5f,	0.25f, 0.5f,	0.25f, 0.0f,	0.35f, 0.0f,

			// 2nd Vertical quad (lower half)
			0.35f, 0.0f,	0.25f, 0.0f,	0.25f, -0.5f,	0.35f, -0.5f
		};
		final float N_alphabetColor[] = {
			// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 1st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// Slant quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// Slant quad (lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// 2nd Vertical quad (upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 2nd Vertical quad (lower half) - White, White, Green Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};
		final float D_alphabetVertices[] = {
			// 1st Vertical quad (Upper half)
			-0.25f, 0.4f,	-0.15f, 0.4f,	-0.15f, 0.0f,	-0.25f, 0.0f,

			// 1st Vertical quad (Lower half)
			-0.25f, 0.0f,	-0.15f, 0.0f,	-0.15f, -0.4f,	-0.25f, -0.4f,

			// Upper horizontal quad
			-0.35f, 0.5f,	0.35f, 0.5f,	0.35f, 0.4f,	-0.35f, 0.4f,

			// 2nd Vertical quad (Upper half)
			0.35f, 0.4f,	0.25f, 0.4f,	0.25f, 0.0f,	0.35f, 0.0f,

			// 2nd Vertical quad (Lower half)
			0.35f, 0.0f,	0.25f, 0.0f,	0.25f, -0.4f,	0.35f, -0.4f,

			// Lower horizontal quad
			-0.35f, -0.5f,	0.35f, -0.5f,	0.35f, -0.4f,	-0.35f, -0.4f
		};
		final float D_alphabetColor[] = {
			// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 1st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// Upper horizontal quad - Saffron
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,

			// 2nd Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 2nd Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// Lower horizontal quad - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};
		final float A_alphabetVertices[] = {
			// Central Horizontal
			0.25f, 0.05f,	0.25f, -0.05f,	-0.25f, -0.05f,	-0.25f, 0.05f,

			// 1st Vertical quad (Upper half)
			-0.35f, 0.4f,	-0.25f, 0.4f,	-0.25f, 0.0f,	-0.35f, 0.0f,

			// 1st Vertical quad (Lower half)
			-0.35f, 0.0f,	-0.25f, 0.0f,	-0.25f, -0.4f,	-0.35f, -0.4f,

			// Upper Horizontal quad
			0.35f, 0.5f,	0.35f, 0.4f,	-0.35f, 0.4f,	-0.35f, 0.5f,

			// 2nd Vertical quad (Upper half)
			0.35f, 0.4f,	0.25f, 0.4f,	0.25f, 0.0f,	0.35f, 0.0f,

			// 2nd Vertical quad (Lower half)
			0.35f, 0.0f,	0.25f, 0.0f,	0.25f, -0.4f,	0.35f, -0.4f,

			// 1st Vertical quad (Lowest part square)
			-0.25f, -0.4f,	-0.35f, -0.4f,	-0.35f, -0.5f,	-0.25f, -0.5f,

			// 2nd Vertical quad (Lowest part square)
			0.25f, -0.4f,	0.35f, -0.4f,	0.35f, -0.5f,	0.25f, -0.5f
		};
		final float A_alphabetColor[] = {
			// Central Horizontal - White
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 1st Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// Upper Horizontal quad - Saffron
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,

			// 2nd Vertical quad (Upper half) - Saffron, Saffron, White, White
			1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

			// 2nd Vertical quad (Lower half) - White, White, Green, Green
			1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// 1st Vertical quad (Lowest part square) - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

			// 2nd Vertical quad (Lowest part square) - Green
			0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
		};

		GLES32.glGenVertexArrays(4, VAObj_INDIA, 0);
		GLES32.glBindVertexArray(VAObj_INDIA[0]);		// For Alphabet I
			GLES32.glGenBuffers(2, VBObj_alpha_I, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_I[0]);		// Vertices of I
			ByteBuffer byteBuffer_v = ByteBuffer.allocateDirect(I_alphabetVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			FloatBuffer VerticesBuffer = byteBuffer_v.asFloatBuffer();
			VerticesBuffer.put(I_alphabetVertices);
			VerticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, I_alphabetVertices.length * 4, VerticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_I[1]);		// Color of I
			ByteBuffer byteBuffer_c = ByteBuffer.allocateDirect(I_alphabetColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			FloatBuffer ColorBuffer = byteBuffer_c.asFloatBuffer();
			ColorBuffer.put(I_alphabetColor);
			ColorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, I_alphabetColor.length * 4, ColorBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
		
		GLES32.glBindVertexArray(VAObj_INDIA[1]);		// For Alphabet N
			GLES32.glGenBuffers(2, VBObj_alpha_N, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_N[0]);		// Vertices of N
			byteBuffer_v = ByteBuffer.allocateDirect(N_alphabetVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			VerticesBuffer = byteBuffer_v.asFloatBuffer();
			VerticesBuffer.put(N_alphabetVertices);
			VerticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, N_alphabetVertices.length * 4, VerticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_N[1]);		// Color of N
			byteBuffer_c = ByteBuffer.allocateDirect(N_alphabetColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			ColorBuffer = byteBuffer_c.asFloatBuffer();
			ColorBuffer.put(N_alphabetColor);
			ColorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, N_alphabetColor.length * 4, ColorBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		GLES32.glBindVertexArray(VAObj_INDIA[2]);		// For Alphabet D
			GLES32.glGenBuffers(2, VBObj_alpha_D, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_D[0]);		// Vertices of D
			byteBuffer_v = ByteBuffer.allocateDirect(D_alphabetVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			VerticesBuffer = byteBuffer_v.asFloatBuffer();
			VerticesBuffer.put(D_alphabetVertices);
			VerticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, D_alphabetVertices.length * 4, VerticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_D[1]);		// Color of D
			byteBuffer_c = ByteBuffer.allocateDirect(D_alphabetColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			ColorBuffer = byteBuffer_c.asFloatBuffer();
			ColorBuffer.put(D_alphabetColor);
			ColorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, D_alphabetColor.length * 4, ColorBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		GLES32.glBindVertexArray(VAObj_INDIA[3]);		// For Alphabet A
			GLES32.glGenBuffers(2, VBObj_alpha_A, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_A[0]);		// Vertices of A
			byteBuffer_v = ByteBuffer.allocateDirect(A_alphabetVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			VerticesBuffer = byteBuffer_v.asFloatBuffer();
			VerticesBuffer.put(A_alphabetVertices);
			VerticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, A_alphabetVertices.length * 4, VerticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_alpha_A[1]);		// Color of A
			byteBuffer_c = ByteBuffer.allocateDirect(A_alphabetColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			ColorBuffer = byteBuffer_c.asFloatBuffer();
			ColorBuffer.put(A_alphabetColor);
			ColorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, A_alphabetColor.length * 4, ColorBuffer, GLES32.GL_STATIC_DRAW);
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
	}

	private void draw() {
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

		// Use shader program
		GLES32.glUseProgram(SPObj);

		// For INDIA
		float MV_matrix[] = new float[16];
		float MVP_matrix[] = new float[16];
		float translation_matrix[] = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -2.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);

		// For alphabet I
		Matrix.translateM(translation_matrix, 0, -2.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
		drawAlphabet(VAObj_INDIA[0], GLES32.GL_TRIANGLE_FAN, 0, 12, 4);

		// For alphabet N
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
		drawAlphabet(VAObj_INDIA[1], GLES32.GL_TRIANGLE_FAN, 0, 20, 4);

		// For alphabet D
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
		drawAlphabet(VAObj_INDIA[2], GLES32.GL_TRIANGLE_FAN, 0, 20, 4);

		// For alphabet I
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
		drawAlphabet(VAObj_INDIA[0], GLES32.GL_TRIANGLE_FAN, 0, 12, 4);

		// For alphabet A
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
		drawAlphabet(VAObj_INDIA[3], GLES32.GL_TRIANGLE_FAN, 0, 28, 4);

		GLES32.glUseProgram(0);

		// render or flush
		requestRender();
	}

	void drawAlphabet(int VAO, int drawingPrimitive, int start, int end, int offset) {
		// Code
		GLES32.glBindVertexArray(VAO);
		for(int i = start; i <= end; i += offset)
			GLES32.glDrawArrays(drawingPrimitive, i, offset);
		GLES32.glBindVertexArray(0);
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj_INDIA[0] != 0) {
			GLES32.glDeleteVertexArrays(4, VAObj_INDIA, 0);
			VAObj_INDIA[0] = 0;
			VAObj_INDIA[1] = 0;
			VAObj_INDIA[2] = 0;
			VAObj_INDIA[3] = 0;
		}

		// Delete Vertex Buffer Object
		if(VBObj_alpha_I[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_alpha_I, 0);
			VBObj_alpha_I[0] = 0;
			VBObj_alpha_I[1] = 0;
		}
		if(VBObj_alpha_N[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_alpha_N, 0);
			VBObj_alpha_N[0] = 0;
			VBObj_alpha_N[1] = 0;
		}
		if(VBObj_alpha_D[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_alpha_D, 0);
			VBObj_alpha_D[0] = 0;
			VBObj_alpha_D[1] = 0;
		}
		if(VBObj_alpha_A[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_alpha_A, 0);
			VBObj_alpha_A[0] = 0;
			VBObj_alpha_A[1] = 0;
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
