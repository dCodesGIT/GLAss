// Program to show 3D shapes - rotating Pyramid with 2 steady lights
// By : Darshan Vikam
// Date : 04 April 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include "../Include/vmath.h"
#include "../Icon/WinIcon.h"

// Library linking
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

// Call back function - WndProc()
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global macro definitions
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Namespace declaration
using namespace vmath;

// Global enum declaration
enum {
	DV_ATTRIBUTE_POSITION = 0,
	DV_ATTRIBUTE_COLOR,
	DV_ATTRIBUTE_NORMAL,
	DV_ATTRIBUTE_TEXTURE0,
};

// Global variable declaration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE *gfp = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbFullscreen = false;
bool gbEnableAnimation = false;
bool gbEnableLight = false;
GLfloat gGLfangle = 0.0f;

GLuint gVSObj;		// Vertex Shader Object
GLuint gFSObj;		// Fragment Shader Object
GLuint gSPObj;		// Shader Program Object
GLuint gVAObj_Pyramid;		// Vertex Array Object 
GLuint gVBObj_Pyramid_Pos;	// Buffer Object for Position
GLuint gVBObj_Pyramid_Norm;	// Buffer Object for Normal

GLuint gMMatrixUniform;		// Model Matrix of ModelViewProjection
GLuint gVMatrixUniform;		// View Matrix of ModelViewProjection
GLuint gPMatrixUniform;		// Persepctive projection Matrix of ModelViewProjection

// Light related
GLuint gLightAmbientUniform;		// Uniform for Ambient component of light
GLuint gLightDiffuseUniform;		// Uniform for Diffused component of light
GLuint gLightSpecularUniform;		// Uniform for Specular component of light
GLuint gLightPositionUniform;		// Uniform for Position of light
GLuint gMaterialAmbientUniform;		// Uniform for Ambient component of material
GLuint gMaterialDiffuseUniform;		// Uniform for Diffused component of material
GLuint gMaterialSpecularUniform;	// Uniform for Specular component of material
GLuint gMaterialShininessUniform;	// Uniform for shininess of material
GLuint gKeyPressedUniform;	// Uniform for key press (for light)

mat4 gPerspectiveProjectionMatrix;	// Matrix of 4x4

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declarations
	void Initialize(void);
	void Display(void);
	void Update(void);
	void Uninitialize(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("PyramidWith2LightsInPP");
	bool bDone = false;

	// Code
	if(fopen_s(&gfp, "Log.txt", "w") != 0) {
		MessageBox(NULL, TEXT("Log.txt file cannot be created.\n Exitting..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
		fprintf(gfp, "Log file created successfully...\n");

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Pyramid with 2 Lights"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	Initialize();

	while(bDone == false) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT)
				bDone = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			Display();
			Update();
		}
	}

	return((int)msg.wParam);
}

// Call back function definition - WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Function declaration
	void Resize(int, int);
	void ToggleFullscreen(void);
	void Uninitialize(void);

	// Code
	switch(iMsg) {
		case WM_ACTIVATE :
			if(HIWORD(wParam) == 0)
				gbActiveWindow = true;
			else
				gbActiveWindow = false;
			break;

		case WM_ERASEBKGND :
			return(0);

		case WM_SIZE :
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_KEYDOWN :
			switch(wParam) {
				case VK_ESCAPE :
					if(MessageBox(ghwnd, TEXT("Do you want really want to quit?"), TEXT("Quit"), MB_YESNO) == IDYES)
						DestroyWindow(ghwnd);
					break;
				case 0x46 :
				case 0x66 :
					ToggleFullscreen();
					if(gbFullscreen == false)
						gbFullscreen = true;
					else
						gbFullscreen = false;
					break;
				default :
					break;
			}
			break;

		case WM_CHAR :
			switch(wParam) {
				case 'a' :
				case 'A' :
					gbEnableAnimation = !gbEnableAnimation;
					break;
				case 'l' :
				case 'L' :
					gbEnableLight = !gbEnableLight;
					break;
				default :
					break;
			}
			break;

		case WM_CLOSE :
			if(MessageBox(ghwnd, TEXT("Do you want really want to quit?"), TEXT("Quit"), MB_YESNO) == IDYES)
				DestroyWindow(ghwnd);
			else
				return true;
			break;

		case WM_DESTROY :
			Uninitialize();
			PostQuitMessage(0);
			break;

		default :
			break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

// User defined function definitions
void ToggleFullscreen(void) {
	// Variable declaration
	MONITORINFO mi;

	// Code
	if(gbFullscreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW) {
			mi = {sizeof(MONITORINFO)};
			if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void Initialize(void) {
	// Function declaration
	void Uninitialize(void);
	void Resize(int, int);
	void ShaderErrorCheck(GLuint, char*);			// ShaderErrorCheck(GLuint shader_object, char* ["COMPILE"/"LINK"]);

	// Variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	FILE *fp_OGL_info = NULL;

	// Code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0)
		DestroyWindow(ghwnd);

	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
		DestroyWindow(ghwnd);

	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL)
		DestroyWindow(ghwnd);

	if(wglMakeCurrent(ghdc, ghrc) == false)
		DestroyWindow(ghwnd);

	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
		DestroyWindow(ghwnd);

	// OpenGL related log entry
	fopen_s(&fp_OGL_info, "OpenGL_info.txt", "w");
	if(fp_OGL_info == NULL)
		MessageBox(ghwnd, TEXT("Unable to open file to write OpenGL related information"), NULL, NULL);
	fprintf(fp_OGL_info, "*** OpenGL Information ***\n\n");
	fprintf(fp_OGL_info, "*** OpenGL related basic information ***\n");
	fprintf(fp_OGL_info, "OpenGL Vendor Company : %s\n", glGetString(GL_VENDOR));
	fprintf(fp_OGL_info, "OpenGL Renderer(Graphics card company) : %s\n", glGetString(GL_RENDERER));
	fprintf(fp_OGL_info, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(fp_OGL_info, "Graphics Library Shading Language(GLSL) Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(fp_OGL_info, "*** OpenGL supported/related extentions ***\n");
	// OpenGL supported/related Extensions
	GLint numExts;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
	for(int i = 0; i < numExts; i++)
		fprintf(fp_OGL_info, "%d. %s\n", i+1, glGetStringi(GL_EXTENSIONS, i));
	fclose(fp_OGL_info);
	fp_OGL_info = NULL;

	// Vertex Shader
	gVSObj = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode = 				// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_MMatrix;" \
		"uniform mat4 u_VMatrix;" \
		"uniform mat4 u_PMatrix;" \
		"uniform int u_iKeyPressed;" \
		"uniform vec3 u_LAmb[2];" \
		"uniform vec3 u_LDiff[2];" \
		"uniform vec3 u_LSpec[2];" \
		"uniform vec4 u_LPos[2];" \
		"uniform vec3 u_KAmb;" \
		"uniform vec3 u_KDiff;" \
		"uniform vec3 u_KSpec;" \
		"uniform float u_KShine;" \
		"out vec3 ADS_light;" \
		"void main(void) {" \
			"if(u_iKeyPressed == 1) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"vec3 transformedNormal = normalize(mat3(u_VMatrix * u_MMatrix) * vNormal);" \
				"vec3 lightSource[2];" \
				"vec3 reflectionVector[2];" \
				"vec3 viewVector = normalize(-eyeCoords.xyz);" \
				"vec3 ambient[2];" \
				"vec3 diffuse[2];" \
				"vec3 specular[2];" \
				"ADS_light = vec3(0.0f, 0.0f, 0.0f);" \
				"for(int i = 0; i < 2; i++) {" \
					"ambient[i] = u_LAmb[i] * u_KAmb;" \
					"lightSource[i] = normalize(vec3(u_LPos[i] - eyeCoords));" \
					"diffuse[i] = u_LDiff[i] * u_KDiff * max(dot(lightSource[i], transformedNormal), 0.0f);" \
					"reflectionVector[i] = reflect(-lightSource[i], transformedNormal);" \
					"specular[i] = u_LSpec[i] * u_KSpec * pow(max(dot(reflectionVector[i], viewVector), 0.0f), u_KShine);" \
					"ADS_light += (ambient[i] + diffuse[i] + specular[i]);" \
				"}" \
			"}" \
			"else {" \
				"ADS_light = vec3(1.0f, 1.0f, 1.0f);" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);				// Compile Shader
	ShaderErrorCheck(gVSObj, "COMPILE");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec3 ADS_light;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = vec4(ADS_light, 1.0f);" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);				// Compile Shader
	ShaderErrorCheck(gFSObj, "COMPILE");

	// Shader program
	gSPObj = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj, gVSObj);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj, gFSObj);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_POSITION, "vPosition");	// Binding of shader program with enum declared - for position(coordinates)
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_NORMAL, "vNormal");		// Binding of shader program with enum declared - for normal
	glLinkProgram(gSPObj);
	ShaderErrorCheck(gSPObj, "LINK");

	// get uniform locations
	gMMatrixUniform = glGetUniformLocation(gSPObj, "u_MMatrix");
	gVMatrixUniform = glGetUniformLocation(gSPObj, "u_VMatrix");
	gPMatrixUniform = glGetUniformLocation(gSPObj, "u_PMatrix");
	gLightAmbientUniform = glGetUniformLocation(gSPObj, "u_LAmb");
	gLightDiffuseUniform = glGetUniformLocation(gSPObj, "u_LDiff");
	gLightSpecularUniform = glGetUniformLocation(gSPObj, "u_LSpec");
	gLightPositionUniform = glGetUniformLocation(gSPObj, "u_LPos");
	gMaterialAmbientUniform = glGetUniformLocation(gSPObj, "u_KAmb");
	gMaterialDiffuseUniform = glGetUniformLocation(gSPObj, "u_KDiff");
	gMaterialSpecularUniform = glGetUniformLocation(gSPObj, "u_KSpec");
	gMaterialShininessUniform = glGetUniformLocation(gSPObj, "u_KShine");
	gKeyPressedUniform = glGetUniformLocation(gSPObj, "u_iKeyPressed");

	// other variable initialization
	const GLfloat PyramidVertex[] = {
		// Front face
		0.0f, 2.0f, 0.0f,	// Apex
		-1.0f, -1.0f, 1.0f,	// left bottom
		1.0f, -1.0f, 1.0f,	// right bottom

		// Right face
		0.0f, 2.0f, 0.0f,	// Apex
		1.0f, -1.0f, 1.0f,	// left bottom
		1.0f, -1.0f, -1.0f,	// right bottom

		// Back face
		0.0f, 2.0f, 0.0f,	// Apex
		1.0f, -1.0f, -1.0f,	// left bottom
		-1.0f, -1.0f, -1.0f,	// right bottom

		// Left face
		0.0f, 2.0f, 0.0f,	// Apex
		-1.0f, -1.0f, -1.0f,	// left bottom
		-1.0f, -1.0f, 1.0f	// right bottom
	};
	const GLfloat PyramidNormal[] = {
		// Front face
		0.0f, 0.447214f, 0.894427f,	// Top
		0.0f, 0.447214f, 0.894427f,	// Left bottom
		0.0f, 0.447214f, 0.894427f,	// Right bottom

		// Right face
		0.894427f, 0.447214f, 0.0f,	// Top
		0.894427f, 0.447214f, 0.0f,	// Left bottom
		0.894427f, 0.447214f, 0.0f,	// Right bottom

		// Back face
		0.0f, 0.447214f, -0.894427f,	// Top
		0.0f, 0.447214f, -0.894427f,	// Left bottom
		0.0f, 0.447214f, -0.894427f,	// Right bottom

		// Left face
		-0.894427f, 0.447214f, 0.0f,	// Top
		-0.894427f, 0.447214f, 0.0f,	// Left bottom
		-0.894427f, 0.447214f, 0.0f	// Right bottom
	};

	// For Pyramid
	glGenVertexArrays(1, &gVAObj_Pyramid);
	glBindVertexArray(gVAObj_Pyramid);		// Bind following steps into gVAObj variable
		// vPosition of Pyramid
		glGenBuffers(1, &gVBObj_Pyramid_Pos);		// Generate buffer
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Pyramid_Pos);	// Binding buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidVertex), PyramidVertex, GL_STATIC_DRAW);	// Placing values in OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);		// Formatting of data entered into OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);	// Enabling of OpenGL's buffer(GL_VERTEX_BUFFER)
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vNormal of Pyramid
		glGenBuffers(1, &gVBObj_Pyramid_Norm);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Pyramid_Norm);
		glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidNormal), PyramidNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_NORMAL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);			// unbind and save above steps into gVAObj

	// Depth related OpenGL code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	gPerspectiveProjectionMatrix = mat4::identity();

	// Warm up call to Resize()
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void ShaderErrorCheck(GLuint shaderObject, char *shaderOpr) {	// Error checking after shader compilation
	// Variable declaration
	GLint iErrorLen = 0;
	GLint iShaderStatus = 0;
	char *szError = NULL;

	// Code
	if(strcmp(shaderOpr, "COMPILE") == 0)
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &iShaderStatus);
	else if(strcmp(shaderOpr, "LINK") == 0)
		glGetShaderiv(shaderObject, GL_LINK_STATUS, &iShaderStatus);
	else {
		fprintf(gfp, "Invalid second parameter in ShaderErrorCheck()");
		return;
	}
	if(iShaderStatus == GL_FALSE) {
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &iErrorLen);
		if(iErrorLen > 0) {
			szError = (char *)malloc(iErrorLen);
			if(szError != NULL) {
				GLsizei written;
				glGetShaderInfoLog(shaderObject, iErrorLen, &written, szError);
				if(strcmp(shaderOpr, "COMPILE") == 0)
					fprintf(gfp, "Shader Compilation Error log : \n");
				else if(strcmp(shaderOpr, "LINK") == 0)
					fprintf(gfp, "Shader linking Error log : \n");
				fprintf(gfp, "%s \n", szError);
				free(szError);
				szError = NULL;
				DestroyWindow(ghwnd);
			}
		}
	}
}

void Resize(int width, int height) {
	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void Display(void) {
	// Variable declaration
	mat4 modelMatrix, viewMatrix, projectionMatrix;
	mat4 translateMatrix, rotationMatrix;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Starting of OpenGL shading program
	glUseProgram(gSPObj);

	// For Pyramid
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	if(gbEnableLight == true) {
		GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f,
			0.25f, 0.25f, 0.25f };
		GLfloat lightDiffuse[] = { 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f };
		GLfloat lightSpecular[] = { 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f };
		GLfloat lightPosition[] = { 2.0f, 0.0f, 0.0f, 1.0f,
			-2.0f, 0.0f, 0.0f, 1.0f };

		glUniform1i(gKeyPressedUniform, 1);
		
		glUniform3fv(gLightAmbientUniform, 2, lightAmbient);
		glUniform3fv(gLightDiffuseUniform, 2, lightDiffuse);
		glUniform3fv(gLightSpecularUniform, 2, lightSpecular);
		glUniform4fv(gLightPositionUniform, 2, lightPosition);

		glUniform3f(gMaterialAmbientUniform, 0.0f, 0.0f, 0.0f);
		glUniform3f(gMaterialDiffuseUniform, 1.0f, 1.0f, 1.0f);
		glUniform3f(gMaterialSpecularUniform, 1.0f, 1.0f, 1.0f);
		glUniform1f(gMaterialShininessUniform, 50);
	}
	else
		glUniform1i(gKeyPressedUniform, 0);

	translateMatrix = translate(0.0f, 0.0f, -5.0f);
	modelMatrix *= translateMatrix;
	rotationMatrix = rotate(gGLfangle, 0.0f, 1.0f, 0.0f);
	modelMatrix *= rotationMatrix;
	projectionMatrix = gPerspectiveProjectionMatrix;

	glUniformMatrix4fv(gMMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gVMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gPMatrixUniform, 1, GL_FALSE, projectionMatrix);

	// OpenGL Drawing
	glBindVertexArray(gVAObj_Pyramid);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
	if(gbEnableAnimation == true)
		gGLfangle += 0.02f;
	if(gGLfangle >= 360.0f)
		gGLfangle = 0.0f;
}

void Uninitialize(void) {
	// Code
	if(gbFullscreen == true)
		ToggleFullscreen();

	// Destroy Vertex Array Object
	if(gVAObj_Pyramid) {
		glDeleteVertexArrays(1, &gVAObj_Pyramid);
		gVAObj_Pyramid = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_Pyramid_Pos) {
		glDeleteBuffers(1, &gVBObj_Pyramid_Pos);
		gVBObj_Pyramid_Pos = 0;
	}
	if(gVBObj_Pyramid_Norm) {
		glDeleteBuffers(1, &gVBObj_Pyramid_Norm);
		gVBObj_Pyramid_Norm = 0;
	}

	// Detach shaders
	glDetachShader(gSPObj, gVSObj);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj, gFSObj);		// Detach fragment shader from final shader program

	// Delete shaders
	if(gVSObj != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj);
		gVSObj = 0;
	}
	if(gFSObj != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj);
		gFSObj = 0;
	}
	if(gSPObj != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj);
		gSPObj = 0;
	}

	// Unlink shader program
	glUseProgram(0);

	wglMakeCurrent(NULL, NULL);

	if(ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if(ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if(gfp) {
		fprintf(gfp, "Closing Log file successfully..\n");
		fclose(gfp);
		gfp = NULL;
	}
}
