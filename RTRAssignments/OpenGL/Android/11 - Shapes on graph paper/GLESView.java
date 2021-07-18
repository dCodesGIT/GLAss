package com.rtr3_android.graphpaper;

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
import java.lang.Math;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
	private GestureDetector gestureDetector;

	private int VSObj;
	private int FSObj;
	private int SPObj;

	private int[] VAObj_gp = new int[1];		// VAO for graph paper
	private int[] VBObj_gp = new int[2];		// [0]-Position, [1]-Color

	private int[] VAObj_shapes = new int[3];	// [0]-Triangle, [1]-Square, [2]-Circle
	private int[] VBObj_triangle = new int[2];	// [0]-Position, [1]-Color
	private int[] VBObj_square = new int[2];	// [0]-Position, [1]-Color
	private int[] VBObj_circle = new int[2];	// [0]-Position, [1]-Color
	private int MVPUniform;

	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private int shapeCnt = 0;
	private boolean allInOne = false;
	private boolean animationEnabled = false;
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
		allInOne = !allInOne;
		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent event) {
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent event) {
		shapeCnt++;
		if(shapeCnt > 3)
			shapeCnt = 1;
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
		animationEnabled = !animationEnabled;
		gfAngle = 0.0f;
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
		float GraphPaperVertices[][] = new float[164][2];
		float GraphPaperColor[][] = new float[164][4];
		int i;
		float vert;
		for(i = 0, vert = -2.0f; i < 164 && vert <= 2.1f; i++, vert += 0.1f) {
			// Left coordinate of horizontal line
			GraphPaperVertices[i][0] = -2.0f;
			GraphPaperVertices[i][1] = vert;
				GraphPaperColor[i][0] = 0.0f;
				GraphPaperColor[i][1] = 0.0f;
				GraphPaperColor[i][2] = 1.0f;
				GraphPaperColor[i][3] = 0.0f;

			// Right coordinate of horizontal line
			i++;
			GraphPaperVertices[i][0] = 2.0f;
			GraphPaperVertices[i][1] = vert;
				GraphPaperColor[i][0] = 0.0f;
				GraphPaperColor[i][1] = 0.0f;
				GraphPaperColor[i][2] = 1.0f;
				GraphPaperColor[i][3] = 0.0f;

			// Bottom coordinate of Vertical line
			i++;
			GraphPaperVertices[i][0] = vert;
			GraphPaperVertices[i][1] = -2.0f;
				GraphPaperColor[i][0] = 0.0f;
				GraphPaperColor[i][1] = 0.0f;
				GraphPaperColor[i][2] = 1.0f;
				GraphPaperColor[i][3] = 0.0f;

			// Top coordinate of Vertical line
			i++;
			GraphPaperVertices[i][0] = vert;
			GraphPaperVertices[i][1] = 2.0f;
				GraphPaperColor[i][0] = 0.0f;
				GraphPaperColor[i][1] = 0.0f;
				GraphPaperColor[i][2] = 1.0f;
				GraphPaperColor[i][3] = 0.0f;
		}
		// Red color of X Axis - left point
		GraphPaperColor[164/2][0] = 1.0f;
		GraphPaperColor[164/2][1] = 0.0f;
		GraphPaperColor[164/2][2] = 0.0f;
		GraphPaperColor[164/2][3] = 0.0f;

		// Red color of X Axis - right point
		GraphPaperColor[(164/2)+1][0] = 1.0f;
		GraphPaperColor[(164/2)+1][1] = 0.0f;
		GraphPaperColor[(164/2)+1][2] = 0.0f;
		GraphPaperColor[(164/2)+1][3] = 0.0f;

		// Green color of Y Axis - Bottom point
		GraphPaperColor[(164/2)-1][0] = 0.0f;
		GraphPaperColor[(164/2)-1][1] = 1.0f;
		GraphPaperColor[(164/2)-1][2] = 0.0f;
		GraphPaperColor[(164/2)-1][3] = 0.0f;

		// Green color of Y Axis - Top point
		GraphPaperColor[(164/2)-2][0] = 0.0f;
		GraphPaperColor[(164/2)-2][1] = 1.0f;
		GraphPaperColor[(164/2)-2][2] = 0.0f;
		GraphPaperColor[(164/2)-2][3] = 0.0f;

		// Converting 2D array into 1D array
		float gpVertices[] = new float[164*2];
		float gpColor[] = new float[164*4];
		for(i = 0; i < 164; i++) {
			gpVertices[(i*2)+0] = GraphPaperVertices[i][0];
			gpVertices[(i*2)+1] = GraphPaperVertices[i][1];

			gpColor[(i*4)+0] = GraphPaperColor[i][0];
			gpColor[(i*4)+1] = GraphPaperColor[i][1];
			gpColor[(i*4)+2] = GraphPaperColor[i][2];
			gpColor[(i*4)+3] = GraphPaperColor[i][3];
		}

		// For graph paper
		GLES32.glGenVertexArrays(1, VAObj_gp, 0);
		GLES32.glBindVertexArray(VAObj_gp[0]);
			GLES32.glGenBuffers(2, VBObj_gp, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_gp[0]);	// For Position
			ByteBuffer byteBuffer_v = ByteBuffer.allocateDirect(gpVertices.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer_v.asFloatBuffer();
			verticesBuffer.put(gpVertices);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, gpVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_gp[1]);	// For color
			ByteBuffer byteBuffer_c = ByteBuffer.allocateDirect(gpColor.length * 4);
			byteBuffer_c.order(ByteOrder.nativeOrder());
			FloatBuffer colorBuffer = byteBuffer_c.asFloatBuffer();
			colorBuffer.put(gpColor);
			colorBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, gpColor.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 4, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		final float triangleVertices[] = new float[] {
			0.0f, 1.0f, 0.0f,	// Apex
			-1.0f, -1.0f, 0.0f,	// Left bottom
			1.0f, -1.0f, 0.0f	// Right bottom
		};
		final float squareVertices[] = new float[] {
			-1.0f, 1.0f, 0.0f,	// top left
			-1.0f, -1.0f, 0.0f,	// bottom left
			1.0f, -1.0f, 0.0f,	// bottom right
			1.0f, 1.0f, 0.0f	// top right
		};

		GLES32.glGenVertexArrays(3, VAObj_shapes, 0);
		GLES32.glBindVertexArray(VAObj_shapes[0]);		// For triangle
			GLES32.glGenBuffers(2, VBObj_triangle, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_triangle[0]);		// Vertices of triangle
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, (FloatBuffer)null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_triangle[1]);		// Color of triangle
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3 * 3 * 4, (FloatBuffer)null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		GLES32.glBindVertexArray(VAObj_shapes[1]);		// For square
			GLES32.glGenBuffers(2, VBObj_square, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_square[0]);		// Vertices of square
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, (FloatBuffer)null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_square[1]);		// Color of square
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, (FloatBuffer)null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
	
		GLES32.glBindVertexArray(VAObj_shapes[2]);		// For square
			GLES32.glGenBuffers(2, VBObj_circle, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_circle[0]);		// Vertices of square
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3600 * 3 * 4, (FloatBuffer)null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_circle[1]);		// Color of square
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 3600 * 3 * 4, (FloatBuffer)null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		// Enable depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Enable culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

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

		// For Graph Paper
		float MV_matrix[] = new float[16];
		float MVP_matrix[] = new float[16];
		float translation_matrix[] = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -5.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glBindVertexArray(VAObj_gp[0]);
		for(int i = 0; i <164; i += 2)
			GLES32.glDrawArrays(GLES32.GL_LINES, i, 2);
		GLES32.glBindVertexArray(0);

		// For Shapes
		float rotation_matrix[] = new float[16];
		if(shapeCnt == 1 || allInOne == true) {
			float triangleVertices[] = new float[] {
				0.0f, 1.0f, 0.0f,	// Apex
				-1.0f, -1.0f, 0.0f,	// Bottom left
				1.0f, -1.0f, 0.0f	// Bottom right
			};
			final float triangleColor[] = new float[] {
				1.0f, 1.0f, 0.0f,	// Apex
				1.0f, 1.0f, 0.0f,	// Bottom left
				1.0f, 1.0f, 0.0f	// Bottom right
			};
			if(animationEnabled && allInOne) {
				Matrix.setIdentityM(MV_matrix, 0);
				Matrix.setIdentityM(MVP_matrix, 0);
				Matrix.setIdentityM(translation_matrix, 0);
				Matrix.setIdentityM(rotation_matrix, 0);
				Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -5.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
				Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
				Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
				GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
			}
			GLES32.glBindVertexArray(VAObj_shapes[0]);		// For triangle
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_triangle[0]);		// Vertices of triangle
				ByteBuffer byteBuffer_tri_v = ByteBuffer.allocateDirect(triangleVertices.length * 4);
				byteBuffer_tri_v.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer_tri = byteBuffer_tri_v.asFloatBuffer();
				verticesBuffer_tri.put(triangleVertices);
				verticesBuffer_tri.position(0);
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, verticesBuffer_tri, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
				
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_triangle[1]);		// Color of triangle
				ByteBuffer byteBuffer_tri_c = ByteBuffer.allocateDirect(triangleColor.length * 4);
				byteBuffer_tri_c.order(ByteOrder.nativeOrder());
				FloatBuffer ColorBuffer_tri = byteBuffer_tri_c.asFloatBuffer();
				ColorBuffer_tri.put(triangleColor);
				ColorBuffer_tri.position(0);
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleColor.length * 4, ColorBuffer_tri, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
				
				GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 3);
			GLES32.glBindVertexArray(0);
		}
		if(shapeCnt == 2 || allInOne == true) {
			final float squareVertices[] = new float[] {
				-1.0f, 1.0f, 0.0f,	// Top left
				-1.0f, -1.0f, 0.0f,	// Bottom left
				1.0f, -1.0f, 0.0f,	// Bottom right
				1.0f, 1.0f, 0.0f	// Top right
			};
			final float squareColor[] = new float[] {
				1.0f, 1.0f, 0.0f,	// Top right
				1.0f, 1.0f, 0.0f,	// Bottom left
				1.0f, 1.0f, 0.0f,	// Bottom right
				1.0f, 1.0f, 0.0f	// Top right
			};
			if(animationEnabled && allInOne) {
				Matrix.setIdentityM(MV_matrix, 0);
				Matrix.setIdentityM(MVP_matrix, 0);
				Matrix.setIdentityM(translation_matrix, 0);
				Matrix.setIdentityM(rotation_matrix, 0);
				Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -5.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
				Matrix.rotateM(rotation_matrix, 0, gfAngle, 1.0f, 0.0f, 0.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
				Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
				GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
			}
			GLES32.glBindVertexArray(VAObj_shapes[1]);		// For square
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_square[0]);		// Vertices of square
				ByteBuffer byteBuffer_sq_v = ByteBuffer.allocateDirect(squareVertices.length * 4);
				byteBuffer_sq_v.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer_sq = byteBuffer_sq_v.asFloatBuffer();
				verticesBuffer_sq.put(squareVertices);
				verticesBuffer_sq.position(0);
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, verticesBuffer_sq, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_square[1]);		// Color of square
				ByteBuffer byteBuffer_sq_c = ByteBuffer.allocateDirect(squareColor.length * 4);
				byteBuffer_sq_c.order(ByteOrder.nativeOrder());
				FloatBuffer ColorBuffer_sq = byteBuffer_sq_c.asFloatBuffer();
				ColorBuffer_sq.put(squareColor);
				ColorBuffer_sq.position(0);
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareColor.length * 4, ColorBuffer_sq, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

				GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 4);
			GLES32.glBindVertexArray(0);
		}
		if(shapeCnt == 3 || allInOne == true) {
			final int circ_points = 3600;
			float circleVertices[] = new float[circ_points*2];
			float circleColor[] = new float[circ_points*3];
			final float arcConst = (float)circ_points / 180.0f;
			float radius;
			if(allInOne)
				radius = 1.0f + (0.5f * (float)Math.sin(45.0f));
			else
				radius = 1.0f;
			for(int i = 0; i < circ_points*2; i += 2) {
				circleVertices[i] = radius * (float)Math.cos(((float)i/arcConst)*(Math.PI/180.0f));	// X = r * cos(theta)
				circleVertices[i+1] = radius * (float)Math.sin(((float)i/arcConst)*(Math.PI/180.0f));	// Y = r * sin(theta)
			}
			for(int i = 0; i < circ_points*3; i += 3) {
				circleColor[i] = 1.0f;
				circleColor[i+1] = 1.0f;
				circleColor[i+2] = 0.0f;
			}
			if(animationEnabled && allInOne) {
				Matrix.setIdentityM(MV_matrix, 0);
				Matrix.setIdentityM(MVP_matrix, 0);
				Matrix.setIdentityM(translation_matrix, 0);
				Matrix.setIdentityM(rotation_matrix, 0);
				Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -5.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
				Matrix.rotateM(rotation_matrix, 0, gfAngle * 2.0f, 0.0f, 1.0f, 1.0f);
				Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
				Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
				GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
			}
			GLES32.glBindVertexArray(VAObj_shapes[2]);		// For circle
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_circle[0]);		// Vertices of circle
				ByteBuffer byteBuffer_circ_v = ByteBuffer.allocateDirect(circleVertices.length * 4);
				byteBuffer_circ_v.order(ByteOrder.nativeOrder());
				FloatBuffer verticesBuffer_circ = byteBuffer_circ_v.asFloatBuffer();
				verticesBuffer_circ.put(circleVertices);
				verticesBuffer_circ.position(0);
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circleVertices.length * 4, verticesBuffer_circ, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
				
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_circle[1]);		// Color of circle
				ByteBuffer byteBuffer_circ_c = ByteBuffer.allocateDirect(circleColor.length * 4);
				byteBuffer_circ_c.order(ByteOrder.nativeOrder());
				FloatBuffer ColorBuffer_circ = byteBuffer_circ_c.asFloatBuffer();
				ColorBuffer_circ.put(circleColor);
				ColorBuffer_circ.position(0);
				GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circleColor.length * 4, ColorBuffer_circ, GLES32.GL_DYNAMIC_DRAW);
				GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
				GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
				GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
				
				GLES32.glDrawArrays(GLES32.GL_POINTS, 0, circ_points);
			GLES32.glBindVertexArray(0);
		}
		GLES32.glUseProgram(0);

		if(animationEnabled && allInOne)
			gfAngle += 0.5f;
		if(gfAngle > 360.0f)
			gfAngle = 0.0f;

		// render or flush
		requestRender();
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj_shapes[0] != 0) {
			GLES32.glDeleteVertexArrays(3, VAObj_shapes, 0);
			VAObj_shapes[0] = 0;
			VAObj_shapes[1] = 0;
			VAObj_shapes[2] = 0;
		}

		// Delete Vertex Buffer Object
		if(VBObj_triangle[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_triangle, 0);
			VBObj_triangle[0] = 0;
			VBObj_triangle[1] = 0;
		}
		if(VBObj_square[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_square, 0);
			VBObj_square[0] = 0;
			VBObj_square[1] = 0;
		}
		if(VBObj_circle[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_circle, 0);
			VBObj_circle[0] = 0;
			VBObj_circle[1] = 0;
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
