// To show light and different material on 24 sphere
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

float XAxisRotation = 0.0f;
float YAxisRotation = 0.0f;
float ZAxisRotation = 0.0f;
int KeyPressCode = 0;

bool bLight = false;
GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};	// Black light
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};	// White light
GLfloat lightPosition[] = {0.3f, 0.3f, 0.3f, 1.0f};
//GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightModelAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat lightModelLocalViewer[] = {0.0f};

GLUquadric* quadric[24];

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declaration
	void Initialize(void);
	void Display(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("24Sphere");
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
	hwnd = CreateWindow(szAppName, TEXT("24 Sphere"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, hCentre - (WIN_WIDTH / 2), vCentre - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

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
				case 'x' :
				case 'X' :
					KeyPressCode = 1;
					break;
				case 'y' :
				case 'Y' :
					KeyPressCode = 2;
					break;
				case 'z' :
				case 'Z' :
					KeyPressCode = 3;
					break;
			}
			XAxisRotation = 0.0f;
			YAxisRotation = 0.0f;
			ZAxisRotation = 0.0f;
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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lightModelLocalViewer);

	// actual initialization of light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	//glEnable(GL_LIGHT0);

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

	if(width <= height)
		glOrtho(0.0f, 15.5f, 0.0f, 15.5f*((GLfloat)height/(GLfloat)width), -10.0f, 10.0f);
	else
		glOrtho(0.0f, 15.5f*((GLfloat)width/(GLfloat)height), 0.0f, 15.5f, -10.0f, 10.0f);
	//gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	Display();
}

void Display() {
	// Function declaration
	void Spheres_24(void);

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	if(KeyPressCode == 1) {
		glRotatef(XAxisRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = XAxisRotation;
		lightPosition[2] = 0.0f;
	}
	else if(KeyPressCode == 2) {
		glRotatef(YAxisRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[2] = YAxisRotation;
	}
	else if(KeyPressCode == 3) {
		glRotatef(ZAxisRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = ZAxisRotation;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	glPopMatrix();

	//glTranslatef(-1.5f, 1.0f, -3.0f);
	Spheres_24();

	SwapBuffers(ghdc);
}

void Spheres_24(void) {
	// Variable declaration
	int i, j;
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
		{0.5f, 0.5f, 0.5f, 1.0f},		// 1R 3C - Black plastic
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

	// Code
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 16.5f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for(i = 0; i < 24; i++) {
		if(i != 0 && i%6 == 0)
			glTranslatef(7.0f, 15.0f, 0.0f);
		glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient[i]);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse[i]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular[i]);
		materialShininess[i] *= 128.0f;
		glMaterialf(GL_FRONT, GL_SHININESS, materialShininess[i]);

		glTranslatef(0.0f, -2.5f, 0.0f);
		glColor3f(0.1f, 0.1f, 0.1f);
		quadric[i] = gluNewQuadric();
		gluSphere(quadric[i], 1.0f, 30, 30);
	}
}

void Update(void) {
	// Code
	gGLfangle += 0.1f;
	if(gGLfangle >= 360.0f)
		gGLfangle = 0.0f;

	XAxisRotation += 0.1f;
	YAxisRotation += 0.1f;
	ZAxisRotation += 0.1f;
/*	if(XAxisRotation >= 360.0f)
		XAxisRotation = 0.0f;
	if(YAxisRotation >= 360.0f)
		YAxisRotation = 0.0f;
	if(ZAxisRotation >= 360.0f)
		ZAxisRotation = 0.0f;
*/
}

void Uninitialize() {
	// Variable declaration
	int i;

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

	for(i = 0; i < 24; i++) {
		if(quadric[i]) {
			gluDeleteQuadric(quadric[i]);
			quadric[i] = NULL;
		}
	}

	fprintf(gfp, "Log.txt file closing sucessfully...\n");
	fprintf(gfp, "Program terminated sucessfullt...");
	if(gfp) {
		fclose(gfp);
		gfp = NULL;
	}
}