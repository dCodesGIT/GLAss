// Blue screen window using double buffer
// Date : 1 August 2020
// By : Darshan Vikam

// Header files
#include <Windows.h>
#include <stdio.h>
#include "WinIcon.h"
#include <gl/GL.h>

// Macro definition
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib, "OpenGL32.lib")

// Global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declaration
HWND ghwnd = NULL;
DWORD dwStyle;
bool gbFullscreen = false;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
FILE *gfp = NULL;
bool gbActiveWindow = false;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Local function declaration
	void Initialize(void);
	void Display(void);

	// Local variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("BlueScreenWindowUsingDoubleBuffer");
	int hCentre, vCentre;
	bool bDone = false;

	// Code
	if(fopen_s(&gfp, "Log.txt","w") != 0) {
		fprintf(gfp, "Cannot create file log.txt\n");
		exit(0);
	}
	fprintf(gfp, "Log.txt file created successfully\n");
	fprintf(gfp, "Program execution started successfully\n");

	// Initialize WNDCLASSEX members
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(D_ICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	// Register above declared structure
	RegisterClassEx(&wndclass);

	hCentre = (GetSystemMetrics(SM_CXSCREEN) / 2);
	vCentre = (GetSystemMetrics(SM_CYSCREEN) / 2);

	// Create Window call
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Blue screen window using OpenGL"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, hCentre - (WIN_WIDTH / 2), vCentre - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	ghwnd = hwnd;

	Initialize();
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop or User Controlled infinite loop
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
				// Here you call Update function for OpenGL rendering

				// Here you call Display function for OpenGL rendering
				Display();
			}
		}
	}
	return((int)msg.wParam);
}

// Callback function defintion
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Function declaration
	void ToggleFullscreen();
	void Resize(int, int);
	void Uninitialize();

	// Code
	switch(iMsg) {
		case WM_SETFOCUS:
			gbActiveWindow = true;
			break;
		case WM_KILLFOCUS:
			gbActiveWindow = false;
			break;
		case WM_ERASEBKGND:
			return 0;
		case WM_SIZE:
			Resize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_KEYDOWN:
			switch(wParam) {
				case VK_ESCAPE:
					if(MessageBox(hwnd, TEXT("Do you really want to close the application?"), TEXT("Closing window..."), MB_YESNO) == IDYES)
						DestroyWindow(hwnd);
					break;
				case 0x46:
				case 0x66:
					ToggleFullscreen();
					break;
				default:
					break;
			}
			break;
		case WM_CLOSE:
			if(MessageBox(hwnd, TEXT("Do you really want to close the application?"), TEXT("Closeing window..."), MB_YESNO) == IDYES)
				DestroyWindow(hwnd);
			else
				return true;
			break;
		case WM_DESTROY:
			Uninitialize();
			PostQuitMessage(0);
			break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

// Function definition
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
		fprintf(gfp, "ChoosePixelFormat() function failed..");
		DestroyWindow(ghwnd);
	}

	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE) {
		fprintf(gfp, "SetPixelFormat() function failed..");
		DestroyWindow(ghwnd);
	}

	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL) {
		fprintf(gfp, "wglCreateContext() function failed..");
		DestroyWindow(ghwnd);
	}

	if(wglMakeCurrent(ghdc, ghrc) == FALSE) {
		fprintf(gfp, "wglMakeCurrent() function failed..");
		DestroyWindow(ghwnd);
	}

	// Set ClearColor
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	// Warmup call to Resize()
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height) {
	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void Display(void) {
	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SwapBuffers(ghdc);
}

void Uninitialize(void) {
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

	fprintf(gfp, "Log.txt file closed successfully\n");
	fprintf(gfp, "Program terminated successfully");
	if(gfp) {
		fclose(gfp);
		gfp = NULL;
	}
}
