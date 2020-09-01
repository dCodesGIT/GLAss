// To display window in fullscreen mode
// Date : 06 June 2020
// By : Darshan Vikam

// Header files
#include<Windows.h>

// CallBack function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global variable declaration
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;
bool gbFullscreen = false;
HWND ghwnd;

// Entey point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("App with Fullscreen");

	// Code
	// Initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;

	// Register above class
	RegisterClassEx(&wndclass);

	// Create Window
	hwnd = CreateWindow(szAppName, TEXT("Window With Fullscreen Mode"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd;
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	// Message loop
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return((int)msg.wParam);
}

// CallBack function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Function declaration
	void ToggleFullscreen(void);

	// Variable declaration
	HDC hdc;
	// PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Press 'f' to toggle between FULLSCREEN and NORMAL SCREEN");

	// Code
	switch(iMsg) {
		case WM_KEYDOWN:
			switch(wParam) {
				case 0x46:
				case 0x66:
					ToggleFullscreen();
					break;
				default:
					break;
			}
		case WM_PAINT :
			GetClientRect(hwnd, &rc);
			hdc = GetDC(hwnd);
			SetBkColor(hdc, RGB(0, 0, 0));
			SetTextColor(hdc, RGB(0, 255, 0));
			DrawText(hdc, str, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			ReleaseDC(hwnd, hdc);
			break;
		case WM_DESTROY:
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
				MessageBox(ghwnd, TEXT("Changing to Fullscreen Mode"), TEXT("Fullscreen Mode"), MB_OK);
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullscreen = true;
	}
	else {
		MessageBox(ghwnd, TEXT("Exitting Fullscreen Mode"), TEXT("Fullscreen Mode"), MB_OK);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}
