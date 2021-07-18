package com.rtr3_android.deathlyhallow;

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

	private int[] VAObj_Cloak_Of_Invisibility = new int[1];
	private int[] VBObj_Cloak_Of_Invisibility = new int[1];
	private int[] VAObj_Resurrection_Stone = new int[1];
	private int[] VBObj_Resurrection_Stone = new int[1];
	private int[] VAObj_Elder_Wand = new int[1];
	private int[] VBObj_Elder_Wand = new int[1];
	private int MVPUniform;

	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private float gfAngle = 0.0f;
	private float trans = 3.5f;

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
			"uniform mat4 u_mvp_matrix;"+
			"void main(void) {"+
				"gl_Position = u_mvp_matrix * vPosition;"+
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
			"out vec4 FragColor;"+
			"void main(void) {"+
				"FragColor = vec4(1.0);"+
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
		MVPUniform = GLES32.glGetUniformLocation(SPObj, "u_mvp_matrix");

		// vertices, color, shader attribs, VAOs, VBOs initialization
		final float radian = (float)Math.PI / 180.0f;
		final float TriangleSide = 1.5f;
		final float Triangle_radius = (2.0f / (float)Math.sqrt(3.0f)) * TriangleSide;
		final float inCircle_radius = TriangleSide / (float)Math.sqrt(3.0f);

		final float CloakOfInvisibilityVertices[] = {
			(float)(Triangle_radius * Math.cos(90.0f * radian)), (float)(Triangle_radius * Math.sin(90.0f * radian)), 0.0f,		// Apex
			(float)(Triangle_radius * Math.cos(210.0f * radian)), (float)(Triangle_radius * Math.sin(210.0f * radian)), 0.0f,	// left bottom
			(float)(Triangle_radius * Math.cos(330.0f * radian)), (float)(Triangle_radius * Math.sin(330.0f * radian)), 0.0f	// right bottom
		};
		float ResurrectionStoneVertices[] = new float[3600*2];
		for(int i = 0; i < 3600*2; i += 2) {
			ResurrectionStoneVertices[i] = (float)(inCircle_radius * Math.cos(i * radian / 20.0f));
			ResurrectionStoneVertices[i+1] = (float)(inCircle_radius * Math.sin(i * radian / 20.0f));
		}
		final float ElderWandVertices[] = {
			0.0f, (float)(inCircle_radius * 2), 0.0f,
			0.0f, (float)(-inCircle_radius), 0.0f
		};

		GLES32.glGenVertexArrays(1, VAObj_Cloak_Of_Invisibility, 0);		// For Cloak Of Invisibility
		GLES32.glBindVertexArray(VAObj_Cloak_Of_Invisibility[0]);
			GLES32.glGenBuffers(1, VBObj_Cloak_Of_Invisibility, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_Cloak_Of_Invisibility[0]);		// Vertices of Cloak Of Invisibility
			ByteBuffer byteBuffer_coi_v = ByteBuffer.allocateDirect(CloakOfInvisibilityVertices.length * 4);
			byteBuffer_coi_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer_coi = byteBuffer_coi_v.asFloatBuffer();
			verticesBuffer_coi.put(CloakOfInvisibilityVertices);
			verticesBuffer_coi.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, CloakOfInvisibilityVertices.length * 4, verticesBuffer_coi, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		GLES32.glGenVertexArrays(1, VAObj_Resurrection_Stone, 0);		// For Resurrection Stone
		GLES32.glBindVertexArray(VAObj_Resurrection_Stone[0]);
			GLES32.glGenBuffers(1, VBObj_Resurrection_Stone, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_Resurrection_Stone[0]);		// Vertices of Resurrection Stone
			ByteBuffer byteBuffer_rs_v = ByteBuffer.allocateDirect(ResurrectionStoneVertices.length * 4);
			byteBuffer_rs_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer_rs = byteBuffer_rs_v.asFloatBuffer();
			verticesBuffer_rs.put(ResurrectionStoneVertices);
			verticesBuffer_rs.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, ResurrectionStoneVertices.length * 4, verticesBuffer_rs, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
	
		GLES32.glGenVertexArrays(1, VAObj_Elder_Wand, 0);		// For Cloak Of Invisibility
		GLES32.glBindVertexArray(VAObj_Elder_Wand[0]);
			GLES32.glGenBuffers(1, VBObj_Elder_Wand, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_Elder_Wand[0]);		// Vertices of Cloak Of Invisibility
			ByteBuffer byteBuffer_ew_v = ByteBuffer.allocateDirect(ElderWandVertices.length * 4);
			byteBuffer_ew_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer_ew = byteBuffer_ew_v.asFloatBuffer();
			verticesBuffer_ew.put(ElderWandVertices);
			verticesBuffer_ew.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, ElderWandVertices.length * 4, verticesBuffer_ew, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
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

		// For Cloak of Invisibility
		float MV_matrix[] = new float[16];
		float MVP_matrix[] = new float[16];
		float translation_matrix[] = new float[16];
		float rotation_matrix[] = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -2.5f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, -trans, -trans, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glBindVertexArray(VAObj_Cloak_Of_Invisibility[0]);
			GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 3);
		GLES32.glBindVertexArray(0);

		// For Resurrection Stone
		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -2.5f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, trans, -trans, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glBindVertexArray(VAObj_Resurrection_Stone[0]);
			GLES32.glDrawArrays(GLES32.GL_POINTS, 0, 3600);
		GLES32.glBindVertexArray(0);

		// For Elder Wand
		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -2.5f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.translateM(translation_matrix, 0, 0.0f, trans, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, rotation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glBindVertexArray(VAObj_Elder_Wand[0]);
			GLES32.glDrawArrays(GLES32.GL_LINES, 0, 2);
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		if(trans >= 0.0f)
			trans -= 0.005f;
		if(gfAngle <= 480.0f)
			gfAngle += 0.5f;

		// render or flush
		requestRender();
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj_Cloak_Of_Invisibility[0] != 0) {
			GLES32.glDeleteVertexArrays(1, VAObj_Cloak_Of_Invisibility, 0);
			VAObj_Cloak_Of_Invisibility[0] = 0;
		}
		if(VAObj_Resurrection_Stone[0] != 0) {
			GLES32.glDeleteVertexArrays(1, VAObj_Resurrection_Stone, 0);
			VAObj_Resurrection_Stone[0] = 0;
		}
		if(VAObj_Elder_Wand[0] != 0) {
			GLES32.glDeleteVertexArrays(1, VAObj_Elder_Wand, 0);
			VAObj_Elder_Wand[0] = 0;
		}

		// Delete Vertex Buffer Object
		if(VBObj_Cloak_Of_Invisibility[0] != 0) {
			GLES32.glDeleteBuffers(1, VBObj_Cloak_Of_Invisibility, 0);
			VBObj_Cloak_Of_Invisibility[0] = 0;
		}
		if(VBObj_Resurrection_Stone[0] != 0) {
			GLES32.glDeleteBuffers(1, VBObj_Resurrection_Stone, 0);
			VBObj_Resurrection_Stone[0] = 0;
		}
		if(VBObj_Elder_Wand[0] != 0) {
			GLES32.glDeleteBuffers(1, VBObj_Elder_Wand, 0);
			VBObj_Elder_Wand[0] = 0;
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
