// Program to show animation of Deathly Hallow from Harry Potter
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

GLuint gVSObj;		// Vertex Shader Object
GLuint gFSObj;		// Fragment Shader Object
GLuint gSPObj;		// Shader Program Object
GLuint gVAObj_CloakOfInvisibility;	// Vertex Array Object 
GLuint gVBObj_CloakOfInvisibility_pos;	// Fragment Buffer Object for position
GLuint gVAObj_ResurrectionStone;	// Vertex Array Object 
GLuint gVBObj_ResurrectionStone_pos;	// Fragment Buffer Object for position
GLuint gVAObj_ElderWand;		// Vertex Array Object
GLuint gVBObj_ElderWand_pos;		// Fragment Buffer Object for position
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
	TCHAR szAppName[] = TEXT("DeathlyHallowInPP");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Deathly Hallow"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
		"uniform mat4 u_mvpMatrix;" \
		"void main(void) {" \
		"gl_Position = u_mvpMatrix * vPosition;" \
		"}";
	glShaderSource(gVSObj, 1, (const GLchar**)&VSSrcCode, NULL);
	glCompileShader(gVSObj);				// Compile Shader
	ShaderErrorCheck(gVSObj, "COMPILE");

	// Fragment Shader
	gFSObj = glCreateShader(GL_FRAGMENT_SHADER);	// Create shader
	const GLchar *FSSrcCode = 			// Source code of shader
		"#version 450 core" \
		"\n" \
		"out vec4 FragColor;" \
		"void main(void) {" \
		"FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" \
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
	const GLfloat radian = 22.0f / (7.0f * 180.0f);
	const GLfloat TriangleSide = 1.5f;
	const GLfloat Triangle_radius = (2.0f / sqrt(3.0f)) * TriangleSide;
	const GLfloat inCircle_radius = TriangleSide / sqrt(3.0f);

	const GLfloat CloakOfInvisibilityVertex[] = {
		(GLfloat)(Triangle_radius * cos(90.0f * radian)), (GLfloat)(Triangle_radius * sin(90.0f * radian)), 0.0f,	// Apex
		(GLfloat)(Triangle_radius * cos(210.0f * radian)), (GLfloat)(Triangle_radius * sin(210.0f * radian)), 0.0f,	// left bottom
		(GLfloat)(Triangle_radius * cos(330.0f * radian)), (GLfloat)(Triangle_radius * sin(330.0f * radian)), 0.0f	// right bottom
	};

	GLfloat ResurrectionStoneVertex[3600*2];
	for(int i = 0; i < 3600*2; i += 2) {
		ResurrectionStoneVertex[i] = (GLfloat)(inCircle_radius * cos(i * radian / 20.0f));
		ResurrectionStoneVertex[i+1] = (GLfloat)(inCircle_radius * sin(i * radian / 20.0f));
	}

	const GLfloat ElderWandVertex[] = {
		0.0f, (GLfloat)(inCircle_radius * 2), 0.0f,
		0.0f, (GLfloat)(-inCircle_radius), 0.0f
	};

	// For Cloak of Invisibility
	glGenVertexArrays(1, &gVAObj_CloakOfInvisibility);
	glBindVertexArray(gVAObj_CloakOfInvisibility);
		glGenBuffers(1, &gVBObj_CloakOfInvisibility_pos);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_CloakOfInvisibility_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(CloakOfInvisibilityVertex), CloakOfInvisibilityVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// For Resurrection Stone
	glGenVertexArrays(1, &gVAObj_ResurrectionStone);
	glBindVertexArray(gVAObj_ResurrectionStone);
		glGenBuffers(1, &gVBObj_ResurrectionStone_pos);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_ResurrectionStone_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ResurrectionStoneVertex), ResurrectionStoneVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// For Elder Wand
	glGenVertexArrays(1, &gVAObj_ElderWand);
	glBindVertexArray(gVAObj_ElderWand);
		glGenBuffers(1, &gVBObj_ElderWand_pos);
		glBindBuffer(GL_ARRAY_BUFFER, gVBObj_ElderWand_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(ElderWandVertex), ElderWandVertex, GL_STATIC_DRAW);
		glVertexAttribPointer(DV_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(DV_ATTRIBUTE_POSITION);
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
	// Variable declaration
	mat4 modelviewMatrix, modelviewProjectionMatrix;
	mat4 translationMatrix, rotationMatrix;
	static GLfloat trans = 5.0f;
	static GLfloat rot = 0.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Starting of OpenGL shading program
	glUseProgram(gSPObj);

	modelviewMatrix = mat4::identity();
	modelviewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	modelviewMatrix = translationMatrix;
	translationMatrix = translate(-trans, -trans, 0.0f);
	modelviewMatrix *= translationMatrix;
	rotationMatrix = rotate(rot, 0.0f, 1.0f, 0.0f);
	modelviewMatrix *= rotationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);

	// OpenGL Drawing
	glBindVertexArray(gVAObj_CloakOfInvisibility);
	glDrawArrays(GL_LINE_LOOP, 0, 3);
	glBindVertexArray(0);

	modelviewMatrix = mat4::identity();
	modelviewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	modelviewMatrix = translationMatrix;
	translationMatrix = translate(trans, -trans, 0.0f);
	modelviewMatrix *= translationMatrix;
	rotationMatrix = rotate(rot, 0.0f, 1.0f, 0.0f);
	modelviewMatrix *= rotationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);

	// OpenGL Drawing
	glBindVertexArray(gVAObj_ResurrectionStone);
	glDrawArrays(GL_POINTS, 0, 3600);
	glBindVertexArray(0);

	modelviewMatrix = mat4::identity();
	modelviewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	modelviewMatrix = translationMatrix;
	translationMatrix = translate(0.0f, trans, 0.0f);
	modelviewMatrix *= translationMatrix;
	rotationMatrix = rotate(rot, 0.0f, 1.0f, 0.0f);
	modelviewMatrix *= rotationMatrix;
	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);

	// OpenGL Drawing
	glBindVertexArray(gVAObj_ElderWand);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	// End of OpenGL shading program
	glUseProgram(0);

	if(trans >= 0.0f)
		trans -= 0.0005f;
	if(rot <= 720.0f)
		rot += 0.05f;

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
	if(gVAObj_CloakOfInvisibility) {
		glDeleteVertexArrays(1, &gVAObj_CloakOfInvisibility);
		gVAObj_CloakOfInvisibility = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_CloakOfInvisibility_pos) {
		glDeleteBuffers(1, &gVBObj_CloakOfInvisibility_pos);
		gVBObj_CloakOfInvisibility_pos = 0;
	}

	// Destroy Vertex Array Object
	if(gVAObj_ResurrectionStone) {
		glDeleteVertexArrays(1, &gVAObj_ResurrectionStone);
		gVAObj_ResurrectionStone = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_ResurrectionStone_pos) {
		glDeleteBuffers(1, &gVBObj_ResurrectionStone_pos);
		gVBObj_ResurrectionStone_pos = 0;
	}

	// Destroy Vertex Array Object
	if(gVAObj_ElderWand) {
		glDeleteVertexArrays(1, &gVAObj_ElderWand);
		gVAObj_ElderWand = 0;
	}

	// Destroy Vertex Buffer Object
	if(gVBObj_ElderWand_pos) {
		glDeleteBuffers(1, &gVBObj_ElderWand_pos);
		gVBObj_ElderWand_pos = 0;
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
