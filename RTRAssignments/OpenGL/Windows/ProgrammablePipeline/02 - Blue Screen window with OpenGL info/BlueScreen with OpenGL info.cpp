/* Program to retrieve OpenGL related information -
	1. OpenGL vendor company
	2. OpenGL renderer(driver/graphics card)
	3. OpenGL Version
	4. OpenGL Shading Language(GLSL) version and 
	5. Enumerate OpenGL's enabled extentions
*/
// By : Darshan Vikam
// Date : 4 March 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
//#include <gl/glew.h>
#include "C:\glew-2.1.0\include\gl\glew.h"
#include <gl/GL.h>
#include "WinIcon.h"

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

// Global variable declaration
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
FILE *gfp = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullscreen = false;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declarations
	void Initialize(void);
	void Display(void);
	void Uninitialize(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("BlueScreenWindowWithOpenGLInfo");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Blue Screen window with OpenGL related info"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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
			if(gbActiveWindow == true)
				if(gbEscapeKeyPressed == true)
					bDone = true;
		}
	}

	Uninitialize();

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
					MessageBox(ghwnd, TEXT("Do you want really want to quit?"), TEXT("Quit"), MB_YESNO);
					gbEscapeKeyPressed = true;
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
			Uninitialize();
			break;

		case WM_DESTROY :
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

	// Variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	FILE *OGL_info = NULL;

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
	if(iPixelFormatIndex == 0) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if(ghrc == NULL) {
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if(wglMakeCurrent(ghdc, ghrc) == false) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK) {
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// OpenGL related log entry
	fopen_s(&OGL_info, "OpenGL_info.txt", "w");
	if(OGL_info == NULL)
		MessageBox(ghwnd, TEXT("Unable to open file to write OpenGL related information"), NULL, NULL);
	fprintf(OGL_info, "*** OpenGL Information ***\n\n");
	fprintf(OGL_info, "*** OpenGL related basic information ***\n");
	fprintf(OGL_info, "OpenGL Vendor Company : %s\n", glGetString(GL_VENDOR));
	fprintf(OGL_info, "OpenGL Renderer(Graphics card company) : %s\n", glGetString(GL_RENDERER));
	fprintf(OGL_info, "OpenGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(OGL_info, "Graphics Library Shading Language(GLSL) Version : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(OGL_info, "*** OpenGL supported/related extentions ***\n");
	// OpenGL supported/related Extensions
	GLint numExts;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
	for(int i = 0; i < numExts; i++)
		fprintf(OGL_info, "%d. %s\n", i+1, glGetStringi(GL_EXTENSIONS, i));
	fclose(OGL_info);
	OGL_info = NULL;

	// Depth related OpenGL code
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	// Warm up call to Resize()
	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void Resize(int width, int height) {
	// Code
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}

void Display(void) {
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// OpenGL Drawing

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
