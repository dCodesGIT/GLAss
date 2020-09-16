// To display INDIA text with animation
// Date : 12 September 2020
// By : Darshan Vikam

// Header files
#include <Windows.h>
#include <GL/gl.h>
#include <stdio.h>
#include "WinIcon.h"
#include "WinSound.h"
#include <GL/glu.h>
#include <math.h>

// Macro definition
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

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
float gfTimer = 0.0f;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declaration
	void Initialize(void);
	void Display(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("DynamicIndia");
	int hCentre, vCentre;
	bool bDone = false;

	// Code
	if(fopen_s(&gfp, "Log.txt", "w") != 0) {
		fprintf(gfp, "Cannot create file Log.txt\n");
		exit(0);
	}
	fprintf(gfp, "Log.txt file created successfully.\n");
	fprintf(gfp, "Programexecution started succesfully...\n");

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
	hwnd = CreateWindow(szAppName, TEXT("Dynamic INDIA"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, hCentre - (WIN_WIDTH / 2), vCentre - (WIN_HEIGHT / 2), WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;
	Initialize();
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	if(PlaySound(MAKEINTRESOURCE(Rang_De_Basanti_Chola), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC) == FALSE)
		MessageBox(hwnd, TEXT("Error playing audio !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);

//	if(PlaySound(TEXT("Rang De Basanti Chola final.wav"), hInstance, SND_ASYNC | SND_NODEFAULT) == FALSE)
//		MessageBox(hwnd, TEXT("Error playing audio !!!"), TEXT("Error"), MB_OK | MB_ICONERROR);

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
	PlaySound(NULL, 0, 0);
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
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
	Display();
}

void Display() {
	// Function declaration
	void Tricolor(float);
	void I();
	void N();
	void D();
	void A();
	void IAFShow();

	// Variable declaration
	static float i = -35.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);

	if(i < 0.0f)
		Tricolor(i);
	if(i > 0.0f) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5.0f);
		glTranslatef(-2.0f, 0.0f, 0.0f);
		if(gfTimer >= 0.0f && gfTimer <= 5.0f)
			glTranslatef(gfTimer - 5.0f, 0.0f, 0.0f);
		I();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5.0f);
		glTranslatef(2.0f, 0.0f, 0.0f);
		if(gfTimer >= 0.0f && gfTimer <= 5.0f)
			glTranslatef(5.0f - gfTimer, 0.0f, 0.0f);
		A();
		gfTimer += 0.001f;
	}
	if(i > 5.0f) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5.0f);
		glTranslatef(-1.0f, 0.0f, 0.0f);
		if(gfTimer >= 5.0f && gfTimer <= 10.0f)
			glTranslatef(0.0f, 10.0f - gfTimer, 0.0f);
		N();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5.0f);
		glTranslatef(1.0f, 0.0f, 0.0f);
		if(gfTimer >= 5.0f && gfTimer <= 10.0f)
			glTranslatef(0.0f, gfTimer - 10.0f, 0.0f);
		I();
	}
	if(i > 10.0f) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -5.0f);
		D();
	}
	if(i > 32.0f)
		IAFShow();

	i += 0.001f;

	SwapBuffers(ghdc);
}

void Tricolor(float counter) {
	// Variable declaration
	float color[3];
	float rad, radius;
	float x, fade;

	// Code
	fade = 5.0f;
	glTranslatef(0.0f, 0.0f, -2.0f);
	counter += 35;
	if(counter > 0.0f && counter < 10.0f) {			// Saffron - fade in and out
		glBegin(GL_QUADS);
		if(counter < fade) {
			color[0] = (1.0f/fade) * counter;
			color[1] = (0.6f/fade) * counter;
			color[2] = (0.2f/fade) * counter;
		}
		else {
			color[0] = (1.0f/fade) * ((2 * fade) - counter);
			color[1] = (0.6f/fade) * ((2 * fade) - counter);
			color[2] = (0.2f/fade) * ((2 * fade) - counter);
		}
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(4.0f, 3.0f, 0.0f);
		glVertex3f(4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, 3.0f, 0.0f);
		glEnd();
	}

	if(counter > 10.0f && counter < 20.0f) {		// White fade in and out
		counter -= 10.0f;
		glBegin(GL_QUADS);
		if(counter < fade) {
			color[0] = (1.0f/fade) * counter;
			color[1] = (1.0f/fade) * counter;
			color[2] = (1.0f/fade) * counter;
		}
		else {
			color[0] = (1.0f/fade) * ((2 * fade) - counter);
			color[1] = (1.0f/fade) * ((2 * fade) - counter);
			color[2] = (1.0f/fade) * ((2 * fade) - counter);
		}
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(4.0f, 3.0f, 0.0f);
		glVertex3f(4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, 3.0f, 0.0f);
		glEnd();
	}

	if(counter > 20.0f && counter < 30.0f) {		// Green fade in and out
		counter -= 20.0f;
		glBegin(GL_QUADS);
		if(counter < fade) {
			color[0] = (0.07f/fade) * counter;
			color[1] = (0.53f/fade) * counter;
			color[2] = (0.03f/fade) * counter;
		}
		else {
			color[0] = (0.07f/fade) * ((2 * fade) - counter);
			color[1] = (0.53f/fade) * ((2 * fade) - counter);
			color[2] = (0.03f/fade) * ((2 * fade) - counter);
		}
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(4.0f, 3.0f, 0.0f);
		glVertex3f(4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, 3.0f, 0.0f);
		glEnd();
	}

	if(counter > 30.0f && counter < 35.0f) {		// Flag fade in and out
		counter -= 30.0f;
		counter *= 2.0f;
		fade = 5.0f;
		glBegin(GL_QUADS);
		if(counter < fade) {
			color[0] = (1.0f/fade) * counter;
			color[1] = (0.6f/fade) * counter;
			color[2] = (0.2f/fade) * counter;
		}
		else {
			color[0] = (1.0f/fade) * ((2 * fade) - counter);
			color[1] = (0.6f/fade) * ((2 * fade) - counter);
			color[2] = (0.2f/fade) * ((2 * fade) - counter);
		}
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(4.0f, 3.0f, 0.0f);
		glVertex3f(4.0f, 1.0f, 0.0f);
		glVertex3f(-4.0f, 1.0f, 0.0f);
		glVertex3f(-4.0f, 3.0f, 0.0f);

		if(counter < fade) {
			color[0] = (1.0f/fade) * counter;
			color[1] = (1.0f/fade) * counter;
			color[2] = (1.0f/fade) * counter;
		}
		else {
			color[0] = (1.0f/fade) * ((2 * fade) - counter);
			color[1] = (1.0f/fade) * ((2 * fade) - counter);
			color[2] = (1.0f/fade) * ((2 * fade) - counter);
		}
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(4.0f, 1.0f, 0.0f);
		glVertex3f(4.0f, -1.0f, 0.0f);
		glVertex3f(-4.0f, -1.0f, 0.0f);
		glVertex3f(-4.0f, 1.0f, 0.0f);

		if(counter < fade) {
			color[0] = (0.07f/fade) * counter;
			color[1] = (0.53f/fade) * counter;
			color[2] = (0.03f/fade) * counter;
		}
		else {
			color[0] = (0.07f/fade) * ((2 * fade) - counter);
			color[1] = (0.53f/fade) * ((2 * fade) - counter);
			color[2] = (0.03f/fade) * ((2 * fade) - counter);
		}
		glColor3f(color[0], color[1], color[2]);
		glVertex3f(4.0f, -1.0f, 0.0f);
		glVertex3f(4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, -3.0f, 0.0f);
		glVertex3f(-4.0f, -1.0f, 0.0f);
		glEnd();

		rad = (22.0f/7.0f)/180.0f;
		radius = 0.9f;
		glPointSize(2);
		glBegin(GL_POINTS);			// Ashoka chakra
		if(counter < fade)
			glColor3f(0.0f, 0.0f, (0.5f/fade) * counter);
		else
			glColor3f(0.0f, 0.0f, (0.5f/fade) * ((2 * fade) - counter));
		for(x=0.0f; x<=360; x+=0.01f)
			glVertex3f(radius*cos(x*rad), radius*sin(x*rad), 0.0f);
		glEnd();

		radius = 0.85f;
		glBegin(GL_LINES);
		if(counter < fade)
			glColor3f(0.0f, 0.0f, (0.5f/fade) * counter);
		else
			glColor3f(0.0f, 0.0f, (0.5f/fade) * ((2 * fade) - counter));
		for(x=0; x<=360; x+=15) {
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(radius*cos(x*rad), radius*sin(x*rad), 0.0f);
		}
		glEnd();
	}
}

void I() {
	// Code
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(-0.35f, 0.5f, 0.0f);
		glVertex3f(0.35f, 0.5f, 0.0f);
		glVertex3f(0.35f, 0.4f, 0.0f);
		glVertex3f(-0.35f, 0.4f, 0.0f);

		glVertex3f(-0.05f, 0.4f, 0.0f);
		glVertex3f(0.05f, 0.4f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(0.05f, 0.0f, 0.0f);
		glVertex3f(-0.05f, 0.0f, 0.0f);

		glVertex3f(0.05f, 0.0f, 0.0f);
		glVertex3f(-0.05f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);	// Green
		glVertex3f(-0.05f, -0.4f, 0.0f);
		glVertex3f(0.05f, -0.4f, 0.0f);

		glVertex3f(-0.35f, -0.5f, 0.0f);
		glVertex3f(0.35f, -0.5f, 0.0f);
		glVertex3f(0.35f, -0.4f, 0.0f);
		glVertex3f(-0.35f, -0.4f, 0.0f);
	glEnd();
}

void N() {
	// Code
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(-0.35f, 0.5f, 0.0f);
		glVertex3f(-0.25f, 0.5f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glVertex3f(-0.35f, 0.0f, 0.0f);

		glVertex3f(-0.35f, 0.0f, 0.0f);
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);	// Green
		glVertex3f(-0.25f, -0.5f, 0.0f);
		glVertex3f(-0.35f, -0.5f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(-0.35f, 0.5f, 0.0f);
		glVertex3f(-0.25f, 0.5f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(0.05f, 0.0f, 0.0f);
		glVertex3f(-0.05f, 0.0f, 0.0f);

		glVertex3f(0.05f, 0.0f, 0.0f);
		glVertex3f(-0.05f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);	// Green
		glVertex3f(0.25f, -0.5f, 0.0f);
		glVertex3f(0.35f, -0.5f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(0.35f, 0.5f, 0.0f);
		glVertex3f(0.25f, 0.5f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(0.25f, 0.0f, 0.0f);
		glVertex3f(0.35f, 0.0f, 0.0f);

		glVertex3f(0.35f, 0.0f, 0.0f);
		glVertex3f(0.25f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);	// Green
		glVertex3f(0.25f, -0.5f, 0.0f);
		glVertex3f(0.35f, -0.5f, 0.0f);
	glEnd();
}

void D() {
	// Variable declaration
	float saffron[3] = {1.0f, 0.6f, 0.2f};
	float white[3] = {1.0f, 1.0f, 1.0f};
	float green[3] = {0.07f, 0.53f, 0.03f};

	// Code
	if(gfTimer >= 10.0f && gfTimer <= 20.0f) {
		saffron[0] = (1.0f/10.0f) * (gfTimer - 10.0f);
		saffron[1] = (0.6f/10.0f) * (gfTimer - 10.0f);
		saffron[2] = (0.2f/10.0f) * (gfTimer - 10.0f);
		white[0] = (1.0f/10.0f) * (gfTimer - 10.0f);
		white[1] = (1.0f/10.0f) * (gfTimer - 10.0f);
		white[2] = (1.0f/10.0f) * (gfTimer - 10.0f);
		green[0] = (0.07f/10.0f) * (gfTimer - 10.0f);
		green[1] = (0.53f/10.0f) * (gfTimer - 10.0f);
		green[2] = (0.03f/10.0f) * (gfTimer - 10.0f);
	}
	glBegin(GL_QUADS);
		glColor3f(saffron[0], saffron[1], saffron[2]);	// Saffron
		glVertex3f(-0.25f, 0.4f, 0.0f);
		glVertex3f(-0.15f, 0.4f, 0.0f);
		glColor3f(white[0], white[1], white[2]);	// White
		glVertex3f(-0.15f, 0.0f, 0.0f);
		glVertex3f(-0.25f, 0.0f, 0.0f);

		glVertex3f(-0.25f, 0.0f, 0.0f);
		glVertex3f(-0.15f, 0.0f, 0.0f);
		glColor3f(green[0], green[1], green[2]);	// Green
		glVertex3f(-0.15f, -0.4f, 0.0f);
		glVertex3f(-0.25f, -0.4f, 0.0f);

		glColor3f(saffron[0], saffron[1], saffron[2]);	// Saffron
		glVertex3f(-0.35f, 0.5f, 0.0f);
		glVertex3f(0.35f, 0.5f, 0.0f);
		glVertex3f(0.35f, 0.4f, 0.0f);
		glVertex3f(-0.35f, 0.4f, 0.0f);

		glColor3f(saffron[0], saffron[1], saffron[2]);	// Saffron
		glVertex3f(0.35f, 0.4f, 0.0f);
		glVertex3f(0.25f, 0.4f, 0.0f);
		glColor3f(white[0], white[1], white[2]);	// White
		glVertex3f(0.25f, 0.0f, 0.0f);
		glVertex3f(0.35f, 0.0f, 0.0f);

		glVertex3f(0.35f, 0.0f, 0.0f);
		glVertex3f(0.25f, 0.0f, 0.0f);
		glColor3f(green[0], green[1], green[2]);	// Green
		glVertex3f(0.25f, -0.4f, 0.0f);
		glVertex3f(0.35f, -0.4f, 0.0f);

		glColor3f(green[0], green[1], green[2]);	// Green
		glVertex3f(-0.35f, -0.5f, 0.0f);
		glVertex3f(0.35f, -0.5f, 0.0f);
		glVertex3f(0.35f, -0.4f, 0.0f);
		glVertex3f(-0.35f, -0.4f, 0.0f);
	glEnd();
}

void A() {
	// Code
	glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(0.25f, 0.05f, 0.0f);
		glVertex3f(0.25f, -0.05f, 0.0f);
		glVertex3f(-0.25f, -0.05f, 0.0f);
		glVertex3f(-0.25f, 0.05f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(-0.35f, 0.4f, 0.0f);
		glVertex3f(-0.25f, 0.4f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glVertex3f(-0.35f, 0.0f, 0.0f);

		glVertex3f(-0.35f, 0.0f, 0.0f);
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);	// Green
		glVertex3f(-0.25f, -0.4f, 0.0f);
		glVertex3f(-0.35f, -0.4f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(0.35f, 0.5f, 0.0f);
		glVertex3f(0.35f, 0.4f, 0.0f);
		glVertex3f(-0.35f, 0.4f, 0.0f);
		glVertex3f(-0.35f, 0.5f, 0.0f);

		glColor3f(1.0f, 0.6f, 0.2f);	// Saffron
		glVertex3f(0.35f, 0.4f, 0.0f);
		glVertex3f(0.25f, 0.4f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);	// White
		glVertex3f(0.25f, 0.0f, 0.0f);
		glVertex3f(0.35f, 0.0f, 0.0f);

		glVertex3f(0.35f, 0.0f, 0.0f);
		glVertex3f(0.25f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);	// Green
		glVertex3f(0.25f, -0.4f, 0.0f);
		glVertex3f(0.35f, -0.4f, 0.0f);

		glVertex3f(-0.25f, -0.4f, 0.0f);
		glVertex3f(-0.35f, -0.4f, 0.0f);
		glVertex3f(-0.35f, -0.5f, 0.0f);
		glVertex3f(-0.25f, -0.5f, 0.0f);

		glVertex3f(0.25f, -0.4f, 0.0f);
		glVertex3f(0.35f, -0.4f, 0.0f);
		glVertex3f(0.35f, -0.5f, 0.0f);
		glVertex3f(0.25f, -0.5f, 0.0f);
	glEnd();
}

void IAFShow() {
	// Function declaration
	void IAFPlane();

	// Variable declaration
	float i = gfTimer - 32.0f;

	// Code
	if(i > 5.35f) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(2.0f, 0.0f, -5.0f);
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(0.25f, 0.05f, 0.0f);
		glVertex3f(0.25f, 0.016f, 0.0f);
		glVertex3f(-0.25f, 0.016f, 0.0f);
		glVertex3f(-0.25f, 0.05f, 0.0f);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.25f, 0.016f, 0.0f);
		glVertex3f(0.25f, -0.016f, 0.0f);
		glVertex3f(-0.25f, -0.016f, 0.0f);
		glVertex3f(-0.25f, 0.016f, 0.0f);

		glColor3f(0.07f, 0.53f, 0.03f);
		glVertex3f(0.25f, -0.05f, 0.0f);
		glVertex3f(0.25f, -0.016f, 0.0f);
		glVertex3f(-0.25f, -0.016f, 0.0f);
		glVertex3f(-0.25f, -0.05f, 0.0f);
		glEnd();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glTranslatef(-2.75f, 0.0f, 0.0f);
	glTranslatef(i, 0.0f, 0.0f);
	IAFPlane();
	if(i < 5.25f) {
		glTranslatef(1.8f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-2.0f, 0.016f, 0.0f);
		glVertex3f(-2.0f, -0.016f, 0.0f);
		glVertex3f(-2.5f, -0.016f, 0.0f);
		glVertex3f(-2.5f, 0.016f, 0.0f);
		glEnd();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glTranslatef(-3.0f, -0.15f, 0.0f);
	glTranslatef(i, 1 / i, 0.0f);
	if(i < 1)
		glRotatef(90*(1-i), 0.0f, 0.0f, -1.0f);
	IAFPlane();
	if(i < 5.5f) {
		glTranslatef(1.8f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-2.0f, 0.016f, 0.0f);
		glVertex3f(-2.0f, -0.016f, 0.0f);
		glVertex3f(-2.5f, -0.016f, 0.0f);
		glVertex3f(-2.5f, 0.016f, 0.0f);
		glEnd();
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glTranslatef(-3.0f, 0.15f, 0.0f);
	glTranslatef(i, -1 / i, 0.0f);
	if(i < 1)
		glRotatef(90*(1-i), 0.0f, 0.0f, 1.0f);
	IAFPlane();
	if(i < 5.5f) {
		glTranslatef(1.8f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glColor3f(0.07f, 0.53f, 0.03f);
		glVertex3f(-2.0f, 0.016f, 0.0f);
		glVertex3f(-2.0f, -0.016f, 0.0f);
		glVertex3f(-2.5f, -0.016f, 0.0f);
		glVertex3f(-2.5f, 0.016f, 0.0f);
		glEnd();
	}

	gfTimer -= 0.0005f;
}

void IAFPlane() {
	// Code
	glBegin(GL_QUADS);
		glColor3f(0.0f, 0.0f, 0.5f);
		glVertex3f(0.15f, 0.0f, 0.0f);
		glVertex3f(0.0f, -0.05f, 0.0f);
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.05f, 0.0f);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 0.0f, 0.5f);
		glVertex3f(0.1f, 0.0f, 0.0f);
		glVertex3f(0.0f, -0.25f, 0.0f);
		glVertex3f(0.0f, 0.25f, 0.0f);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 0.0f, 0.5f);
		glVertex3f(-0.1f, 0.0f, 0.0f);
		glVertex3f(-0.25f, -0.15f, 0.0f);
		glVertex3f(-0.2f, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 0.0f, 0.5f);
		glVertex3f(-0.1f, 0.0f, 0.0f);
		glVertex3f(-0.25, 0.15f, 0.0f);
		glVertex3f(-0.2f, 0.0f, 0.0f);
	glEnd();
	glLineWidth(3);
	glBegin(GL_LINES);
		glColor3f(1.0f, 0.6f, 0.2f);
		glVertex3f(-0.05f, 0.01f, 0.0f);
		glVertex3f(0.05f, 0.01f, 0.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.05f, 0.0f, 0.0f);
		glVertex3f(0.05f, 0.0f, 0.0f);
		glColor3f(0.07f, 0.53f, 0.03f);
		glVertex3f(-0.05f, -0.01f, 0.0f);
		glVertex3f(0.05f, -0.01f, 0.0f);
	glEnd();
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
