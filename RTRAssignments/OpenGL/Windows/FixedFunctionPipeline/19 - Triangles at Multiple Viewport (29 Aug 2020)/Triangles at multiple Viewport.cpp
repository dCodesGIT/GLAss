// To display spinning multicoloured triangle at different Viewport()
// Date : 29 August 2020
// By : Darshan Vikam

// Header files
#include <Windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include "WinIcon.h"
#include <GL/glu.h>

// Macro definition
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

// CallBack function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declaration
HWND ghwnd;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbFullscreen = false;
FILE *gfp = NULL;
bool gbActiveWindow = false;
HDC ghdc;
HGLRC ghrc = NULL;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declaration
	void Initialize(void);
	void Display(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("SpinningTriangleAtDifferentViewport");
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
	hwnd = CreateWindow(szAppName, TEXT("Spinning multicoloured triangle with Viewport()"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, hCentre - (WIN_WIDTH / 2), vCentre - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

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
				// Here you call the Update function for OpenGL rendering
				// Here you call the Display function for OpenGL rendering
				Display();
			}
		}
	}
	return((int)msg.wParam);
}

// CallBack function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Variable declaration
	static int h, w;

	// Function declaration
	void ToggleFullscreen(void);
	void Resize(int, int);
	void Uninitialize(void);
	void Display(void);

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
			h = HIWORD(lParam);
			w = LOWORD(lParam);
			glViewport(0, 0, (GLsizei)w, (GLsizei)h);
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE :
					if(gbFullscreen == true)
						ToggleFullscreen();
					if(MessageBox(hwnd, TEXT("Do you really want to close the application?"), TEXT("Closing window"), MB_YESNO) == IDYES)
						DestroyWindow(hwnd);
					break;
				case 0x46 :
				//case 0x66 :
					ToggleFullscreen();
					break;
				case 0x30 :
				case VK_NUMPAD0 :
					glViewport(0, 0, (GLsizei)w, (GLsizei)h);
					Resize(w, h);
					break;
				case 0x31 :
				case VK_NUMPAD1 :
					glViewport(0, h/2, (GLsizei)(w/2), (GLsizei)(h/2));
					Resize(w/2, h/2);
					break;
				case 0x32 :
				case VK_NUMPAD2 :
					glViewport(w/2, h/2, (GLsizei)(w/2), (GLsizei)(h/2));
					Resize(w/2, h/2);
					break;
				case 0x33 :
				case VK_NUMPAD3 :
					glViewport(w/2, 0, (GLsizei)(w/2), (GLsizei)(h/2));
					Resize(w/2, h/2);
					break;
				case 0x34 :
				case VK_NUMPAD4 :
					glViewport(0, 0, (GLsizei)(w/2), (GLsizei)(h/2));
					Resize(w/2, h/2);
					break;
				case 0x35 :
				case VK_NUMPAD5 :
					glViewport(0, 0, (GLsizei)(w/2), (GLsizei)h);
					Resize(w/2, h);
					break;
				case 0x36 :
				case VK_NUMPAD6 :
					glViewport(w/2, 0, (GLsizei)(w/2), (GLsizei)h);
					Resize(w/2, h);
					break;
				case 0x37 :
				case VK_NUMPAD7 :
					glViewport(0, h/2, (GLsizei)w, (GLsizei)(h/2));
					Resize(w, h/2);
					break;
				case 0x38 :
				case VK_NUMPAD8 :
					glViewport(0, 0, (GLsizei)w, (GLsizei)(h/2));
					Resize(w, h/2);
					break;
				case 0x39 :
				case VK_NUMPAD9 :
					glViewport(w/4, h/4, (GLsizei)(w/2), (GLsizei)(h/2));
					Resize(w/2, h/2);
					break;
				default :
					break;
			}
			break;
		case WM_CLOSE :
			if(MessageBox(hwnd, TEXT("Do you really want to close the application?"), TEXT("Closing window"), MB_YESNO) == IDYES)
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

	// Warmup call to Resize()
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height) {
	// Function declaration
	void Display();

	// Code
	if(height == 0)
		height = 1;
	//glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	Display();
}

void Display() {
	// Variable declaration
	static GLfloat angle = 0.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	angle += 0.1f;
	if(angle >= 360.0f)
		angle = 0.0f;

	SwapBuffers(ghdc);
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
	fprintf(gfp, "Program terminated sucessfully...");
	if(gfp) {
		fclose(gfp);
		gfp = NULL;
	}
}