// Bluescreen in DirectX 3D
// By : Darshan Vikam
// Date : 1 August 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
#include <d3d11.h>
#include "../Icon/WinIcon.h"

// Library linking
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Call back function - WndProc()
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global function declaration
void WriteLog(char *, char *, char *);

// Global macro definitions
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global variable declaration
HWND ghwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
char FileName_log[] = "Log.txt";
float gClearColor[4];		// RGBA

bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullscreen = false;

IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

// Entry point function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow) {
	// Function declarations
	HRESULT Initialize(void);
	void Display(void);
	void Uninitialize(void);

	// Variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("BlueScreenInDirect3D11");
	bool bDone = false;

	// Code
	WriteLog(FileName_log, "w", "Log file created successfully.. \n");
	WriteLog(FileName_log, "a+", "Program started successfully.. \n");

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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Blue Screen in Direct3D 11"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Initialize Direct3D
	HRESULT hr = Initialize();
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "Initialize() failed !!!\n");
		DestroyWindow(hwnd);
		hwnd = NULL;
	}

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
	HRESULT Resize(int, int);
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
			if(gpID3D11DeviceContext) {
				HRESULT hr = Resize(LOWORD(lParam), HIWORD(lParam));
				if(FAILED(hr)) {
					WriteLog(FileName_log, "a+", "Resize() failed !!!\n");
					return hr;
				}
			}
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
					gbFullscreen = !gbFullscreen;
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

void WriteLog(char *FileName, char *Option, char *Log) {
	// Code
	FILE *fp = fopen(FileName, Option);
	if(fp == NULL) {
		MessageBox(NULL, TEXT("File cannot be created.\n Exitting..."), TEXT("ERROR"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(fp, "%s", Log);
	fclose(fp);
	fp = NULL;
}

HRESULT Initialize(void) {
	// Function declaration
	void LogD3DInfo(D3D_DRIVER_TYPE, D3D_FEATURE_LEVEL);
	HRESULT Resize(int, int);
	void Uninitialize(void);

	// Variable declaration
	HRESULT hr;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = { D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,		// Windows Advanced Rasterization Platform
			D3D_DRIVER_TYPE_REFERENCE };	// Software driver
	UINT numDriverTypes = 0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;
	UINT numFeatureLevels = 1;	// Based on d3dFeatureLevel_required

	// Code
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void *)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;		// Also known as MSAA (Multi Sampling and Anti Alizing)
	dxgiSwapChainDesc.Windowed = TRUE;

	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D_DRIVER_TYPE);
	for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL,			// H/w Adapter
					d3dDriverType,			// Driver Type
					NULL,				// Software Driver handler (if rasterizer was written by us)
					0,				// Device flags (0 if not to be specified)
					&d3dFeatureLevel_required,	// Feature Level
					numFeatureLevels,		// Number of Feature Levels
					D3D11_SDK_VERSION,		// SDK Version
					&dxgiSwapChainDesc,		// Swap Chain Descriptor
					&gpIDXGISwapChain,		// Swap Chain
					&gpID3D11Device,		// Device
					&d3dFeatureLevel_acquired,	// Feature Level
					&gpID3D11DeviceContext		// Device Context
		);
		if(SUCCEEDED(hr))
			break;
	}
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "D3D11CreateDeviceAndSwapChain() failed !!!\n");
		return hr;
	}
	LogD3DInfo(d3dDriverType, d3dFeatureLevel_acquired);
/*	WriteLog(FileName_log, "a+", "Driver : ");
	if(d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		WriteLog(FileName_log, "a+", "Hardware driver\n");
	else if(d3dDriverType == D3D_DRIVER_TYPE_WARP)
		WriteLog(FileName_log, "a+", "WARP type driver\n");
	else if(d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		WriteLog(FileName_log, "a+", "Reference type driver\n");
	else
		WriteLog(FileName_log, "a+", "Unknown type driver\n");
	WriteLog(FileName_log, "a+", "Feature Support : ");
	if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		WriteLog(FileName_log, "a+", "11.0 (highest)\n");
	else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		WriteLog(FileName_log, "a+", "10.1 \n");
	else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		WriteLog(FileName_log, "a+", "10.0 \n");
	else
		WriteLog(FileName_log, "a+", "Unknown \n");
*/
	// d3d Clear color (blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 1.0f;
	gClearColor[3] = 1.0f;

	// Warm up call to resize
	hr = Resize(WIN_WIDTH, WIN_HEIGHT);
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "Resize() failed !!!\n");
		return hr;
	}

	return S_OK;
}

HRESULT Resize(int width, int height) {
	// Variable declaration
	HRESULT hr = S_OK;

	// Code
	if(gpID3D11RenderTargetView) {		// Free any size dependent resources
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	// resize swap chain buffers accordingly
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// get back buffer from swap chain
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pID3D11Texture2D_BackBuffer);

	// get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer, NULL, &gpID3D11RenderTargetView);
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateRenderTargetView() failed !!!\n");
		return hr;
	}
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	// Set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	// Set Viewport
	D3D11_VIEWPORT d3dViewPort;
	ZeroMemory((void *)&d3dViewPort, sizeof(D3D11_VIEWPORT));
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	return hr;
}

void Display(void) {
	// Code
	// Clear render target view to chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	// Switch between buffers
	gpIDXGISwapChain->Present(0, 0);
}

void Uninitialize(void) {
	// Code
	if(gpID3D11RenderTargetView) {
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	if(gpID3D11DeviceContext) {
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if(gpID3D11Device) {
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	if(gpIDXGISwapChain) {
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	WriteLog(FileName_log, "a+", "Log file closed successfully...\n");
}

void LogD3DInfo(D3D_DRIVER_TYPE driverType, D3D_FEATURE_LEVEL featureLevel) {
	// Varibale declaration
	IDXGIFactory *pIDXGIFactory = NULL;
	IDXGIAdapter *pIDXGIAdapter = NULL;
	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	char str[255];
	char buffer[255];

	// Code
	if(FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory))) {
		WriteLog(FileName_log, "a+", "CreateDXGIFactory failed..\n");
		goto cleanup;
	}
	if(pIDXGIFactory->EnumAdapters(0, &pIDXGIAdapter) == DXGI_ERROR_NOT_FOUND) {
		WriteLog(FileName_log, "a+", "DXGIAdapter not found..\n");
		goto cleanup;
	}

	ZeroMemory((void*)&dxgiAdapterDesc, sizeof(DXGI_ADAPTER_DESC));
	pIDXGIAdapter->GetDesc(&dxgiAdapterDesc);
	WideCharToMultiByte(CP_ACP, 0, dxgiAdapterDesc.Description, 255, str, sizeof(str), NULL, NULL);

	sprintf(buffer, "\nGraphics Card Name : %s\n", str);
	WriteLog(FileName_log, "a+", buffer);

	sprintf(buffer, "Graphics Card Video Memory : %d MB(%I64d bytes) \n", (dxgiAdapterDesc.DedicatedVideoMemory)/(1024*1024), (__int64)dxgiAdapterDesc.DedicatedVideoMemory);
	WriteLog(FileName_log, "a+", buffer);

	WriteLog(FileName_log, "a+", "Driver : ");
	if(driverType == D3D_DRIVER_TYPE_HARDWARE)
		WriteLog(FileName_log, "a+", "Hardware driver\n");
	else if(driverType == D3D_DRIVER_TYPE_WARP)
		WriteLog(FileName_log, "a+", "WARP type driver\n");
	else if(driverType == D3D_DRIVER_TYPE_REFERENCE)
		WriteLog(FileName_log, "a+", "Reference type driver\n");
	else
		WriteLog(FileName_log, "a+", "Unknown type driver\n");
	WriteLog(FileName_log, "a+", "Feature Support : ");
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		WriteLog(FileName_log, "a+", "11.0 (highest)\n");
	else if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		WriteLog(FileName_log, "a+", "10.1 \n");
	else if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		WriteLog(FileName_log, "a+", "10.0 \n");
	else
		WriteLog(FileName_log, "a+", "Unknown \n");
	WriteLog(FileName_log, "a+", "\n");

	cleanup :
	if(pIDXGIAdapter) {
		pIDXGIAdapter->Release();
		pIDXGIAdapter = NULL;
	}
	if(pIDXGIFactory) {
		pIDXGIFactory->Release();
		pIDXGIFactory = NULL;
	}
}