package com.rtr3_android.tessellation;

import android.content.Context;		// For drawing context
import android.opengl.GLSurfaceView;	// for OpenGL Surface view
import javax.microedition.khronos.opengles.GL10;	// For OpenGLES 1.0 (required)
import javax.microedition.khronos.egl.EGLConfig;	// For EGLConfig (as needed)
import android.opengl.GLES32;		// For OpenGL-ES 3.2

import android.util.Log;
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
	private int TCSObj, TESObj;
	private int FSObj;
	private int SPObj;

	private int[] VAObj = new int[1];
	private int[] VBObj = new int[1];

	private int MVPUniform;
	private int SegmentCntUniform;
	private int StripeCntUniform;

	private int segmentCount;

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
		segmentCount++;
		if(segmentCount > 30)
			segmentCount = 30;
		return (true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent event) {
		return (true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent event) {
		segmentCount--;
		if(segmentCount < 1)
			segmentCount = 1;
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
			"#version 320 es \n"+
			"in vec2 vPosition;"+
			"void main(void) {"+
				"gl_Position = vec4(vPosition, 0.0, 0.0);"+
			"}"
		);
		GLES32.glShaderSource(VSObj, VSSrcCode);
		GLES32.glCompileShader(VSObj);
		if(!ShaderErrorCheck(VSObj, GLES32.GL_VERTEX_SHADER)) {
			System.out.print("Error in Vertex Shader");
			return;
		}

		// Tessellation Control Shader
		TCSObj = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);
		final String TCSSrcCode = String.format(
			"#version 320 es \n"+
			"layout(vertices = 4)out;"+
			"uniform int numberOfSegments;"+
			"uniform int numberOfStrips;"+
			"void main(void) {"+
				"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"+
					"gl_TessLevelOuter[0] = float(numberOfStrips);"+
					"gl_TessLevelOuter[1] = float(numberOfSegments);"+
			"}"
		);
		GLES32.glShaderSource(TCSObj, TCSSrcCode);
		GLES32.glCompileShader(TCSObj);
		if(!ShaderErrorCheck(TCSObj, GLES32.GL_TESS_CONTROL_SHADER)) {
			System.out.print("Error in Tessellation Control Shader");
			return;
		}

		// Tessellation Evaluation Shader
		TESObj = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);
		final String TESSrcCode = String.format(
			"#version 320 es \n"+
			"layout(isolines)in;"+
			"uniform mat4 u_mvp_matrix;"+
			"void main(void) {"+
				"float tessCoords = gl_TessCoord.x;"+
				"vec3 p0 = gl_in[0].gl_Position.xyz;"+
				"vec3 p1 = gl_in[1].gl_Position.xyz;"+
				"vec3 p2 = gl_in[2].gl_Position.xyz;"+
				"vec3 p3 = gl_in[3].gl_Position.xyz;"+
				"vec3 p = (p0 * (1.0-tessCoords) * (1.0-tessCoords) * (1.0-tessCoords)) + (3.0 * p1 * (1.0-tessCoords) * (1.0-tessCoords) * tessCoords) + (3.0 * p2 * (1.0-tessCoords) * tessCoords * tessCoords) + (p3 * tessCoords * tessCoords * tessCoords);"+
				"gl_Position = u_mvp_matrix * vec4(p, 1.0);"+
			"}"
		);
		GLES32.glShaderSource(TESObj, TESSrcCode);
		GLES32.glCompileShader(TESObj);
		if(!ShaderErrorCheck(TESObj, GLES32.GL_TESS_EVALUATION_SHADER)) {
			System.out.print("Error in Tessellation Evaluation Shader");
			return;
		}

		// Fragment shader
		FSObj = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		final String FSSrcCode = String.format(
			"#version 320 es \n"+
			"precision mediump float;"+
			"out vec4 FragColor;"+
			"void main(void) {"+
				"FragColor = vec4(1.0, 1.0, 0.0, 0.0);"+
			"}"
		);
		GLES32.glShaderSource(FSObj, FSSrcCode);
		GLES32.glCompileShader(FSObj);
		if(!ShaderErrorCheck(FSObj, GLES32.GL_FRAGMENT_SHADER)) {
			System.out.print("Error in Fragment Shader");
			return;
		}

		SPObj = GLES32.glCreateProgram();
		GLES32.glAttachShader(SPObj, VSObj);
		GLES32.glAttachShader(SPObj, TCSObj);
		GLES32.glAttachShader(SPObj, TESObj);
		GLES32.glAttachShader(SPObj, FSObj);
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_VERTEX, "vPosition");
		GLES32.glLinkProgram(SPObj);
		ShaderErrorCheck(SPObj, 0);

		// get uniforms
		MVPUniform = GLES32.glGetUniformLocation(SPObj, "u_mvp_matrix");
		SegmentCntUniform = GLES32.glGetUniformLocation(SPObj, "numberOfSegments");
		StripeCntUniform = GLES32.glGetUniformLocation(SPObj, "numberOfStrips");
//		Log.d("DarshanDebug", ""+SegmentCntUniform+"  "+StripeCntUniform);

		// vertices, color, shader attribs, VAOs, VBOs initialization
		final float[] bezierControlPoints = new float[] {
			-1.0f, -1.0f,		-0.5f, 1.0f,		0.5f, -1.0f,		1.0f, 1.0f,
		};
		GLES32.glGenVertexArrays(1, VAObj, 0);
		GLES32.glBindVertexArray(VAObj[0]);
			GLES32.glGenBuffers(1, VBObj, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj[0]);
			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(bezierControlPoints.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
			verticesBuffer.put(bezierControlPoints);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, bezierControlPoints.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		// Enable depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Enable culling
		GLES32.glEnable(GLES32.GL_CULL_FACE);

		segmentCount = 1;

		// Set background frame color
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// set projection matrix to identity
		Matrix.setIdentityM(perspProjMatrix, 0);
	}

	private boolean ShaderErrorCheck(int shaderObject, int type) {
		int[] iStatus = new int[1];
		int[] iInfoLogLength = new int[1];
		String szInfoLog;

		if(type == GLES32.GL_VERTEX_SHADER || type == GLES32.GL_FRAGMENT_SHADER || type == GLES32.GL_TESS_CONTROL_SHADER || type == GLES32.GL_TESS_EVALUATION_SHADER)
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

		float[] MV_matrix = new float[16];
		float[] MVP_matrix = new float[16];
		float[] translation_matrix = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -3.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);
		GLES32.glUniform1i(SegmentCntUniform, segmentCount);
		GLES32.glUniform1i(StripeCntUniform, 1);

		GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);
		GLES32.glBindVertexArray(VAObj[0]);
			GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

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
		if(VBObj[0] != 0) {
			GLES32.glDeleteBuffers(1, VBObj, 0);
			VBObj[0] = 0;
		}

		if(SPObj != 0) {
			if(VSObj != 0) {
				GLES32.glDetachShader(SPObj, VSObj);
				GLES32.glDeleteShader(VSObj);
				VSObj = 0;
			}
			if(TCSObj != 0) {
				GLES32.glDetachShader(SPObj, TCSObj);
				GLES32.glDeleteShader(TCSObj);
				TCSObj = 0;
			}
			if(TESObj != 0) {
				GLES32.glDetachShader(SPObj, TESObj);
				GLES32.glDeleteShader(TESObj);
				TESObj = 0;
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
