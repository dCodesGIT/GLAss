// To display multicoloured Cube by using matrices
// Date : 11 October 2020
// By : Darshan Vikam

#define _USE_MATH_DEFINES

// Header files
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include "WinIcon.h"

// Macro definition
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

// CallBack function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global function declaration
void Update(void);

// Global variable declaration
HWND ghwnd;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
FILE *gfp = NULL;
bool gbActiveWindow = false;
HDC ghdc;
HGLRC ghrc = NULL;
GLfloat gGLfangle = 0.0f;
GLfloat identityMatrix[16];
GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat rotationMatrix[16];

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declaration
	void Initialize(void);
	void Display(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("Cube");
	int hCentre, vCentre;
	bool bDone = false;

	// Code
	if(fopen_s(&gfp, "Log.txt", "w") != 0) {
		fprintf(gfp, "Cannot create file Log.txt\n");
		exit(0);
	}
	fprintf(gfp, "Log.txt file created successfully.\n");
	fprintf(gfp, "Program execution started succesfully...\n");

	// Initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;

	// Register above class
	RegisterClassEx(&wndclass);

	hCentre = (GetSystemMetrics(SM_CXSCREEN) / 2);
	vCentre = (GetSystemMetrics(SM_CYSCREEN) / 2);

	// Create Window
	hwnd = CreateWindow(szAppName, TEXT("Multicoloured Cube"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, hCentre - (WIN_WIDTH / 2), vCentre - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;
	Initialize();
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game loop
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
			if(gbActiveWindow == true) {
				// Here you call the Display function for OpenGL rendering
				Display();
				// Here you call the Update function for OpenGL rendering
				Update();
			}
		}
	}
	return((int)msg.wParam);
}

// CallBack function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Function declaration
	void ToggleFullscreen(void);
	void Resize(int, int);
	void Uninitialize(void);

	// Code
	switch(iMsg) {
		case WM_SETFOCUS :
			gbActiveWindow = true;
			break;
		case WM_KILLFOCUS :
			gbActiveWindow = false;
			break;
		case WM_ERASEBKGND :
			return 0;
		case WM_SIZE :
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE :
					if(gbFullscreen == true)
						ToggleFullscreen();
					if(MessageBox(hwnd, TEXT("Do you really want to close the application?"), TEXT("Closing window..."), MB_YESNO) == IDYES)
						DestroyWindow(hwnd);
					break;
				case 0x46 :
				case 0x66 :
					ToggleFullscreen();
					break;
				default :
					break;
			}
			break;
		case WM_CLOSE :
			if(MessageBox(hwnd, TEXT("Do you really want to close the application?"), TEXT("Closing window..."), MB_YESNO) == IDYES)
				DestroyWindow(hwnd);
			else
				return true;
			break;
		case WM_DESTROY:
			Uninitialize();
			PostQuitMessage(0);
			break;
		default:
			break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

// Function declaration
void ToggleFullscreen() {
	// Variable declaration
	MONITORINFO mi = { sizeof(MONITORINFO) };
	
	// Code
	if(gbFullscreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW) {
			if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}

void Initialize(void) {
	// Function declaration
	void Resize(int, int);
	void myglIdentity(GLfloat *);
	void myglTranslate(GLfloat *, float, float, float);
	void myglScale(GLfloat *, float, float, float);

	// Variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// Code
	ghdc = GetDC(ghwnd);
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

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if(iPixelFormatIndex == 0) {
		fprintf(gfp, "ChoosePixelFormat() function failed...");
		DestroyWindow(ghwnd);
	}

	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		fprintf(gfp, "SetPixelFormat() function failed...");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if(ghdc == NULL) {
		fprintf(gfp, "wglCreateContext() function failed...");
		DestroyWindow(ghwnd);
	}

	if(wglMakeCurrent(ghdc, ghrc) == FALSE) {
		fprintf(gfp, "wglMakeCurrent() function failed...");
		DestroyWindow(ghwnd);
	}

	// Set clearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	myglIdentity(identityMatrix);
	myglTranslate(translationMatrix, 0.0f, 0.0f, -5.0f);
	myglScale(scaleMatrix, 0.75f, 0.75f, 0.75f);

	// Set depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Warmup call to Resize()
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height) {
	// Function declaration
	void Display();

	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	Display();
}

void Display() {
	// Function declaration
	void myglRotate(GLfloat, float, float, float);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	glLoadMatrixf(identityMatrix);

	//glTranslatef(0.0f, 0.0f, -5.0f);
	glMultMatrixf(translationMatrix);
	glMultMatrixf(scaleMatrix);
	//glRotatef(gGLfangle, 1.0f, 1.0f, 1.0f);
	myglRotate(gGLfangle, 1.0f, 0.0f, 0.0f);
	myglRotate(gGLfangle, 0.0f, 1.0f, 0.0f);
	myglRotate(gGLfangle, 0.0f, 0.0f, 1.0f);


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);		// Front face
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);		// Right face
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);		// Bottom face
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);		// Left face
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);

	glColor3f(1.0f, 0.0f, 0.0f);		// Back face
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);		// Top face
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
	gGLfangle += 0.1f;
	if(gGLfangle >= 360.0f)
		gGLfangle = 0.0f;
}

void Uninitialize() {
	// Code
	if(gbFullscreen == true) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	if(wglGetCurrentContext() == ghrc)
		wglMakeCurrent(NULL, NULL);

	if(ghrc) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if(ghdc) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	fprintf(gfp, "Log.txt file closing sucessfully...\n");
	fprintf(gfp, "Program terminated sucessfullt...");
	if(gfp) {
		fclose(gfp);
		gfp = NULL;
	}
}

void myglIdentity(GLfloat *identity) {
	// Variable declaration
	GLfloat matrix[4][4];
	int i, j;

	// Function declaration
	void TwoD2OneDColumnMajor(GLfloat matrix[4][4], GLfloat identity[16]);

	// Code
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(i == j)
				matrix[i][j] = 1;
			else
				matrix[i][j] = 0;
		}
	}	
	TwoD2OneDColumnMajor(matrix, identity);
}

void myglTranslate(GLfloat *matrix, float x, float y, float z) {
	// Code
	myglIdentity(matrix);
	matrix[12] = (GLfloat)x;
	matrix[13] = (GLfloat)y;
	matrix[14] = (GLfloat)z;
}

void myglScale(GLfloat *matrix, float x, float y, float z) {
	// Code
	myglIdentity(matrix);
	matrix[0] = (GLfloat)x;
	matrix[5] = (GLfloat)y;
	matrix[10] = (GLfloat)z;
}

void myglRotate(GLfloat angle, float x, float y, float z) {
	// Variable declaration
	float radAngle;

	// Code
	myglIdentity(rotationMatrix);
	radAngle = (float)(angle * (M_PI / 180.0f));
	if(x == 1.0f) {
		rotationMatrix[5] = cos(radAngle);
		rotationMatrix[6] = sin(radAngle);
		rotationMatrix[9] = -sin(radAngle);
		rotationMatrix[10] = cos(radAngle);
	}
	else if(y == 1.0f) {
		rotationMatrix[0] = cos(radAngle);
		rotationMatrix[2] = -sin(radAngle);
		rotationMatrix[8] = sin(radAngle);
		rotationMatrix[10] = cos(radAngle);
	}
	else if(z == 1.0f) {
		rotationMatrix[0] = cos(radAngle);
		rotationMatrix[1] = sin(radAngle);
		rotationMatrix[4] = -sin(radAngle);
		rotationMatrix[5] = cos(radAngle);
	}
	glMultMatrixf(rotationMatrix);
}

void TwoD2OneDColumnMajor(GLfloat TwoDMatrix[4][4], GLfloat OneDMatrix[16]) {
	// Variable declaration
	int i, j;

	// Code
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++)
			OneDMatrix[((i*4)+j)] = TwoDMatrix[j][i];
	}
}