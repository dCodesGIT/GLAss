package com.rtr3_android.checkerboard;

import android.content.Context;		// For drawing context
import android.opengl.GLSurfaceView;	// for OpenGL Surface view
import javax.microedition.khronos.opengles.GL10;	// For OpenGLES 1.0 (required)
import javax.microedition.khronos.egl.EGLConfig;	// For EGLConfig (as needed)
import android.opengl.GLES32;		// For OpenGL-ES 3.2

import android.view.MotionEvent;	// For Motion event
import android.view.GestureDetector;	// For GestureDetector
import android.view.GestureDetector.OnGestureListener;		// For Gestures
import android.view.GestureDetector.OnDoubleTapListener;	// For Taps

// For texture loading
import android.graphics.Bitmap;
import android.opengl.GLUtils;

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

	private int[] VAObj_boards = new int[1];	// [0]-Quad
	private int[] VBObj_checker = new int[2];		// [0]-Position, [1]-Texture
	private int MVPUniform;

	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private int TexSampUniform;

	private final int CheckerImageHeight = 64;
	private final int CheckerImageWidth = 64;
	private byte CheckerImage[] = new byte[CheckerImageWidth * CheckerImageHeight * 4];
	private int[] procedural_texture = new int[1];

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
			"in vec2 vTexCoord;"+
			"uniform mat4 u_mvp_matrix;"+
			"out vec2 out_TexCoord;"+
			"void main(void) {"+
				"gl_Position = u_mvp_matrix * vPosition;"+
				"out_TexCoord = vTexCoord;"+
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
			"in vec2 out_TexCoord;"+
			"uniform highp sampler2D u_texture_sampler;"+
			"out vec4 FragColor;"+
			"void main(void) {"+
				"FragColor = texture(u_texture_sampler, out_TexCoord);"+
			"}"
		);
		GLES32.glShaderSource(FSObj, FSSrcCode);
		GLES32.glCompileShader(FSObj);
		ShaderErrorCheck(FSObj, GLES32.GL_FRAGMENT_SHADER);

		SPObj = GLES32.glCreateProgram();		// create shader program
		GLES32.glAttachShader(SPObj, VSObj);		// Attach vertex shader
		GLES32.glAttachShader(SPObj, FSObj);		// Attach fragment shader
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_VERTEX, "vPosition");
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_TEXTURE, "vTexCoord");
		GLES32.glLinkProgram(SPObj);
		ShaderErrorCheck(SPObj, 0);

		// get uniforms
		MVPUniform = GLES32.glGetUniformLocation(SPObj, "u_mvp_matrix");
		TexSampUniform = GLES32.glGetUniformLocation(SPObj, "u_texture_sampler");

		// vertices, color, shader attribs, VAOs, VBOs initialization
		final float checkerboardTexCoords[] = {
			0.0f, 1.0f,	// Left top
			0.0f, 0.0f,	// Left bottom
			1.0f, 0.0f,	// Right bottom
			1.0f, 1.0f	// Right top
		};

		GLES32.glGenVertexArrays(1, VAObj_boards, 0);
		GLES32.glBindVertexArray(VAObj_boards[0]);		// For checkerboard
			GLES32.glGenBuffers(2, VBObj_checker, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_checker[0]);		// Vertices of checkerboard
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 3 * 4, null, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
			
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_checker[1]);		// TexCoords of checkerboard
			ByteBuffer byteBuffer_t = ByteBuffer.allocateDirect(checkerboardTexCoords.length * 4);
			byteBuffer_t.order(ByteOrder.nativeOrder());
			FloatBuffer TexCoordBuffer = byteBuffer_t.asFloatBuffer();
			TexCoordBuffer.put(checkerboardTexCoords);
			TexCoordBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, checkerboardTexCoords.length * 4, TexCoordBuffer, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_TEXTURE, 2, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_TEXTURE);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);
	
		// Enable depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Enabling Texture
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);
		// Specific to loading checkerboard as a texture(Procedural texture)
		procedural_texture[0] = loadProceduralTexture();

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

	private int loadProceduralTexture() {
		// Variable declaration
		int texture[] = new int[1];

		// Code
		MakeCheckerImage();
		Bitmap bitmap = Bitmap.createBitmap(CheckerImageWidth, CheckerImageHeight, Bitmap.Config.ARGB_8888);

		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(CheckerImageWidth * CheckerImageHeight * 4);
		byteBuffer.order(ByteOrder.nativeOrder());
		byteBuffer.put(CheckerImage);
		byteBuffer.position(0);

		bitmap.copyPixelsFromBuffer(byteBuffer);

		GLES32.glGenTextures(1, texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

		return texture[0];
	}

	private void MakeCheckerImage() {
		// Variable declaration
		int i, j, c;

		// Code
		for(i = 0; i < CheckerImageHeight; i++) {
			for(j = 0; j < CheckerImageWidth; j++) {
				c = ((i & 8) ^ (j & 8)) * 255;
				CheckerImage[(i * 64 + j) * 4 + 0] = (byte)c;
				CheckerImage[(i * 64 + j) * 4 + 1] = (byte)c;
				CheckerImage[(i * 64 + j) * 4 + 2] = (byte)c;
				CheckerImage[(i * 64 + j) * 4 + 3] = (byte)0xff;
			}
		}
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

		// For checkerboard
		float MV_matrix[] = new float[16];
		float MVP_matrix[] = new float[16];
		float translation_matrix[] = new float[16];

		Matrix.setIdentityM(MV_matrix, 0);
		Matrix.setIdentityM(MVP_matrix, 0);
		Matrix.setIdentityM(translation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -3.6f);
		Matrix.multiplyMM(MV_matrix, 0, MV_matrix, 0, translation_matrix, 0);
		Matrix.multiplyMM(MVP_matrix, 0, perspProjMatrix, 0, MV_matrix, 0);
		GLES32.glUniformMatrix4fv(MVPUniform, 1, false, MVP_matrix, 0);

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, procedural_texture[0]);
		GLES32.glUniform1i(TexSampUniform, 0);

		final float checkerboardVertices_straight[] = {
			-2.0f, 1.0f, 0.0f,	// Left top
			-2.0f, -1.0f, 0.0f,	// Left bottom
			0.0f, -1.0f, 0.0f,	// Right bottom
			0.0f, 1.0f, 0.0f	// Right top
		};
		GLES32.glBindVertexArray(VAObj_boards[0]);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_checker[0]);
			ByteBuffer byteBuffer_v = ByteBuffer.allocateDirect(checkerboardVertices_straight.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			FloatBuffer verticesBuffer = byteBuffer_v.asFloatBuffer();
			verticesBuffer.put(checkerboardVertices_straight);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, checkerboardVertices_straight.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glBindVertexArray(0);

		final float checkerboardVertices_tilted[] = {
			1.0f, 1.0f, 0.0f,		// Left top
			1.0f, -1.0f, 0.0f,		// Left bottom
			2.41421f, -1.0f, -1.41421f,	// Right bottom
			2.41421f, 1.0f, -1.41421f	// Right top
		};
		GLES32.glBindVertexArray(VAObj_boards[0]);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj_checker[0]);
			byteBuffer_v = ByteBuffer.allocateDirect(checkerboardVertices_tilted.length * 4);
			byteBuffer_v.order(ByteOrder.nativeOrder());
			verticesBuffer = byteBuffer_v.asFloatBuffer();
			verticesBuffer.put(checkerboardVertices_tilted);
			verticesBuffer.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, checkerboardVertices_tilted.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);
	
		// render or flush
		requestRender();
	}

	private void Uninitialize() {
		// Code
		// Delete Vertex Array Object
		if(VAObj_boards[0] != 0) {
			GLES32.glDeleteVertexArrays(1, VAObj_boards, 0);
			VAObj_boards[0] = 0;
		}

		// Delete Vertex Buffer Object
		if(VBObj_checker[0] != 0) {
			GLES32.glDeleteBuffers(2, VBObj_checker, 0);
			VBObj_checker[0] = 0;
			VBObj_checker[1] = 0;
		}

		// Delete textures
		if(procedural_texture[0] != 0) {
			GLES32.glDeleteTextures(1, procedural_texture, 0);
			procedural_texture[0] = 0;
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
