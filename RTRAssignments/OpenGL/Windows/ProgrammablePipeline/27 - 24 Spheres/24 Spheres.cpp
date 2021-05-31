// Program to show 24 different Sphere with different materials and with Per fragment lighting
// By : Darshan Vikam
// Date : 10 April 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include "../Include/vmath.h"
#include "../Icon/WinIcon.h"
#include "Sphere.h"

// Library linking
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "Sphere.lib")

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
bool gbEnableLighting = false;
bool gbXRotationEnabled = false;
bool gbYRotationEnabled = false;
bool gbZRotationEnabled = false;
GLfloat gGLfAngle = 0.0f;
int gWidth, gHeight;

GLfloat sphere_Vertices[1146];
GLfloat sphere_Normals[1146];
GLfloat sphere_Textures[764];
unsigned short sphere_Elements[2280];
GLuint gNumVertices, gNumElements;

GLuint gVSObj;		// Vertex Shader Object
GLuint gFSObj;		// Fragment Shader Object
GLuint gSPObj;		// Shader Program Object
GLuint gVAObj_Sphere;		// Vertex Array Object 
GLuint gVBObj_pos_Sphere;	// Fragment Buffer Object for position
GLuint gVBObj_norm_Sphere;	// Fragment Buffer Object for normals
GLuint gVBObj_elem_Sphere;	// Fragment Buffer Object for elements

GLuint gMMatrixUniform;		// Model Matrix of ModelViewProjection
GLuint gVMatrixUniform;		// View Matrix of ModelViewProjection
GLuint gPMatrixUniform;		// Persepctive projection Matrix of ModelViewProjection

// Light related
GLuint gLightAmbientUniform;	// Uniform for Ambient component of light
GLuint gLightDiffuseUniform;	// Uniform for Diffused component of light
GLuint gLightSpecularUniform;	// Uniform for Specular component of light
GLuint gLightPositionUniform;	// Uniform for Position of light
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
	TCHAR szAppName[] = TEXT("24SpheresInPP");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("24 Spheres"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
				case 'l' :
				case 'L' :
					gbEnableLighting = !gbEnableLighting;
					break;
				case 'x' :
				case 'X' :
					gbXRotationEnabled = true;
					gbYRotationEnabled = false;
					gbZRotationEnabled = false;
					gGLfAngle = 0.0f;
					break;
				case 'y' :
				case 'Y' :
					gbXRotationEnabled = false;
					gbYRotationEnabled = true;
					gbZRotationEnabled = false;
					gGLfAngle = 0.0f;
					break;
				case 'z' :
				case 'Z' :
					gbXRotationEnabled = false;
					gbYRotationEnabled = false;
					gbZRotationEnabled = true;
					gGLfAngle = 0.0f;
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
	void ShaderErrorCheck(GLuint, char*);		// ShaderErrorCheck(GLuint shader_object, char* ["COMPILE"/"LINK"]);

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
		"uniform vec4 u_LPos;" \
		"out vec3 transformedNormal;" \
		"out vec3 lightSource;" \
		"out vec3 viewVector;" \
		"void main(void) {" \
			"if(u_iKeyPressed == 1) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"transformedNormal = mat3(u_VMatrix * u_MMatrix) * vNormal;" \
				"lightSource = vec3(u_LPos - eyeCoords);" \
				"viewVector = -eyeCoords.xyz;" \
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
		"in vec3 transformedNormal;" \
		"in vec3 lightSource;" \
		"in vec3 viewVector;" \
		"uniform int u_iKeyPressed;" \
		"uniform vec3 u_LAmb;" \
		"uniform vec3 u_LDiff;" \
		"uniform vec3 u_LSpec;" \
		"uniform vec3 u_KAmb;" \
		"uniform vec3 u_KDiff;" \
		"uniform vec3 u_KSpec;" \
		"uniform float u_KShine;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"vec3 lightColor;" \
			"if(u_iKeyPressed == 1) {" \
				"vec3 normalized_tNorm = normalize(transformedNormal);" \
				"vec3 normalized_lSrc = normalize(lightSource);" \
				"vec3 normalized_viewVec = normalize(viewVector);" \
				"vec3 reflectionVector = reflect(-normalized_lSrc, normalized_tNorm);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(normalized_lSrc, normalized_tNorm), 0.0f);" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, normalized_viewVec), 0.0f), u_KShine);" \
				"lightColor = ambient + diffuse + specular;" \
			"}" \
			"else {" \
				"lightColor = vec3(0.0f, 0.0f, 0.0f);" \
			"}" \
			"FragColor = vec4(lightColor, 1.0f);" \
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
	getSphereVertexData(sphere_Vertices, sphere_Normals, sphere_Textures, sphere_Elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// For Sphere
	glGenVertexArrays(1, &gVAObj_Sphere);
	glBindVertexArray(gVAObj_Sphere);		// Bind following steps into gVAObj variable
		// vPosition VBObj
		glGenBuffers(1, &gVBObj_pos_Sphere);		// Generate buffer
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_pos_Sphere);	// Binding buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_Vertices), sphere_Vertices, GL_STATIC_DRAW);	// Placing values in OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);		// Formatting of data entered into OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);	// Enabling of OpenGL's buffer(GL_VERTEX_BUFFER)
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vNormal VBObj
		glGenBuffers(1, &gVBObj_norm_Sphere);		// Generate buffer
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_norm_Sphere);	// Binding buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_Normals), sphere_Normals, GL_STATIC_DRAW);	// Placing values in OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glVertexAttribPointer(DV_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);		// Formatting of data entered into OpenGL's shader buffer(GL_VERTEX_BUFFER)
		glEnableVertexAttribArray(DV_ATTRIBUTE_NORMAL);	// Enabling of OpenGL's buffer(GL_VERTEX_BUFFER)
		glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbinding buffer - GL_ARRAY_BUFFER

		// Element VBObj
		glGenBuffers(1, &gVBObj_elem_Sphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVBObj_elem_Sphere);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_Elements), sphere_Elements, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
//	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gWidth = width;
	gHeight = height;

	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void Display(void) {
	// Variable declaration
	mat4 modelMatrix, viewMatrix, projectionMatrix;
	mat4 translateMatrix;
	GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f };
	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f };
	GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f };
//	GLfloat lightSpecular[] = { 0.25f, 0.25f, 0.25f };
	GLfloat lightPosition[4];
	GLfloat materialAmbient[][4] =
	{
		{0.0215f, 0.1745f, 0.0215f, 1.0f},	// 1R 1C - Emerald
		{0.135f, 0.2225f, 0.1575f, 1.0f},	// 2R 1C - Jade
		{0.05375f, 0.05f, 0.06625f, 1.0f},	// 3R 1C - Obsidian
		{0.25f, 0.20725f, 0.20725f, 1.0f},	// 4R 1C - Pearl
		{0.1745f, 0.01175f, 0.01175f, 1.0f},	// 5R 1C - Ruby
		{0.1f, 0.18725f, 0.1745f, 1.0f},	// 6R 1C - Turquoise
		{0.329412f, 0.223529f, 0.027451f, 1.0f},// 1R 2C - Brass
		{0.2125f, 0.1275f, 0.054f, 1.0f},	// 2R 2C - Bronze
		{0.25f, 0.25f, 0.25f, 1.0f},		// 3R 2C - Chrome
		{0.19125f, 0.0735f, 0.0225f, 1.0f},	// 4R 2C - Copper
		{0.24725f, 0.1995f, 0.0745f, 1.0f},	// 5R 2C - Gold
		{0.19225f, 0.19225f, 0.19225f, 1.0f},	// 6R 2C - Silver
		{0.0f, 0.0f, 0.0f, 1.0f},		// 1R 3C - Black plastic
		{0.0f, 0.1f, 0.06f, 1.0f},		// 2R 3C - Cyan plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 3R 3C - Green plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 4R 3C - Red plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 5R 3C - White plastic
		{0.0f, 0.0f, 0.0f, 1.0f},		// 6R 3C - Yellow plastic
		{0.02f, 0.02f, 0.02f, 1.0f},		// 1R 4C - Black rubber
		{0.0f, 0.05f, 0.05f, 1.0f},		// 2R 4C - Cyan rubber
		{0.0f, 0.05f, 0.0f, 1.0f},		// 3R 4C - Green rubber
		{0.05f, 0.0f, 0.0f, 1.0f},		// 4R 4C - Red rubber
		{0.05f, 0.05f, 0.05f, 1.0f},		// 5R 4C - White rubber
		{0.05f, 0.05f, 0.04f, 1.0f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialDiffuse[][4] =
	{
		{0.07568f, 0.61424f, 0.07568f, 1.0f},	// 1R 1C - Emerald
		{0.54f, 0.89f, 0.63f, 1.0f},		// 2R 1C - Jade
		{0.18275f, 0.17f, 0.22525f, 1.0f},	// 3R 1C - Obsidian
		{1.0f, 0.829f, 0.829f, 1.0f},		// 4R 1C - Pearl
		{0.61424f, 0.04136f, 0.04163f, 1.0f},	// 5R 1C - Ruby
		{0.396f, 0.74151f, 0.69102f, 1.0f},	// 6R 1C - Turquoise
		{0.780392f, 0.568627f, 0.113725f, 1.0f},// 1R 2C - Brass
		{0.714f, 0.4284f, 0.18144f, 1.0f},	// 2R 2C - Bronze
		{0.4f, 0.4f, 0.4f, 1.0f},		// 3R 2C - Chrome
		{0.7038f, 0.27048f, 0.0828f, 1.0f},	// 4R 2C - Copper
		{0.75164f, 0.60648f, 0.22648f, 1.0f},	// 5R 2C - Gold
		{0.50754f, 0.50754f, 0.50754f, 1.0f},	// 6R 2C - Silver
		{0.01f, 0.01f, 0.01f, 1.0f},		// 1R 3C - Black plastic
		{0.0f, 0.50980392f, 0.50980392f, 1.0f},	// 2R 3C - Cyan plastic
		{0.1f, 0.35f, 0.1f, 1.0f},		// 3R 3C - Green plastic
		{0.5f, 0.0f, 0.0f, 1.0f},		// 4R 3C - Red plastic
		{0.55f, 0.55f, 0.55f, 1.0f},		// 5R 3C - White plastic
		{0.5f, 0.5f, 0.0f, 1.0f},		// 6R 3C - Yellow plastic
		{0.01f, 0.01f, 0.01f, 1.0f},		// 1R 4C - Black rubber
		{0.4f, 0.5f, 0.5f, 1.0f},		// 2R 4C - Cyan rubber
		{0.4f, 0.5f, 0.4f, 1.0f},		// 3R 4C - Green rubber
		{0.5f, 0.4f, 0.4f, 1.0f},		// 4R 4C - Red rubber
		{0.5f, 0.5f, 0.5, 1.0f},		// 5R 4C - White rubber
		{0.5f, 0.5f, 0.4f, 1.0f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialSpecular[][4] =
	{
		{0.633f, 0.727811f, 0.33f, 1.0f},		// 1R 1C - Emerald
		{0.316228f, 0.316228f, 0.316228f, 1.0f},	// 2R 1C - Jade
		{0.332741f, 0.328634f, 0.346435f, 1.0f},	// 3R 1C - Obsidian
		{0.296648f, 0.296648f, 0.296648f, 1.0f},	// 4R 1C - Pearl
		{0.727811f, 0.626959f, 0.626959f, 1.0f},	// 5R 1C - Ruby
		{0.297254f, 0.308290f, 0.306678f, 1.0f},	// 6R 1C - Turquoise
		{0.992157f, 0.941176f, 0.807843f, 1.0f},	// 1R 2C - Brass
		{0.393548f, 0.271906f, 0.166721f, 1.0f},	// 2R 2C - Bronze
		{0.774597f, 0.774597f, 0.774597f, 1.0f},	// 3R 2C - Chrome
		{0.256777f, 0.137622f, 0.086014f, 1.0f},	// 4R 2C - Copper
		{0.628281f, 0.555802f, 0.366065f, 1.0f},	// 5R 2C - Gold
		{0.508273f, 0.508273f, 0.508273f, 1.0f},	// 6R 2C - Silver
		{0.5f, 0.5f, 0.5f, 1.0f},			// 1R 3C - Black plastic
		{0.50196078f, 0.50196078f, 0.50196078f, 1.0f},	// 2R 3C - Cyan plastic
		{0.45f, 0.55f, 0.45f, 1.0f},		// 3R 3C - Green plastic
		{0.7f, 0.6f, 0.6f, 1.0f},		// 4R 3C - Red plastic
		{0.7f, 0.7f, 0.7f, 1.0f},		// 5R 3C - White plastic
		{0.6f, 0.6f, 0.5f, 1.0f},		// 6R 3C - Yellow plastic
		{0.4f, 0.4f, 0.4f, 1.0f},		// 1R 4C - Black rubber
		{0.04f, 0.7f, 0.7f, 1.0f},		// 2R 4C - Cyan rubber
		{0.04f, 0.7f, 0.04f, 1.0f},		// 3R 4C - Green rubber
		{0.7f, 0.04f, 0.04f, 1.0f},		// 4R 4C - Red rubber
		{0.7f, 0.7f, 0.7f, 1.0f},		// 5R 4C - White rubber
		{0.7f, 0.7f, 0.04f, 1.0f}		// 6R 4C - Yellow rubber
	};
	GLfloat materialShininess[] =
	{	0.6f,		// 1R 1C - Emerald
		0.1f,		// 2R 1C - Jade
		0.3f,		// 3R 1C - Obsidian
		0.088f,		// 4R 1C - Pearl
		0.6f,		// 5R 1C - Ruby
		0.1f,		// 6R 1C - Turquoise
		0.21794872f,	// 1R 2C - Brass
		0.2f,		// 2R 2C - Bronze
		0.6f,		// 3R 2C - Chrome
		0.1f,		// 4R 2C - Copper
		0.4f,		// 5R 2C - Gold
		0.4f,		// 6R 2C - Silver
		0.25f,		// 1R 3C - Black plastic
		0.25f,		// 2R 3C - Cyan plastic
		0.25f,		// 3R 3C - Green plastic
		0.25f,		// 4R 3C - Red plastic
		0.25f,		// 5R 3C - White plastic
		0.25f,		// 6R 3C - Yellow plastic
		0.078125f,	// 1R 4C - Black rubber
		0.078125f,	// 2R 4C - Cyan rubber
		0.078125f,	// 3R 4C - Green rubber
		0.078125f,	// 4R 4C - Red rubber
		0.078125f,	// 5R 4C - White rubber
		0.078125f	// 6R 4C - Yellow rubber
	};
	GLfloat radian = M_PI / 180.0f;
	GLfloat radius = 100.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 6; j++) {
			// Starting of OpenGL shading program
			glUseProgram(gSPObj);

			glViewport((gWidth / 4) * i, (gHeight / 6) * (5-j), (GLsizei)(gWidth/4), (GLsizei)(gHeight/6));

			modelMatrix = mat4::identity();
			viewMatrix = mat4::identity();
			projectionMatrix = mat4::identity();
			translateMatrix = mat4::identity();

			if(gbXRotationEnabled == true) {
				lightPosition[0] = 0.0f; 
				lightPosition[1] = radius * (GLfloat)cos(gGLfAngle * radian);
				lightPosition[2] = radius * (GLfloat)sin(gGLfAngle * radian);
			}
			else if(gbYRotationEnabled == true) {
				lightPosition[0] = radius * (GLfloat)sin(gGLfAngle * radian);
				lightPosition[1] = 0.0f;
				lightPosition[2] = radius * (GLfloat)cos(gGLfAngle * radian);
			}
			else if(gbZRotationEnabled == true) {
				lightPosition[0] = radius * (GLfloat)cos(gGLfAngle * radian);
				lightPosition[1] = radius * (GLfloat)sin(gGLfAngle * radian);
				lightPosition[2] = 0.0f;
			}
			else {
				lightPosition[0] = 10.0f;
				lightPosition[1] = 10.0f;
				lightPosition[2] = 10.0f;
				lightPosition[3] = 1.0f;
			}

			if(materialShininess[(i*6)+j] < 1.0f)
				materialShininess[(i*6)+j] *= 128.0f;
			if(gbEnableLighting == true) {
				glUniform1i(gKeyPressedUniform, 1);
				glUniform3fv(gLightAmbientUniform, 1, lightAmbient);
				glUniform3fv(gLightDiffuseUniform, 1, lightDiffuse);
				glUniform3fv(gLightSpecularUniform, 1, lightSpecular);
				glUniform4fv(gLightPositionUniform, 1, lightPosition);
				glUniform3fv(gMaterialAmbientUniform, 1, materialAmbient[(i*6)+j]);
				glUniform3fv(gMaterialDiffuseUniform, 1, materialDiffuse[(i*6)+j]);
				glUniform3fv(gMaterialSpecularUniform, 1, materialSpecular[(i*6)+j]);
				glUniform1fv(gMaterialShininessUniform, 1, &materialShininess[(i*6)+j]);
			}
			else
				glUniform1i(gKeyPressedUniform, 0);

			translateMatrix = translate(0.0f, 0.0f, -2.5f);
			modelMatrix = translateMatrix;
			projectionMatrix = gPerspectiveProjectionMatrix;

			glUniformMatrix4fv(gMMatrixUniform, 1, GL_FALSE, modelMatrix);
			glUniformMatrix4fv(gVMatrixUniform, 1, GL_FALSE, viewMatrix);
			glUniformMatrix4fv(gPMatrixUniform, 1, GL_FALSE, projectionMatrix);

			// OpenGL Drawing
			glBindVertexArray(gVAObj_Sphere);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVBObj_elem_Sphere);
			glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
			glBindVertexArray(0);

			// End of OpenGL shading program
			glUseProgram(0);
		}
	}

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
	if(gbEnableLighting == true) {
		if(gbXRotationEnabled == true || gbYRotationEnabled == true || gbZRotationEnabled == true)
			gGLfAngle += 0.05f;
	}
	if(gGLfAngle >= 360.0f)
		gGLfAngle = 0.0f;
}

void Uninitialize(void) {
	// Code
	if(gbFullscreen == true)
		ToggleFullscreen();

	// Destroy Vertex Array Object
	if(gVAObj_Sphere) {
		glDeleteVertexArrays(1, &gVAObj_Sphere);
		gVAObj_Sphere = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_pos_Sphere) {
		glDeleteVertexArrays(1, &gVBObj_pos_Sphere);
		gVBObj_pos_Sphere = 0;
	}
	if(gVBObj_norm_Sphere) {
		glDeleteVertexArrays(1, &gVBObj_norm_Sphere);
		gVBObj_norm_Sphere = 0;
	}
	if(gVBObj_elem_Sphere) {
		glDeleteVertexArrays(1, &gVBObj_elem_Sphere);
		gVBObj_elem_Sphere = 0;
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
