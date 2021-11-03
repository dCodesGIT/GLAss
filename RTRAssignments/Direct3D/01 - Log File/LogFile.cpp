// Log file in DirectX 3D
// By : Darshan Vikam
// Date : 31 July 2021

// Header files
#include <stdio.h>
#include <d3d11.h>

// Runtime linking of libraries
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")		// DirectX Graphics Infrastructure

// Global variables
FILE *fp_log = NULL;

// Entry point function - main()
int main() {
	// Variable declaration
	IDXGIFactory *pIDXGIFactory = NULL;
	IDXGIAdapter *pIDXGIAdapter = NULL;		// Physical representation of device -> Adapter
	DXGI_ADAPTER_DESC dxgiAdapterDesc;
	HRESULT hr;
	char str[255];

	// Code
	fp_log = fopen("log.txt", "w");
	if(fp_log == NULL) {
		printf("\n Unable to open log.txt");
		return 0;
	}

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);
	if(FAILED(hr)) {
		fprintf(fp_log, "CreateDXGIFactory failed..\n");
		goto cleanup;
	}

	if(pIDXGIFactory->EnumAdapters(0, &pIDXGIAdapter) == DXGI_ERROR_NOT_FOUND) {
		fprintf(fp_log, "DXGIAdapter not found..\n");
		goto cleanup;
	}

	ZeroMemory((void*)&dxgiAdapterDesc, sizeof(DXGI_ADAPTER_DESC));

	pIDXGIAdapter->GetDesc(&dxgiAdapterDesc);
	WideCharToMultiByte(CP_ACP, 0, dxgiAdapterDesc.Description, 255, str, sizeof(str), NULL, NULL);
	fprintf(fp_log, "Graphics Card Name : %s \n", str);
	fprintf(fp_log, "Graphics Card Video Memory : %d MB(%I64d bytes) \n", (dxgiAdapterDesc.DedicatedVideoMemory)/(1024*1024), (__int64)dxgiAdapterDesc.DedicatedVideoMemory);
	printf("Graphics Card Video Memory : %d MB(%I64d bytes) \n", (dxgiAdapterDesc.DedicatedVideoMemory)/(1024*1024), (__int64)dxgiAdapterDesc.DedicatedVideoMemory);

	cleanup :
	if(pIDXGIAdapter) {
		pIDXGIAdapter->Release();
		pIDXGIAdapter = NULL;
	}
	if(pIDXGIFactory) {
		pIDXGIFactory->Release();
		pIDXGIFactory = NULL;
	}
	if(fp_log) {
		fclose(fp_log);
		fp_log = NULL;
	}
	return 0;
}
