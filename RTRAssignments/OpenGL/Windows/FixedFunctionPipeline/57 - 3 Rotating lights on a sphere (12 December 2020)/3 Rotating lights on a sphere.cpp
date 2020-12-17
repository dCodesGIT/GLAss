// To show 3 Rotating light on a steady sphere
// Date : 12 December 2020
// By : Darshan Vikam

// Header files
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "WinIcon.h"

// Macro definition
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
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

bool bLight = false;

GLfloat lightAmbientZero[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuseZero[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightSpecularZero[] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightPositionZero[] = {0.0f, 0.0f, 0.0f, 1.0f};		// Positional light

GLfloat lightAmbientOne[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuseOne[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lightSpecularOne[] = {0.0f, 1.0f, 0.0f, 1.0f};
GLfloat lightPositionOne[] = {0.0f, 0.0f, 0.0f, 1.0f};		// Positional light

GLfloat lightAmbientTwo[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightDiffuseTwo[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lightSpecularTwo[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat lightPositionTwo[] = {0.0f, 0.0f, 0.0f, 1.0f};		// Positional light

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat materialShininess = 128.0f;

float lightAngle0 = 0.0f;
float lightAngle1 = 0.0f;
float lightAngle2 = 0.0f;

GLUquadric* quadric = NULL;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declaration
	void Initialize(void);
	void Display(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("3RotatingLightOnSphere");
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
	hwnd = CreateWindow(szAppName, TEXT("3 Rotating lights on steady sphere"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, hCentre - (WIN_WIDTH / 2), vCentre - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

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
		case WM_CHAR :
			switch(wParam) {
				case 'L' :
				case 'l' :
					if(bLight == false) {
						glEnable(GL_LIGHTING);
						bLight = true;
					}
					else {
						glDisable(GL_LIGHTING);
						bLight = false;
					}
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

	// Set depth
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Set clearColor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// actual initialization of light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientZero);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseZero);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecularZero);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbientOne);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuseOne);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecularOne);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbientTwo);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuseTwo);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecularTwo);
	glEnable(GL_LIGHT2);

	// actual initialization of material
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);

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
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
		gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		glPushMatrix();
			glRotatef(lightAngle0, 1.0f, 0.0f, 0.0f);
			lightPositionZero[1] = lightAngle0;
			glLightfv(GL_LIGHT0, GL_POSITION, lightPositionZero);
		glPopMatrix();
		glPushMatrix();
			glRotatef(lightAngle1, 0.0f, 1.0f, 0.0f);
			lightPositionOne[0] = lightAngle1;
			glLightfv(GL_LIGHT1, GL_POSITION, lightPositionOne);
		glPopMatrix();
		glPushMatrix();
			glRotatef(lightAngle2, 0.0f, 0.0f, 1.0f);
			lightPositionTwo[1] = lightAngle2;
			glLightfv(GL_LIGHT2, GL_POSITION, lightPositionTwo);
		glPopMatrix();
		glTranslatef(0.0f, 0.0f, -1.0f);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(0.0f, 0.0f, 0.0f);
		quadric = gluNewQuadric();
		gluSphere(quadric, 0.5f, 120, 120);
		// Normals are calculated by GLU library and no need to specify explicitly
	glPopMatrix();

	SwapBuffers(ghdc);
}

void Update(void) {
	// Code
	gGLfangle += 0.1f;
	if(gGLfangle >= 360.0f)
		gGLfangle = 0.0f;

	lightAngle0 += 0.025f;
	if(lightAngle0 >= 360.0f)
		lightAngle0 = 0.0f;
	lightAngle1 += 0.05f;
	if(lightAngle1 >= 360.0f)
		lightAngle1 = 0.0f;
	lightAngle2 += 0.075f;
	if(lightAngle2 >= 360.0f)
		lightAngle2 = 0.0f;
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

	if(quadric) {
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}

	fprintf(gfp, "Log.txt file closing sucessfully...\n");
	fprintf(gfp, "Program terminated sucessfullt...");
	if(gfp) {
		fclose(gfp);
		gfp = NULL;
	}
}