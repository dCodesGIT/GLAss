// To display window in fullscreen mode (Game mode) using ChangeDisplaySettings()
// Date : 05 July 2020
// By : Darshan Vikam

// Header files
#include <windows.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Callback function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable declaration
DWORD dwStyle;
bool gbFullScreen = false;
DEVMODE dmPrev = { sizeof(DEVMODE) };
WINDOWPLACEMENT wpPrev;
HWND ghwnd = NULL;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("AppFullScreenUsingChangeDisplaySettings");

	// Code
	// WNDCLASSEX structure initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	/// Register above structure
	RegisterClassEx(&wndclass);

	// Create window
	hwnd = CreateWindow(szAppName, TEXT("Window with Fullscreen mode"), WS_OVERLAPPEDWINDOW, 0, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

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

// Callback Function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Function declaration
	void ToggleFullscreen(void);

	// Variable declaration
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Press 'f' to toggle into FULLSCREEN");

	// Code
	switch(iMsg) {
		case WM_PAINT:
			GetClientRect(hwnd, &rc);
			hdc = BeginPaint(hwnd, &ps);
			SetBkColor(hdc, RGB(0, 0, 0));
			SetTextColor(hdc, RGB(0, 255, 0));
			DrawText(hdc, str, -1, &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			EndPaint(hwnd, &ps);
			break;
		case WM_KEYDOWN:
			if(wParam == 'f' || wParam == 'F')
				ToggleFullscreen();
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
	DEVMODE dmFullscreen = { sizeof(DEVMODE) };
	RECT rcFS;

	// Code
	if (gbFullScreen == false) {
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW) {
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
				dmFullscreen.dmSize = sizeof(DEVMODE);
				dmFullscreen.dmPelsWidth = WIN_WIDTH;
				dmFullscreen.dmPelsHeight = WIN_HEIGHT;
				dmFullscreen.dmBitsPerPel = 32;
				dmFullscreen.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				if (ChangeDisplaySettings(&dmFullscreen, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL) {
					ShowCursor(FALSE);
					gbFullScreen = true;
				}
			}
		}
	}
	else {
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		if(ChangeDisplaySettings(&dmFullscreen, CDS_RESET) != DISP_CHANGE_SUCCESSFUL)
			MessageBox(ghwnd, TEXT("Fullscreen Restoration failed during ChangeDisplaySettings()"), TEXT("Error"), MB_OK | MB_ICONERROR);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}

/*
// Function definition
void ToggleFullscreen() {
	// Variable Declaration
	DEVMODE dmFullscreen = { sizeof(DEVMODE) };
	RECT rcFS;

	// Code
	if(gbFullScreen == false) {
		dmFullscreen.dmSize = sizeof(DEVMODE);
		dmFullscreen.dmPelsWidth = WIN_WIDTH;
		dmFullscreen.dmPelsHeight = WIN_HEIGHT;
		dmFullscreen.dmBitsPerPel = 32;
		dmFullscreen.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		if(ChangeDisplaySettings(&dmFullscreen, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			//extendedWindowStyle = WS_EX_APPWINDOW;
			//windowStyle = WS_POPUP;
			rcFS.top = 0;
			rcFS.left = 0;
			rcFS.bottom = WIN_HEIGHT;
			rcFS.right = WIN_WIDTH;
			AdjustWindowRectEx(&rcFS, WS_POPUP, FALSE, WS_EX_APPWINDOW);
			ShowCursor(FALSE);
			gbFullScreen = true;
		}
	}
	else {
		//extendedWindowStyle = NULL;
		//windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU | WS_CLIPCHILDERN | WS_CLIPSIBLINGS;
		rcFS.top = 0;
		rcFS.left = 0;
		rcFS.bottom = WIN_HEIGHT;
		rcFS.right = WIN_WIDTH;
		AdjustWindowRectEx(&rcFS, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, TRUE, NULL);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}
*/