// To demonstrate File I/O Creation using GUI
// Date : 28 June 2020
// By : Darshan Vikam

#include<Windows.h>
#include<stdio.h>

// Callback function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global File pointer declaration
FILE *fp = NULL;

// WinMain - entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("File IO");

	// Code
	// Opening of file
	if(fopen_s(&fp, "Log.txt", "w") != 0) {
		MessageBox(NULL, TEXT("Cannot create file"), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}
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

	// Create Window
	hwnd = CreateWindow(szAppName, TEXT("File I/O using window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	// Message loop
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	fclose(fp);
	fp = NULL;
	return((int)msg.wParam);
}

// Callback function definition
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	// Code
	switch(iMsg) {
		case WM_CREATE :
			fprintf(fp, "INDIA is my country... \n");
			break;
		case WM_DESTROY:
			fprintf(fp, "Jai Hind \n");
			PostQuitMessage(0);
			break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}