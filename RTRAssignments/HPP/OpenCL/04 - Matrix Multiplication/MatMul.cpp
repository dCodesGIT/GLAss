// Multiplying matrices of floats in OpenCL
// By : Darshan Vikam
// Date : 28 July 2021
//=============================================================================

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <CL/opencl.h>	// OpenCL specific header file
#include "../Include/helper_timer.h"
#include "../Include/ocl_kernel_loader.h"
#include "../Include/ocl_exit_error.h"
//-----------------------------------------------------------------------------

// Global variable declaration (for OpenCL)
cl_int			ret_ocl;
cl_platform_id		oclPlatformId;
cl_device_id		oclDeviceId;		// Compute Device ID
cl_context		oclContext;		// Compute Context
cl_command_queue	oclCommandQueue;	// Compute Command Queue
cl_program		oclProgram;		// Compute Program
cl_kernel		oclKernel;		// Compute Kernel

char *oclSrcCode = NULL;
size_t kernelCodeLength;

size_t localWorkSize = 256;
size_t globalWorkSize;

float *hostA = NULL;
float *hostB = NULL;
float *hostC = NULL;
float *CHost = NULL;

cl_mem deviceA = NULL;
cl_mem deviceB = NULL;
cl_mem deviceC = NULL;

float timeOnCPU, timeOnGPU;
//-----------------------------------------------------------------------------

// Entry point function - main()
int main() {
	// Function declaration
	void fillFloatArrayWithRandomNumbers(float *, int);
	size_t roundGlobalSizeToNearestMultipleOfLocalSize(int, unsigned int);
	void matMulHost(float *, float *, float *, int, int, int);
	void cleanup();

	// Variable declaration
	int numARows, numACols;
	int numBRows, numBCols;
	int numCRows, numCCols;
	int numCHostRows, numCHostCols;

	// Code
	numARows = 4;
	numACols = 4;
	numBRows = 4;
	numBCols = 4;
	numCRows = numARows;
	numCCols = numBCols;
	numCHostRows = numARows;
	numCHostCols = numBCols;
	
	int sizeA = numARows * numACols * sizeof(float);
	int sizeB = numBRows * numBCols * sizeof(float);
	int sizeC = numCRows * numCCols * sizeof(float);
	int sizeCHost = numCHostRows * numCHostCols * sizeof(float);
	
	hostA = (float *)malloc(sizeA);
	if(hostA == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host input matrix A.");
	
	hostB = (float *)malloc(sizeB);
	if(hostB == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host input matrix B.");
	
	hostC = (float *)malloc(sizeC);
	if(hostC == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host output matrix C.");
	
	CHost = (float *)malloc(sizeCHost);
	if(CHost == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for OpenCL output matrix CHost.");
	
	// Filling above host input arraay with hard coded values (arbitary values)
	fillFloatArrayWithRandomNumbers(hostA, numARows * numACols);
	fillFloatArrayWithRandomNumbers(hostB, numBRows * numBCols);
	
	// Get OpenCL supporting platform's id
	ret_ocl = clGetPlatformIDs(1, &oclPlatformId, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clGetPlatformIDs() failed", ret_ocl);
	
	// Get OpenCL supporting GPU device's id
	ret_ocl = clGetDeviceIDs(oclPlatformId, CL_DEVICE_TYPE_GPU, 1, &oclDeviceId, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clGetDeviceIDs() failed", ret_ocl);
	
	char gpu_name[256];
	clGetDeviceInfo(oclDeviceId, CL_DEVICE_NAME, sizeof(gpu_name), &gpu_name, NULL);
	printf("%s \n", gpu_name);
	
	// Create OpenCL compute context
	oclContext = clCreateContext(NULL, 1, &oclDeviceId, NULL, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateContext() failed", ret_ocl);
	
	// Create command queue
	oclCommandQueue = clCreateCommandQueue(oclContext, oclDeviceId, 0, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateCommandQueue() failed", ret_ocl);
	
	// Create OpenCl program from '.cl' file
	oclSrcCode = loadOCLProgram("MatMul.cl", "", &kernelCodeLength);
	oclProgram = clCreateProgramWithSource(oclContext, 1, (const char **)&oclSrcCode, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateProgramWithSource() failed", ret_ocl);
	
	// Build OpenCL program
	ret_ocl = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
	if(ret_ocl != CL_SUCCESS) {
		size_t len;
		clGetProgramBuildInfo(oclProgram, oclDeviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		char *buffer = NULL;
		buffer = (char *)malloc(len);
		if(buffer == NULL)
			exit(1);
		clGetProgramBuildInfo(oclProgram, oclDeviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
		printf(buffer);
		ocl_exit_error("clBuildProgram() failed : ", ret_ocl);
	}
	
	oclKernel = clCreateKernel(oclProgram, "matrixMultiply", &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateKernel() failed", ret_ocl);
	
	// Create buffer for arrays
	int size = numCRows * numCCols * sizeof(cl_float);
	deviceA = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateBuffer() for 1st array failed", ret_ocl);
	
	deviceB = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateBuffer() for 2nd array failed", ret_ocl);
	
	deviceC = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateBuffer() for output array failed", ret_ocl);
	
	// Set OpenCL kernel arguments
	ret_ocl = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&deviceA);		// 'deviceA' maps to 'A' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 1st argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void *)&deviceB);		// 'deviceB' maps to 'B' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 2nd argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void *)&deviceC);		// 'deviceC' maps to 'C' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 3rd argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void *)&numARows);		// 'numARows' maps to 'numARows' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 4, sizeof(cl_int), (void *)&numACols);		// 'numACols' maps to 'numACols' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 5, sizeof(cl_int), (void *)&numBRows);		// 'numBRows' maps to 'numBRows' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 6, sizeof(cl_int), (void *)&numBCols);		// 'numBCols' maps to 'numBCols' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 7, sizeof(cl_int), (void *)&numCRows);		// 'numCRows' maps to 'numCRows' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 8, sizeof(cl_int), (void *)&numCCols);		// 'numCCols' maps to 'numCCols' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	// Copy 'input' device buffer to device memory
	ret_ocl = clEnqueueWriteBuffer(oclCommandQueue, deviceA, CL_FALSE, 0, size, hostA, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueWriteBuffer() for 1st argument failed", ret_ocl);
	
	ret_ocl = clEnqueueWriteBuffer(oclCommandQueue, deviceB, CL_FALSE, 0, size, hostB, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueWriteBuffer() for 2nd argument failed", ret_ocl);
	
	// Run the OpenCL kernel
	globalWorkSize = roundGlobalSizeToNearestMultipleOfLocalSize(localWorkSize, (numCRows * numCCols));
	
	// Timer
	StopWatchInterface *timer = NULL;	// For time counter
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);			// Start timer
	ret_ocl = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 1, NULL, &globalWorkSize, &localWorkSize, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueNDRangeKernel() failed", ret_ocl);
	
	// Finish OpenCL command queue
	clFinish(oclCommandQueue);
	
	sdkStopTimer(&timer);			// Stop timer
	timeOnGPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
	
	// Read back result from device buffer to cpu buffer
	ret_ocl = clEnqueueReadBuffer(oclCommandQueue, deviceC, CL_TRUE, 0, size, hostC, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueReadBuffer() failed", ret_ocl);
	
	matMulHost(hostA, hostB, CHost, numACols, numCHostRows, numCHostCols);
	
	// Compare results for golden-host
	const float epsilon = 0.000001f;
	bool bAccuracy = true;
	int breakValue = 0;
	for(int i = 0; i < numARows * numACols; i++) {
		float val1 = CHost[i];
		float val2 = hostC[i];
		if(fabs(val1 - val2) > epsilon) {
			bAccuracy = false;
			breakValue = i;
			break;
		}
	}
	if(bAccuracy == false)
		printf("Break Value = %d\n", breakValue);
	
	// print results into a file
	FILE *fp_op = fopen("Output.txt", "w");
	fprintf(fp_op, "1st matrix is from 0th element %.6f to %dth element %.6f\n", hostA[0], (numARows * numACols) - 1, hostA[(numARows * numACols) - 1]);
	fprintf(fp_op, "2nd matrix is from 0th element %.6f to %dth element %.6f\n", hostB[0], (numBRows * numBCols) - 1, hostB[(numBRows * numBCols) - 1]);
	fprintf(fp_op, "Multiplication of above 2 matrices creates 3rd matrix as :\n");
	fprintf(fp_op, "3rd matrix is from 0th element %.6f to %dth element %.6f\n", hostC[0], (numCRows * numCCols) - 1, hostC[(numCRows * numCCols) - 1]);
	fprintf(fp_op, "Global work size = %u \n", (unsigned int)globalWorkSize);
	fprintf(fp_op, "Local work size = %u \n", (unsigned int)localWorkSize);
	fprintf(fp_op, "Time taken on CPU = %0.6f (ms) \n", timeOnCPU);
	fprintf(fp_op, "Time taken on GPU = %0.6f (ms) \n", timeOnGPU);
	if(bAccuracy)
		fprintf(fp_op, "Comparision of output arrays on CPU and GPU are accurate within the limit(%f)", epsilon);
	else
		fprintf(fp_op, "Not all comparision of output arrays on CPU and GPU are accurate within the limit(%f)", epsilon);
	fclose(fp_op);
	fp_op = NULL;
	
	// total clean up before exitting
	cleanup();
	
	return 0;
}
//-----------------------------------------------------------------------------

void fillFloatArrayWithRandomNumbers(float *pFloatArray, int iSize) {
	// Code
	const float fScale = 1.0f / (float)RAND_MAX;
	for(int i = 0; i < iSize; i++)
		pFloatArray[i] = fScale * rand();
}
//-----------------------------------------------------------------------------

size_t roundGlobalSizeToNearestMultipleOfLocalSize(int local_size, unsigned int global_size) {
	// Code
	unsigned int r = global_size % local_size;
	if(r == 0)
		return global_size;
	else
		return (global_size + local_size - r);
}
//-----------------------------------------------------------------------------

void matMulHost(float* A, float* B, float* C, int iACols, int iCRows, int iCCols) {
	// Code
	StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);
	
	for(int i = 0; i < iCRows; i++) {
		for(int j = 0; j < iCCols; j++) {
			float sum = 0.0f;
			for(int k = 0; k < iACols; k++) {
				float a = A[i * iACols + k];
				float b = B[k * iCCols + j];
				sum += a * b;
			}
			C[i * iCCols + j] = sum;
		}
	}
	
	sdkStopTimer(&timer);
	timeOnCPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
}
//-----------------------------------------------------------------------------

void cleanup() {
	// Code
	// Free OpenCL related memory
	if(oclSrcCode) {
		free((void *)oclSrcCode);
		oclSrcCode = NULL;
	}
	
	if(oclKernel) {
		clReleaseKernel(oclKernel);
		oclKernel = NULL;
	}
	
	if(oclProgram) {
		clReleaseProgram(oclProgram);
		oclProgram = NULL;
	}
	
	if(oclCommandQueue) {
		clReleaseCommandQueue(oclCommandQueue);
		oclCommandQueue = NULL;
	}
	
	if(oclContext) {
		clReleaseContext(oclContext);
		oclContext = NULL;
	}
	
	// Free device memory
	if(deviceA) {
		clReleaseMemObject(deviceA);
		deviceA = NULL;
	}
	
	if(deviceB) {
		clReleaseMemObject(deviceB);
		deviceB = NULL;
	}
	
	if(deviceC) {
		clReleaseMemObject(deviceC);
		deviceC = NULL;
	}
	
	// Free host-memory
	if(hostA) {
		free(hostA);
		hostA = NULL;
	}
	
	if(hostB) {
		free(hostB);
		hostB = NULL;
	}
	
	if(hostC) {
		free(hostC);
		hostC = NULL;
	}
	
	if(CHost) {
		free(CHost);
		CHost = NULL;
	}
}
//-----------------------------------------------------------------------------
