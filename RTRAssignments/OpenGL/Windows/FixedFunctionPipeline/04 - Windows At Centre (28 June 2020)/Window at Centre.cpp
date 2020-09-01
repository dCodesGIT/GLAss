// To create window with centre position
// Date : 28 June 2020
// By : Darshan Vikam

#include<Windows.h>

// Callback function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMain - entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("App With Handles");
	int xCoordinate = 0, yCoordinate = 0;

	// Code
	// Initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// Register above class
	RegisterClassEx(&wndclass);

	// Finding of centre coordrinates
	xCoordinate = GetSystemMetrics(SM_CXSCREEN) / 2;
	yCoordinate = GetSystemMetrics(SM_CYSCREEN) / 2;

	// Create Window
	hwnd = CreateWindow(szAppName, TEXT("Window with handles"), WS_OVERLAPPEDWINDOW, xCoordinate - 400, yCoordinate - 300, 800, 600, NULL, NULL, hInstance, NULL);

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
	// Code
	switch(iMsg) {
		case WM_CREATE:
			MessageBox(hwnd, TEXT("WM_CREATE received"), TEXT("Message 1.0"), MB_OK);
			break;
		case WM_LBUTTONDOWN:
			MessageBox(hwnd, TEXT("WM_LBUTTONDOWN received"), TEXT("Message 2.0"), MB_OK | MB_ICONINFORMATION);
			break;
		case WM_RBUTTONDOWN:
			MessageBox(hwnd, TEXT("WM_RBUTTONDOWN received"), TEXT("Message 3.0"), MB_OK | MB_ICONINFORMATION);
			break;
		case WM_MBUTTONDOWN:
			MessageBox(hwnd, TEXT("WM_MBUTTONDOWN received"), TEXT("Message 4.0"), MB_OK | MB_ICONINFORMATION);
			break;
		case WM_KEYDOWN:
			MessageBox(hwnd, TEXT("WM_KEYDOWN received"), TEXT("Message 5.0"), MB_OK | MB_ICONINFORMATION);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}