// Rectangle (using triangleStrip) in DirectX 3D
// By : Darshan Vikam
// Date : 14 August 2021

// Global header files
#include <Windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <d3dcompiler.h>		// For shader compilation
#pragma warning(disable : 4838)		// Suppressing Warning number 4838 (typecasting of unsigned int to signed int)
#include "../XNAMath/xnamath.h"		// XNA(XNA Not Acronym) Math (for functionalities of Maths)
#include "../Icon/WinIcon.h"

// Library linking
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

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

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;

struct CBUFFER {
	XMMATRIX WorldViewProjectionMatrix;		// Similar to Model View Projection Matrix in OpenGL
};
XMMATRIX gPerspProjMatrix;

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
	TCHAR szAppName[] = TEXT("RectangleInDirect3D11");
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

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("Rectangle in Direct3D 11"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, 100, 100, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);
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

	// Initialization shaders (HLSL-High Level Shading Language), input layout and constant buffer
	// Initialize, create, compile and set - Vertex Shader
	const char* VertexShaderSourceCode = 
		"cbuffer ConstantBuffer {" \
			"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"float4 main(float4 pos : POSITION) : SV_POSITION {" \
			"float4 position = mul(worldViewProjectionMatrix, pos);" \
			"return position;" \
		"}";
	ID3DBlob *pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;
	hr = D3DCompile(VertexShaderSourceCode, lstrlenA(VertexShaderSourceCode) +1, "VS", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &pID3DBlob_VertexShaderCode, &pID3DBlob_Error);
	if(FAILED(hr)) {
		if(pID3DBlob_Error != NULL) {		// Shader Error Check
			WriteLog(FileName_log, "a+", "D3DCompile() failed for Vertex Shader :\n");
			WriteLog(FileName_log, "a+", (char *)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else		// DirectX COM Error check
			WriteLog(FileName_log, "a+", "DirectX COM Error !!!");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "Vertex Shader Compiled Successfully...\n");
	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), NULL, &gpID3D11VertexShader);
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateVertexShader() failed !!!\n");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateVertexShader() succeeded...\n");
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);
	pID3DBlob_Error = NULL;

	// Initialize, create, compile and set - Pixel Shader
	const char* PixelShaderSourceCode = 
		"float4 main(void) : SV_TARGET {" \
			"return (float4(1.0, 1.0, 1.0, 1.0));" \
		"}";
	ID3DBlob *pID3DBlob_PixelShaderCode = NULL;
	hr = D3DCompile(PixelShaderSourceCode, lstrlenA(PixelShaderSourceCode) + 1, "PS", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &pID3DBlob_PixelShaderCode, &pID3DBlob_Error);
	if(FAILED(hr)) {
		if(pID3DBlob_Error != NULL) {		// Shader Error Check
			WriteLog(FileName_log, "a+", "D3DCompile() failed for Pixel Shader :\n");
			WriteLog(FileName_log, "a+", (char *)pID3DBlob_Error->GetBufferPointer());
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}
		else			// DirectX COM Error check
			WriteLog(FileName_log, "a+", "DirectX COM Error !!!\n");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "Pixel Shader Compiled Successfully...\n");
	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(), pID3DBlob_PixelShaderCode->GetBufferSize(), NULL, &gpID3D11PixelShader);
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "ID3D11Device::CreatePixelShader() failed !!!\n");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "ID3D11Device::CreatePixelShader() succeeded...\n");
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);
	pID3DBlob_Error = NULL;

	// Initialize, create and set - Input Layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc;
	ZeroMemory(&inputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));
	inputElementDesc.SemanticName = "POSITION";
	inputElementDesc.SemanticIndex = 0;
	inputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc.AlignedByteOffset = 0;
	inputElementDesc.InputSlot = 0;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc.InstanceDataStepRate = 0;
	hr = gpID3D11Device->CreateInputLayout(&inputElementDesc, 1, pID3DBlob_VertexShaderCode->GetBufferPointer(), pID3DBlob_VertexShaderCode->GetBufferSize(), &gpID3D11InputLayout);
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "ID3D11Device::CreatInputLayout() failed !!!\n");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateInputLayout() succeeded..\n");
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	// Initialize arrays of vertices, color, etc...
	const float rectangleVertices[] = {
		-1.0f, 1.0f, 0.0f,	// Left top
		1.0f, 1.0f, 0.0f,	// Right top
		-1.0f, -1.0f, 0.0f,	// Left bottom
		1.0f, -1.0f, 0.0f	// Right bottom
	};

	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory((void *)&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * _ARRAYSIZE(rectangleVertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer);
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateBuffer() failed for vertex buffer !!!\n");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateBuffer() succeeded for vertex buffer...\n");

	// Copy rectangleVertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory((void *)&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, rectangleVertices, sizeof(rectangleVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer, 0);

	// Create and set constant buffer
	ZeroMemory((void *)&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_ConstantBuffer);
	if(FAILED(hr)) {
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateBuffer() failed for constant buffer !!!\n");
		return hr;
	}
	else
		WriteLog(FileName_log, "a+", "ID3D11Device::CreateBuffer() succeeded for constant buffer...\n");
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	// D3D Clear color (black)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	// Set projection matrix to identity
	gPerspProjMatrix = XMMatrixIdentity();

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

	// Set Projection Matrix
	gPerspProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

	return hr;
}

void Display(void) {
	// Code
	// Clear render target view to chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	// Set vertex buffer to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer, &stride, &offset);

	// Set geometry primitive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// translation is converted with world matrix transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();
	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);
	worldMatrix *= translationMatrix;
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspProjMatrix;

	// Load data into constant buffer
	CBUFFER constantBuffer;
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// Draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(4, 0);

	// Switch between buffers
	gpIDXGISwapChain->Present(0, 0);
}

void Uninitialize(void) {
	// Code
	if(gpID3D11Buffer_ConstantBuffer) {
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}
	if(gpID3D11InputLayout) {
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}
	if(gpID3D11Buffer_VertexBuffer) {
		gpID3D11Buffer_VertexBuffer->Release();
		gpID3D11Buffer_VertexBuffer = NULL;
	}
	if(gpID3D11PixelShader) {
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}
	if(gpID3D11VertexShader) {
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}
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
	WriteLog(FileName_log, "a+", "\nLog file closed successfully...\n");
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