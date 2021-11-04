package com.rtr3_android.interleavedarray;

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
import android.graphics.BitmapFactory;
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

	private int[] VAObj = new int[1];	// [0]-Cube
	private int[] VBObj = new int[1];	// [0]-Interleaved

	private int MMatrixUniform, VMatrixUniform, PMatrixUniform;
	private int LAmbUniform, LDiffUniform, LSpecUniform, LPosUniform;
	private int KAmbUniform, KDiffUniform, KSpecUniform, KShineUniform;
	private int InterleavedUniform;

	private float perspProjMatrix[] = new float[16];	// 4x4 matrix

	private boolean enableInterleaved = false;
	private boolean enableAnimation = false;
	private float gfAngle = 0.0f;

	private int TexSampUniform;
	private int[] marble_texture = new int[1];

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
		enableAnimation = !enableAnimation;
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
		enableInterleaved = !enableInterleaved;
		return;
	}

	@Override
	public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
		Uninitialize();
		System.exit(0);
		return (true);
	}

	@Override
	public void onShowPress(MotionEvent event) {
		return;
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
			"in vec3 vPosition;"+
			"in vec3 vColor;"+
			"in vec3 vNormal;"+
			"in vec2 vTexCoord;"+
			"uniform mat4 u_MMatrix, u_VMatrix, u_PMatrix;"+
			"uniform vec3 u_LAmb, u_LDiff, u_LSpec;"+
			"uniform vec4 u_LPos;"+
			"uniform vec3 u_KAmb, u_KDiff, u_KSpec;" +
			"uniform float u_KShine;"+
			"uniform int u_interleavedEnabled;"+
			"out vec3 out_light;"+
			"out vec2 out_TexCoord;"+
			"void main(void) {"+
				"out_light = vec3(1.0);"+
				"out_TexCoord = vec2(0.0);"+
				"if(u_interleavedEnabled == 1) {"+
					"vec4 eyeCoords = u_VMatrix * u_MMatrix * vec4(vPosition, 1.0f);"+
					"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);"+
					"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));"+
					"vec3 reflectionVector = reflect(-lightSource, transformedNormal);"+
					"vec3 viewVector = normalize(-eyeCoords.xyz);"+
					"vec3 ambient = u_LAmb * u_KAmb;"+
					"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0) * vColor;"+
					"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);"+
					"out_light = ambient + diffuse + specular;"+
					"out_TexCoord = vTexCoord;"+
				"}"+
				"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vec4(vPosition, 1.0f);"+
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
			"in vec3 out_light;"+
			"in vec2 out_TexCoord;"+
			"uniform highp sampler2D u_texture_sampler;"+
			"out vec4 FragColor;"+
			"void main(void) {"+
				"vec3 texColor = texture(u_texture_sampler, out_TexCoord).rgb;"+
				"FragColor = vec4(out_light * texColor, 1.0);"+
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
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_NORMAL, "vNormal");
		GLES32.glBindAttribLocation(SPObj, GLESMacros.DV_ATTRIB_TEXTURE, "vTexCoord");
		GLES32.glLinkProgram(SPObj);
		ShaderErrorCheck(SPObj, 0);

		// get uniforms
		MMatrixUniform = GLES32.glGetUniformLocation(SPObj, "u_MMatrix");
		VMatrixUniform = GLES32.glGetUniformLocation(SPObj, "u_VMatrix");
		PMatrixUniform = GLES32.glGetUniformLocation(SPObj, "u_PMatrix");
		LAmbUniform = GLES32.glGetUniformLocation(SPObj, "u_LAmb");
		LDiffUniform = GLES32.glGetUniformLocation(SPObj, "u_LDiff");
		LSpecUniform = GLES32.glGetUniformLocation(SPObj, "u_LSpec");
		LPosUniform = GLES32.glGetUniformLocation(SPObj, "u_LPos");
		KAmbUniform = GLES32.glGetUniformLocation(SPObj, "u_KAmb");
		KDiffUniform = GLES32.glGetUniformLocation(SPObj, "u_KDiff");
		KSpecUniform = GLES32.glGetUniformLocation(SPObj, "u_KSpec");
		KShineUniform = GLES32.glGetUniformLocation(SPObj, "u_KShine");
		InterleavedUniform = GLES32.glGetUniformLocation(SPObj, "u_interleavedEnabled");
		TexSampUniform = GLES32.glGetUniformLocation(SPObj, "u_texture_sampler");

		// vertices, color, shader attribs, VAOs, VBOs initialization
		float cube[] = {
			// Front face (Top left) - Vertices, Color(red), Normals, TexCoords
			-1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
			// Front face (Bottom left) - Vertices, Color(red), Normals, TexCoords
			-1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
			// Front face (Bottom right) - Vertices, Color(red), Normals, TexCoords
			1.0f, -1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
			// Front face (Top right) - Vertices, Color(red), Normals, TexCoords
			1.0f, 1.0f, 1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
			
			// Right face (Top left) - Vertices, Color(green), Normals, TexCoords
			1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			// Right face (Botttom left) - Vertices, Color(green), Normals, TexCoords
			1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
			// Right face (Bottom right) - Vertices, Color(green), Normals, TexCoords
			1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			// Right face (Top right) - Vertices, Color(green), Normals, TexCoords
			1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
			
			// Bottom face (Top left) - Vertices, Color(blue), Normals, TexCoords
			1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
			// Bottom face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
			-1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			// Bottom face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
			-1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
			// Bottom face (Top right) - Vertices, Color(blue), Normals, TexCoords
			1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
			
			// Left face (Top left) - Vertices, Color(green), Normals, TexCoords
			-1.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
			// Left face (Bottom left) - Vertices, Color(green), Normals, TexCoords
			-1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
			// Left face (Bottom right) - Vertices, Color(green), Normals, TexCoords
			-1.0f, -1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
			// Left face (Top right) - Vertices, Color(green), Normals, TexCoords
			-1.0f, 1.0f, 1.0f,		0.0f, 1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
			
			// Back face (Top left) - Vertices, Color(red), Normals, TexCoords
			1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
			// Back face (Bottom left) - Vertices, Color(red), Normals, TexCoords
			1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
			// Back face (Bottom right) - Vertices, Color(red), Normals, TexCoords
			-1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
			// Back face (Top right) - Vertices, Color(red), Normals, TexCoords
			-1.0f, 1.0f, -1.0f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
			
			// Top face (Top left) - Vertices, Color(blue), Normals, TexCoords
			1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
			// Top face (Bottom left) - Vertices, Color(blue), Normals, TexCoords
			1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
			// Top face (Bottom right) - Vertices, Color(blue), Normals, TexCoords
			-1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
			// Top face (Top right) - Vertices, Color(blue), Normals, TexCoords
			-1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		};

		GLES32.glGenVertexArrays(1, VAObj, 0);
		GLES32.glBindVertexArray(VAObj[0]);		// For Cube
			GLES32.glGenBuffers(1, VBObj, 0);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, VBObj[0]);
			ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cube.length * 4);
			byteBuffer.order(ByteOrder.nativeOrder());
			FloatBuffer data = byteBuffer.asFloatBuffer();
			data.put(cube);
			data.position(0);
			GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube.length * 4, data, GLES32.GL_STATIC_DRAW);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_VERTEX, 3, GLES32.GL_FLOAT, false, (3+3+3+2)*4, 0*4);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_VERTEX);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_COLOR, 3, GLES32.GL_FLOAT, false, (3+3+3+2)*4, 3*4);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_COLOR);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_NORMAL, 3, GLES32.GL_FLOAT, false, (3+3+3+2)*4, 6*4);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_NORMAL);
			GLES32.glVertexAttribPointer(GLESMacros.DV_ATTRIB_TEXTURE, 2, GLES32.GL_FLOAT, false, (3+3+3+2)*4, 9*4);
			GLES32.glEnableVertexAttribArray(GLESMacros.DV_ATTRIB_TEXTURE);
			GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		GLES32.glBindVertexArray(0);

		// Enable depth
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		// Enabling Texture
		GLES32.glEnable(GLES32.GL_TEXTURE_2D);
		marble_texture[0] = loadGLTexture(R.raw.marble);

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

	private int loadGLTexture(int imgSrcID) {
		// Variable declaration
		int texture[] = new int [1];
		BitmapFactory.Options option = new BitmapFactory.Options();

		// Code
		option.inScaled = false;
		Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imgSrcID, option);

		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
		GLES32.glGenTextures(1, texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);
		GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
		GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

		return texture[0];
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

		// For pyramid
		float[] MMatrix = new float[16];
		float[] VMatrix = new float[16];
		float[] PMatrix = new float[16];
		float[] translation_matrix = new float[16];
		float[] rotation_matrix = new float[16];

		Matrix.setIdentityM(MMatrix, 0);
		Matrix.setIdentityM(VMatrix, 0);
		PMatrix = perspProjMatrix;
		Matrix.setIdentityM(translation_matrix, 0);
		Matrix.setIdentityM(rotation_matrix, 0);

		Matrix.translateM(translation_matrix, 0, 0.0f, 0.0f, -5.0f);
		Matrix.multiplyMM(MMatrix, 0, MMatrix, 0, translation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 1.0f, 0.0f, 0.0f);
		Matrix.multiplyMM(MMatrix, 0, MMatrix, 0, rotation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 1.0f, 0.0f);
		Matrix.multiplyMM(MMatrix, 0, MMatrix, 0, rotation_matrix, 0);
		Matrix.rotateM(rotation_matrix, 0, gfAngle, 0.0f, 0.0f, 1.0f);
		Matrix.multiplyMM(MMatrix, 0, MMatrix, 0, rotation_matrix, 0);

		GLES32.glUniformMatrix4fv(MMatrixUniform, 1, false, MMatrix, 0);
		GLES32.glUniformMatrix4fv(VMatrixUniform, 1, false, VMatrix, 0);
		GLES32.glUniformMatrix4fv(PMatrixUniform, 1, false, PMatrix, 0);

		if(enableInterleaved) {
			GLES32.glUniform3f(LAmbUniform, 0.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(LDiffUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform3f(LSpecUniform, 0.5f, 0.5f, 0.5f);
			GLES32.glUniform4f(LPosUniform, 0.0f, 0.0f, 2.0f, 1.0f);
			GLES32.glUniform3f(KAmbUniform, 0.0f, 0.0f, 0.0f);
			GLES32.glUniform3f(KDiffUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform3f(KSpecUniform, 1.0f, 1.0f, 1.0f);
			GLES32.glUniform1f(KShineUniform, 50.0f);
			GLES32.glUniform1i(InterleavedUniform, 1);
		}
		else
			GLES32.glUniform1i(InterleavedUniform, 0);

		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, marble_texture[0]);
		GLES32.glUniform1i(TexSampUniform, 0);

		GLES32.glBindVertexArray(VAObj[0]);
		for(int i = 0; i < 24; i += 4)
			GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, i, 4);
		GLES32.glBindVertexArray(0);

		GLES32.glUseProgram(0);

		if(enableAnimation)
			gfAngle += 0.5f;
		if(gfAngle >= 360.0f)
			gfAngle = 0.0f;

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

		// Delete textures
		if(marble_texture[0] != 0) {
			GLES32.glDeleteTextures(1, marble_texture, 0);
			marble_texture[0] = 0;
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
