// Program to show 3D Sphere with Per Vertex and Per Fragment lighting (toggling)
// By : Darshan Vikam
// Date : 08 April 2021

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
bool gbEnableAlbedo = false;
bool gbToggleLighting = false;

GLfloat sphere_Vertices[1146];
GLfloat sphere_Normals[1146];
GLfloat sphere_Textures[764];
unsigned short sphere_Elements[2280];
GLuint gNumVertices, gNumElements;

	// for Per Vertex lighting
GLuint gVSObj_PVL;		// Vertex Shader Object
GLuint gFSObj_PVL;		// Fragment Shader Object
GLuint gSPObj_PVL;		// Shader Program Object
	// for Per Fragment lighting
GLuint gVSObj_PFL;		// Vertex Shader Object
GLuint gFSObj_PFL;		// Fragment Shader Object
GLuint gSPObj_PFL;		// Shader Program Object

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
	TCHAR szAppName[] = TEXT("PVPFLightingInPP");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("PV vs PF Lighting"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
				case 'a' :
				case 'A' :
					gbEnableAlbedo = !gbEnableAlbedo;
					break;
				case 't' :
				case 'T' :
					if(gbToggleLighting == false) {
						gMMatrixUniform = glGetUniformLocation(gSPObj_PFL, "u_MMatrix");
						gVMatrixUniform = glGetUniformLocation(gSPObj_PFL, "u_VMatrix");
						gPMatrixUniform = glGetUniformLocation(gSPObj_PFL, "u_PMatrix");
						gLightAmbientUniform = glGetUniformLocation(gSPObj_PFL, "u_LAmb");
						gLightDiffuseUniform = glGetUniformLocation(gSPObj_PFL, "u_LDiff");
						gLightSpecularUniform = glGetUniformLocation(gSPObj_PFL, "u_LSpec");
						gLightPositionUniform = glGetUniformLocation(gSPObj_PFL, "u_LPos");
						gMaterialAmbientUniform = glGetUniformLocation(gSPObj_PFL, "u_KAmb");
						gMaterialDiffuseUniform = glGetUniformLocation(gSPObj_PFL, "u_KDiff");
						gMaterialSpecularUniform = glGetUniformLocation(gSPObj_PFL, "u_KSpec");
						gMaterialShininessUniform = glGetUniformLocation(gSPObj_PFL, "u_KShine");
						gKeyPressedUniform = glGetUniformLocation(gSPObj_PFL, "u_iKeyPressed");
					}
					else {
						gMMatrixUniform = glGetUniformLocation(gSPObj_PVL, "u_MMatrix");
						gVMatrixUniform = glGetUniformLocation(gSPObj_PVL, "u_VMatrix");
						gPMatrixUniform = glGetUniformLocation(gSPObj_PVL, "u_PMatrix");
						gLightAmbientUniform = glGetUniformLocation(gSPObj_PVL, "u_LAmb");
						gLightDiffuseUniform = glGetUniformLocation(gSPObj_PVL, "u_LDiff");
						gLightSpecularUniform = glGetUniformLocation(gSPObj_PVL, "u_LSpec");
						gLightPositionUniform = glGetUniformLocation(gSPObj_PVL, "u_LPos");
						gMaterialAmbientUniform = glGetUniformLocation(gSPObj_PVL, "u_KAmb");
						gMaterialDiffuseUniform = glGetUniformLocation(gSPObj_PVL, "u_KDiff");
						gMaterialSpecularUniform = glGetUniformLocation(gSPObj_PVL, "u_KSpec");
						gMaterialShininessUniform = glGetUniformLocation(gSPObj_PVL, "u_KShine");
						gKeyPressedUniform = glGetUniformLocation(gSPObj_PVL, "u_iKeyPressed");
					}
					gbToggleLighting = !gbToggleLighting;
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

	// Per Vertex Lighting
	// Vertex Shader
	gVSObj_PVL = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode_PVL = 				// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_MMatrix;" \
		"uniform mat4 u_VMatrix;" \
		"uniform mat4 u_PMatrix;" \
		"uniform int u_iKeyPressed;" \
		"uniform vec3 u_LAmb;" \
		"uniform vec3 u_LDiff;" \
		"uniform vec3 u_LSpec;" \
		"uniform vec4 u_LPos;" \
		"uniform vec3 u_KAmb;" \
		"uniform vec3 u_KDiff;" \
		"uniform vec3 u_KSpec;" \
		"uniform float u_KShine;" \
		"out vec3 ADS_lighting;" \
		"void main(void) {" \
			"if(u_iKeyPressed == 1) {" \
				"vec4 eyeCoords = u_VMatrix * u_MMatrix * vPosition;" \
				"vec3 transformedNormal = normalize(mat3(transpose(inverse(u_VMatrix * u_MMatrix))) * vNormal);" \
				"vec3 lightSource = normalize(vec3(u_LPos - eyeCoords));" \
				"vec3 reflectionVector = reflect(-lightSource, transformedNormal);" \
				"vec3 viewVector = normalize(-eyeCoords.xyz);" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(lightSource, transformedNormal), 0.0f);" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, viewVector), 0.0f), u_KShine);" \
				"ADS_lighting = ambient + diffuse + specular;" \
			"}" \
			"else {" \
				"ADS_lighting = vec3(0.0f, 0.0f, 0.0f);" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj_PVL, 1, (const GLchar**)&VSSrcCode_PVL, NULL);
	glCompileShader(gVSObj_PVL);				// Compile Shader
	ShaderErrorCheck(gVSObj_PVL, "COMPILE");

	// Fragment Shader
	gFSObj_PVL = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode_PVL = 			// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec3 ADS_lighting;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
			"FragColor = vec4(ADS_lighting, 1.0f);" \
		"}";
	glShaderSource(gFSObj_PVL, 1, (const GLchar**)&FSSrcCode_PVL, NULL);
	glCompileShader(gFSObj_PVL);				// Compile Shader
	ShaderErrorCheck(gFSObj_PVL, "COMPILE");

	// Shader program
	gSPObj_PVL = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj_PVL, gVSObj_PVL);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj_PVL, gFSObj_PVL);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj_PVL, DV_ATTRIBUTE_POSITION, "vPosition");	// Binding of shader program with enum declared - for position(coordinates)
	glBindAttribLocation(gSPObj_PVL, DV_ATTRIBUTE_NORMAL, "vNormal");		// Binding of shader program with enum declared - for normal
	glLinkProgram(gSPObj_PVL);
	ShaderErrorCheck(gSPObj_PVL, "LINK");

	// Per Fragment Lighting
	// Vertex Shader
	gVSObj_PFL = glCreateShader(GL_VERTEX_SHADER);		// Create shader
	const GLchar *VSSrcCode_PFL = 				// Source code of shader
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
				"transformedNormal = mat3(transpose(inverse(u_VMatrix * u_MMatrix))) * vNormal;" \
				"lightSource = vec3(u_LPos - eyeCoords);" \
				"viewVector = -eyeCoords.xyz;" \
			"}" \
			"gl_Position = u_PMatrix * u_VMatrix * u_MMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj_PFL, 1, (const GLchar**)&VSSrcCode_PFL, NULL);
	glCompileShader(gVSObj_PFL);				// Compile Shader
	ShaderErrorCheck(gVSObj_PFL, "COMPILE");

	// Fragment Shader
	gFSObj_PFL = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode_PFL = 			// Source code of shader
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
				"vec3 reflectionVector = reflect(normalize(-lightSource), normalize(transformedNormal));" \
				"vec3 ambient = u_LAmb * u_KAmb;" \
				"vec3 diffuse = u_LDiff * u_KDiff * max(dot(normalize(lightSource), normalize(transformedNormal)), 0.0f);" \
				"vec3 specular = u_LSpec * u_KSpec * pow(max(dot(reflectionVector, normalize(viewVector)), 0.0f), u_KShine);" \
				"lightColor = ambient + diffuse + specular;" \
			"}" \
			"else {" \
				"lightColor = vec3(0.0f, 0.0f, 0.0f);" \
			"}" \
			"FragColor = vec4(lightColor, 1.0f);" \
		"}";
	glShaderSource(gFSObj_PFL, 1, (const GLchar**)&FSSrcCode_PFL, NULL);
	glCompileShader(gFSObj_PFL);				// Compile Shader
	ShaderErrorCheck(gFSObj_PFL, "COMPILE");

	// Shader program
	gSPObj_PFL = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj_PFL, gVSObj_PFL);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj_PFL, gFSObj_PFL);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj_PFL, DV_ATTRIBUTE_POSITION, "vPosition");	// Binding of shader program with enum declared - for position(coordinates)
	glBindAttribLocation(gSPObj_PFL, DV_ATTRIBUTE_NORMAL, "vNormal");		// Binding of shader program with enum declared - for normal
	glLinkProgram(gSPObj_PFL);
	ShaderErrorCheck(gSPObj_PFL, "LINK");

	// get uniform locations
	gMMatrixUniform = glGetUniformLocation(gSPObj_PVL, "u_MMatrix");
	gVMatrixUniform = glGetUniformLocation(gSPObj_PVL, "u_VMatrix");
	gPMatrixUniform = glGetUniformLocation(gSPObj_PVL, "u_PMatrix");
	gLightAmbientUniform = glGetUniformLocation(gSPObj_PVL, "u_LAmb");
	gLightDiffuseUniform = glGetUniformLocation(gSPObj_PVL, "u_LDiff");
	gLightSpecularUniform = glGetUniformLocation(gSPObj_PVL, "u_LSpec");
	gLightPositionUniform = glGetUniformLocation(gSPObj_PVL, "u_LPos");
	gMaterialAmbientUniform = glGetUniformLocation(gSPObj_PVL, "u_KAmb");
	gMaterialDiffuseUniform = glGetUniformLocation(gSPObj_PVL, "u_KDiff");
	gMaterialSpecularUniform = glGetUniformLocation(gSPObj_PVL, "u_KSpec");
	gMaterialShininessUniform = glGetUniformLocation(gSPObj_PVL, "u_KShine");
	gKeyPressedUniform = glGetUniformLocation(gSPObj_PVL, "u_iKeyPressed");

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
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
}

void Display(void) {
	// Variable declaration
	mat4 modelMatrix, viewMatrix, projectionMatrix;
	mat4 translateMatrix;
	vec3 lightAmbient, lightDiffuse, lightSpecular;
	vec4 lightPosition;
	vec3 materialAmbient, materialDiffuse, materialSpecular;
	GLfloat materialShininess;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Starting of OpenGL shading program
	if(gbToggleLighting == false)
		glUseProgram(gSPObj_PVL);
	else
		glUseProgram(gSPObj_PFL);

	// For Sphere
	modelMatrix = mat4::identity();
	viewMatrix = mat4::identity();
	projectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();

	if(gbEnableAlbedo == true) {
		lightAmbient = { 0.1f, 0.1f, 0.1f };
		lightDiffuse = { 1.0f, 1.0f, 1.0f };
		lightSpecular = { 1.0f, 1.0f, 1.0f };
		lightPosition = { 100.0f, 100.0f, 100.0f, 1.0f };
		materialAmbient = { 0.0f, 0.0f, 0.0f };
		materialDiffuse = { 0.5f, 0.2f, 0.7f };
		materialSpecular = { 0.7f, 0.7f, 0.7f };
		materialShininess = 128.0f;
	}
	else {
		lightAmbient = { 0.0f, 0.0f, 0.0f };
		lightDiffuse = { 1.0f, 1.0f, 1.0f };
		lightSpecular = { 1.0f, 1.0f, 1.0f };
		lightPosition = { 100.0f, 100.0f, 100.0f, 1.0f };
		materialAmbient = { 0.0f, 0.0f, 0.0f };
		materialDiffuse = { 1.0f, 1.0f, 1.0f };
		materialSpecular = { 1.0f, 1.0f, 1.0f };
		materialShininess = 50.0f;
	}
	if(gbEnableLighting == true) {
		glUniform1i(gKeyPressedUniform, 1);
		glUniform3fv(gLightAmbientUniform, 1, lightAmbient);
		glUniform3fv(gLightDiffuseUniform, 1, lightDiffuse);
		glUniform3fv(gLightSpecularUniform, 1, lightSpecular);
		glUniform4fv(gLightPositionUniform, 1, lightPosition);
		glUniform3fv(gMaterialAmbientUniform, 1, materialAmbient);
		glUniform3fv(gMaterialDiffuseUniform, 1, materialDiffuse);
		glUniform3fv(gMaterialSpecularUniform, 1, materialSpecular);
		glUniform1fv(gMaterialShininessUniform, 1, &materialShininess);
	}
	else
		glUniform1i(gKeyPressedUniform, 0);

	translateMatrix = translate(0.0f, 0.0f, -2.0f);
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

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
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

	// Detach shaders - Per Fragment Lighting
	glDetachShader(gSPObj_PFL, gVSObj_PFL);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj_PFL, gFSObj_PFL);		// Detach fragment shader from final shader program

	// Delete shaders - Per Fragment Lighting
	if(gVSObj_PFL != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj_PFL);
		gVSObj_PFL = 0;
	}
	if(gFSObj_PFL != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj_PFL);
		gFSObj_PFL = 0;
	}
	if(gSPObj_PFL != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj_PFL);
		gSPObj_PFL = 0;
	}

	// Detach shaders - Per Vertex Lighting
	glDetachShader(gSPObj_PVL, gVSObj_PVL);		// Detach vertex shader from final shader program
	glDetachShader(gSPObj_PVL, gFSObj_PVL);		// Detach fragment shader from final shader program

	// Delete shaders - Per Vertex Lighting
	if(gVSObj_PVL != 0) {			// Delete Vertex shader
		glDeleteShader(gVSObj_PVL);
		gVSObj_PVL = 0;
	}
	if(gFSObj_PVL != 0) {			// Delete Fragment shader
		glDeleteShader(gFSObj_PVL);
		gFSObj_PVL = 0;
	}
	if(gSPObj_PVL != 0) {			// Delete final shader program
		glDeleteProgram(gSPObj_PVL);
		gSPObj_PVL = 0;
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
