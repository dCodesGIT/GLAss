// To create window with icon
// Date : 11 July 2020
// By : Darshan Vikam

// Header files
#include<Windows.h>
#include"WinIcon.h"

// Global Callback function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {
	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("WindowWithIcon");

	// Code
	// Initialization of WNDCLASSEX members
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
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

	// Register above initialized class
	RegisterClassEx(&wndclass);

	// Create window function call
	hwnd = CreateWindow(szAppName, TEXT("Window with ICON"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Message Loop
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return((int)msg.wParam);
}

// CallBack function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Code
	switch(iMsg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default: break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}