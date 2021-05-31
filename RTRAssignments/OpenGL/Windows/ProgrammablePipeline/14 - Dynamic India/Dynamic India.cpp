// Program to show Dynamic India
// By : Darshan Vikam
// Date : 05 April 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <gl/glew.h>
#include <gl/GL.h>
#include <math.h>
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
bool gbPlay = false;

GLuint gVSObj;		// Vertex Shader Object
GLuint gFSObj;		// Fragment Shader Object
GLuint gSPObj;		// Shader Program Object

GLuint gVAObj_INDIA[4];		// Vertex Array Object - INDIA alphabets
GLuint gVBObj_alpha_I[2];	// Buffer Objects for Letter I
GLuint gVBObj_alpha_N[2];	// Buffer Objects for Letter N
GLuint gVBObj_alpha_D[2];	// Buffer Objects for Letter D
GLuint gVBObj_alpha_A[2];	// Buffer Objects for Letter A

GLuint gVAObj_Tirangaa[3];	/* Vertex Array Object -
				   [0] => Actual Tirangaa
				   [1] => Ashoka Chakra's circle
				   [2] => Ashoka Chakra's 24 Spokes */
GLuint gVBObj_Tirangaa[2];		// Buffer Object for Tricolor (Tirangaa)
GLuint gVBObj_AshokaChakra_circle[2];	// Buffer Object for Ashoka Chakra's circle
GLuint gVBObj_AshokaChakra_24[2];	// Buffer Object for Ashoka Chakra's 24 spokes

GLuint gVAObj_IAF;		// Vertex Array Object - Indian Air Force planes
GLuint gVBObj_IAF_plane[2];	// Buffer Object for IAF plane

GLuint gMVPUniform;	// Matrix 

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
	TCHAR szAppName[] = TEXT("DynamicIndiaInPP");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Dynamic India"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
				case 'P' :
				case 'p' :
					gbPlay = !gbPlay;
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
		"in vec4 vColor;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec4 out_color;" \
		"void main(void) {" \
			"gl_Position = u_mvpMatrix * vPosition;" \
			"out_color = vColor;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);				// Compile Shader
	ShaderErrorCheck(gVSObj, "COMPILE");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 450 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void) {" \
		"FragColor = out_color;" \
		"}";
	glShaderSource(gFSObj, 1, (const GLchar**)&FSSrcCode, NULL);
	glCompileShader(gFSObj);				// Compile Shader
	ShaderErrorCheck(gFSObj, "COMPILE");

	// Shader program
	gSPObj = glCreateProgram();		// Create final shader
	glAttachShader(gSPObj, gVSObj);		// Add Vertex shader code to final shader
	glAttachShader(gSPObj, gFSObj);		// Add Fragment shader code to final shader
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_POSITION, "vPosition");	// Binding of shader program with enum declared - for position(coordinates)
	glBindAttribLocation(gSPObj, DV_ATTRIBUTE_COLOR, "vColor");		// Binding of shader program with enum declared - for color
	glLinkProgram(gSPObj);
	ShaderErrorCheck(gSPObj, "LINK");

	// get MVP uniform location
	gMVPUniform = glGetUniformLocation(gSPObj, "u_mvpMatrix");

	// other variable initialization
	const GLfloat Alphabet_I_vertex[] = {
		// Top quad of I
		-0.35f, 0.5f,	 0.35f, 0.5f,	 0.35f, 0.4f,	-0.35f, 0.4f,

		// Vertical quad (Upper half) of I
		-0.05f, 0.4f,	 0.05f, 0.4f,	 0.05f, 0.0f,	-0.05f, 0.0f,

		// Vertical quad (Lower half) of I
		 0.05f, 0.0f,	-0.05f, 0.0f,	-0.05f, -0.4f,	 0.05f, -0.4f,

		// Bottom quad of I
		-0.35f, -0.5f,	 0.35f, -0.5f,	 0.35f, -0.4f,	-0.35f, -0.4f
	};
	const GLfloat Alphabet_I_color[] = {
		// Top quad of I - Saffron
		1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,

		// Vertical quad (Upper half) of I - Saffron, Saffron, White, White
		1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

		// Vertical quad (Lower half) of I - White, White, Green, Green
		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

		// Bottom quad of I - Green
		0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
	};
	const GLfloat Alphabet_N_vertex[] = {
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
	const GLfloat Alphabet_N_color[] = {
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
	const GLfloat Alphabet_D_vertex[] = {
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
	const GLfloat Alphabet_D_color[] = {
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
	const GLfloat Alphabet_A_vertex[] = {
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
	const GLfloat Alphabet_A_color[] = {
		// Central Horizontal - White
		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

		// 1st Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

		// 1st Vertical quad (Lower half) - White, White, Green, Green
		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

		// Upper Horizontal quad - Saffron
		1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,

		// 2st Vertical quad (Upper half) - Saffron, Saffron, White, White
		1.0f, 0.6f, 0.2f,	1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

		// 2st Vertical quad (Lower half) - White, White, Green, Green
		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

		// 1st Vertical quad (Lowest part square) - Green
		0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,

		// 2st Vertical quad (Lowest part square) - Green
		0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f
	};
	const GLfloat mid_patch = 0.5f;
	const GLfloat tricolor_vertices[] = {
		// Upper half
		-4.0f, 3.0f,	-4.0f, mid_patch,	4.0f, mid_patch,	4.0f, 3.0f,

		// Middle half
		-4.0f, mid_patch,	-4.0f, -mid_patch,	4.0f, -mid_patch,	4.0f,mid_patch,

		// Lower half
		-4.0f, -mid_patch,	-4.0f, -3.0f,	4.0f, -3.0f,	4.0f, -mid_patch
	};
	const GLfloat tricolor_color[] = {
		// Upper half - Saffron, White, White, Saffron
		1.0f, 0.6f, 0.2f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 0.6f, 0.2f,

		// Middle half - White
		1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,	1.0f, 1.0f, 1.0f,

		// Lower half - White, Green, Green, White
		1.0f, 1.0f, 1.0f,	0.07f, 0.53f, 0.03f,	0.07f, 0.53f, 0.03f,	1.0f, 1.0f, 1.0f
	};
	const GLfloat radian = (GLfloat)(M_PI / 180.0f);
	const GLfloat radius = 0.75f;
	GLfloat AshokaChakra_vertex[3600*2];
	GLfloat AshokaChakra_color[3600*3];
	for(int i = 0; i < 3600*2; i += 2) {
		AshokaChakra_vertex[i] = radius * cos(i * radian / 20.0f);
		AshokaChakra_vertex[i+1] = radius * sin(i * radian / 20.0f);
	}
	for(int i = 0; i < 3600*3; i += 3) {
		AshokaChakra_color[i] = 0.0f;
		AshokaChakra_color[i+1] = 0.0f;
		AshokaChakra_color[i+2] = 1.0f;
	}
	GLfloat AshokaChakra_24_vertex[24*4*2];
	GLfloat AshokaChakra_24_color[24*4*3];
	const GLfloat diamond_len = 0.05f;
	const GLfloat diamond_angle = 0.05f;
	for(int i = 0, j = 0; i < 24*4*2; i += 2, j += 15) {
		AshokaChakra_24_vertex[i] = 0.0f;
		AshokaChakra_24_vertex[i+1] = 0.0f;
		i += 2;
		AshokaChakra_24_vertex[i] = (radius - diamond_len) * cos(j * radian - diamond_angle);
		AshokaChakra_24_vertex[i+1] = (radius - diamond_len) * sin(j * radian - diamond_angle);
		i += 2;
		AshokaChakra_24_vertex[i] = radius * cos(j * radian);
		AshokaChakra_24_vertex[i+1] = radius * sin(j * radian);
		i += 2;
		AshokaChakra_24_vertex[i] = (radius - diamond_len) * cos(j * radian + diamond_angle);
		AshokaChakra_24_vertex[i+1] = (radius - diamond_len) * sin(j * radian + diamond_angle);
	}
	for(int i = 0; i < 24*4*3; i += 3) {
		AshokaChakra_24_color[i] = 0.0f;
		AshokaChakra_24_color[i+1] = 0.0f;
		AshokaChakra_24_color[i+2] = 1.0f;
	}

	// For INDIA - Alphabets
	glGenVertexArrays(4, gVAObj_INDIA);
	glBindVertexArray(gVAObj_INDIA[0]);	// Alphabet I
		glGenBuffers(2, gVBObj_alpha_I);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_I[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_I_vertex), Alphabet_I_vertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_I[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_I_color), Alphabet_I_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_INDIA[1]);	// Alphabet N
		glGenBuffers(2, gVBObj_alpha_N);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_N[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_N_vertex), Alphabet_N_vertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_N[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_N_color), Alphabet_N_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_INDIA[2]);	// Alphabet D
		glGenBuffers(2, gVBObj_alpha_D);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_D[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_D_vertex), Alphabet_D_vertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_D[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_D_color), Alphabet_D_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_INDIA[3]);	// Alphabet A
		glGenBuffers(2, gVBObj_alpha_A);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_A[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_A_vertex), Alphabet_A_vertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_alpha_A[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(Alphabet_A_color), Alphabet_A_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(2, gVAObj_Tirangaa);
	glBindVertexArray(gVAObj_Tirangaa[0]);		// For tricolor
		glGenBuffers(2, gVBObj_Tirangaa);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Tirangaa[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(tricolor_vertices), tricolor_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_Tirangaa[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(tricolor_color), tricolor_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_Tirangaa[1]);		// For Ashoka chakra
		glGenBuffers(2, gVBObj_AshokaChakra_circle);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_AshokaChakra_circle[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(AshokaChakra_vertex), AshokaChakra_vertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_AshokaChakra_circle[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(AshokaChakra_color), AshokaChakra_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindVertexArray(gVAObj_Tirangaa[2]);		// For Ashoka chakra
		glGenBuffers(2, gVBObj_AshokaChakra_24);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_AshokaChakra_24[0]);	// Position
		glBufferData(GL_ARRAY_BUFFER, sizeof(AshokaChakra_24_vertex), AshokaChakra_24_vertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_AshokaChakra_24[1]);	// Color
		glBufferData(GL_ARRAY_BUFFER, sizeof(AshokaChakra_24_color), AshokaChakra_24_color, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_COLOR);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


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
	// Function declarations
	void I();
	void N();
	void D();
	void A();
	void Tricolor();

	// Variable declaration
	mat4 modelviewMatrix, modelviewProjectionMatrix;
	mat4 translationMatrix;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Starting of OpenGL shading program
	glUseProgram(gSPObj);

	modelviewMatrix = mat4::identity();
	modelviewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	modelviewMatrix = translationMatrix;
/*
	translationMatrix = translate(-2.0f, 0.0f, 0.0f);
	modelviewMatrix *= translationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
	I();

	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	modelviewMatrix *= translationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
	N();

	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	modelviewMatrix *= translationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
	D();

	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	modelviewMatrix *= translationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
	I();

	translationMatrix = translate(1.0f, 0.0f, 0.0f);
	modelviewMatrix *= translationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
	A();
*/

	translationMatrix = translate(0.0f, 0.0f, -2.0f);
	modelviewMatrix *= translationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
	Tricolor();

	// End of OpenGL shading program
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
}

void I() {
	// Code
	glBindVertexArray(gVAObj_INDIA[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glBindVertexArray(0);
}

void N() {
	// Code
	glBindVertexArray(gVAObj_INDIA[1]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
}

void D() {
	// Code
	glBindVertexArray(gVAObj_INDIA[2]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glBindVertexArray(0);
}

void A() {
	// Code
	glBindVertexArray(gVAObj_INDIA[3]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 4);
	glBindVertexArray(0);
}

void Tricolor() {
	// Code
	glBindVertexArray(gVAObj_Tirangaa[0]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glBindVertexArray(0);

	glBindVertexArray(gVAObj_Tirangaa[1]);
	glPointSize(3);
	for(int i = 0; i < 3600*2; i++)
		glDrawArrays(GL_POINTS, i, 1);
	glBindVertexArray(0);

	glBindVertexArray(gVAObj_Tirangaa[2]);
	for(int i = 0; i < 24*4; i += 4)
		glDrawArrays(GL_TRIANGLE_FAN, i, 4);
	glBindVertexArray(0);
}

void Uninitialize(void) {
	// Code
	if(gbFullscreen == true)
		ToggleFullscreen();

	// Destroy Vertex Array Object
	if(gVAObj_INDIA) {
		glDeleteVertexArrays(4, gVAObj_INDIA);
		gVAObj_INDIA[0] = 0;
		gVAObj_INDIA[1] = 0;
		gVAObj_INDIA[2] = 0;
		gVAObj_INDIA[3] = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_alpha_I) {
		glDeleteBuffers(2, gVBObj_alpha_I);
		gVBObj_alpha_I[0] = 0;
		gVBObj_alpha_I[1] = 0;
	}
	if(gVBObj_alpha_N) {
		glDeleteBuffers(2, gVBObj_alpha_N);
		gVBObj_alpha_N[0] = 0;
		gVBObj_alpha_N[1] = 0;
	}
	if(gVBObj_alpha_D) {
		glDeleteBuffers(2, gVBObj_alpha_D);
		gVBObj_alpha_D[0] = 0;
		gVBObj_alpha_D[1] = 0;
	}
	if(gVBObj_alpha_A) {
		glDeleteBuffers(2, gVBObj_alpha_A);
		gVBObj_alpha_A[0] = 0;
		gVBObj_alpha_A[1] = 0;
	}

	// Destroy Vertex Array Object
	if(gVAObj_Tirangaa) {
		glDeleteVertexArrays(3, gVAObj_Tirangaa);
		gVAObj_Tirangaa[0] = 0;
		gVAObj_Tirangaa[1] = 0;
		gVAObj_Tirangaa[2] = 0;
	}
	// Destroy Vertex Buffer Objects
	if(gVBObj_Tirangaa) {
		glDeleteBuffers(2, gVBObj_Tirangaa);
		gVBObj_Tirangaa[0] = 0;
		gVBObj_Tirangaa[1] = 0;
	}
	if(gVBObj_AshokaChakra_circle) {
		glDeleteBuffers(2, gVBObj_AshokaChakra_circle);
		gVBObj_AshokaChakra_circle[0] = 0;
		gVBObj_AshokaChakra_circle[1] = 0;
	}
	if(gVBObj_AshokaChakra_24) {
		glDeleteBuffers(2, gVBObj_AshokaChakra_24);
		gVBObj_AshokaChakra_24[0] = 0;
		gVBObj_AshokaChakra_24[1] = 0;
	}

	// Destroy Vertex Array Object
	if(gVAObj_IAF) {
		glDeleteVertexArrays(1, &gVAObj_IAF);
		gVAObj_IAF = 0;
	}
	// Destroy Vertex Buffer Object
	if(gVBObj_IAF_plane) {
		glDeleteBuffers(2, gVBObj_IAF_plane);
		gVBObj_IAF_plane[0] = 0;
		gVBObj_IAF_plane[1] = 0;
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
