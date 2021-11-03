// Adding floats in OpenCL
// By : Darshan Vikam
// Date : 27 July 2021
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

int iNumberOfArrayElements = 114447770;	// Nvidia OpenCL sample
size_t localWorkSize = 256;
size_t globalWorkSize;

float *hostInput1 = NULL;
float *hostInput2 = NULL;
float *hostOutput = NULL;
float *gold = NULL;

cl_mem deviceInput1 = NULL;
cl_mem deviceInput2 = NULL;
cl_mem deviceOutput = NULL;

float timeOnCPU, timeOnGPU;
//-----------------------------------------------------------------------------

// Entry point function - main()
int main() {
	// Function declaration
	void fillFloatArrayWithRandomNumbers(float *, int);
	size_t roundGlobalSizeToNearestMultipleOfLocalSize(int, unsigned int);
	void vecAddHost(const float *, const float *, float *, int);
	void cleanup();
/*	char* loadOCLProgram(const char *, const char *, size_t *);
	void exit_error(char *);
	void ocl_exit_error(char *, cl_int);
*/
	// Code
	hostInput1 = (float *)malloc(iNumberOfArrayElements * sizeof(float));
	if(hostInput1 == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host input array 1.");
	
	hostInput2 = (float *)malloc(iNumberOfArrayElements * sizeof(float));
	if(hostInput2 == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host input array 2.");
	
	hostOutput = (float *)malloc(iNumberOfArrayElements * sizeof(float));
	if(hostOutput == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host output array.");
	
	gold = (float *)malloc(iNumberOfArrayElements * sizeof(float));
	if(gold == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for gold output array.");
	
	// Filling above host input arraay with hard coded values (arbitary values)
	fillFloatArrayWithRandomNumbers(hostInput1, iNumberOfArrayElements);
	fillFloatArrayWithRandomNumbers(hostInput2, iNumberOfArrayElements);
	
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
/*	const char *szOCLKernelPath = 
	"__kernel void vecAdd(__global float *in1, __global float *in2, __global float *out, int len) { \n" \
		"int i = get_global_id(0); \n" \
		"if(i < len) \n" \
			"out[i] = in1[i] + in2[i] \n;" \
	"} \n";
	oclProgram = clCreateProgramWithSource(oclContext, 1, &szOCLKernelPath, NULL, &ret_ocl);
*/
	oclSrcCode = loadOCLProgram("VecAdd.cl", "", &kernelCodeLength);
	oclProgram = clCreateProgramWithSource(oclContext, 1, (const char **)&oclSrcCode, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateProgramWithSource() failed", ret_ocl);
	
	// Build OpenCL program
	ret_ocl = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
	if(ret_ocl != CL_SUCCESS) {
		size_t len;
		char buffer[1024];
		clGetProgramBuildInfo(oclProgram, oclDeviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		ocl_exit_error(strcat("clBuildProgram() failed : ", buffer), ret_ocl);
	}
	
	oclKernel = clCreateKernel(oclProgram, "vecAdd", &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateKernel() failed", ret_ocl);
	
	// Create buffer for arrays
	int size = iNumberOfArrayElements * sizeof(cl_float);
	deviceInput1 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateBuffer() for 1st array failed", ret_ocl);
	
	deviceInput2 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateBuffer() for 2nd array failed", ret_ocl);
	
	deviceOutput = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateBuffer() for output array failed", ret_ocl);
	
	// Set OpenCL kernel arguments
	ret_ocl = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&deviceInput1);		// 'deviceInput1' maps to 'in1' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 1st argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void *)&deviceInput2);		// 'deviceInput2' maps to 'in2' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 2nd argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void *)&deviceOutput);		// 'deviceOutput' maps to 'out' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 3rd argument failed", ret_ocl);
	
	ret_ocl = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void *)&iNumberOfArrayElements);		// 'iNumberOfArrayElements' maps to 'len' in kernel
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clSetKernelArg() for 4th argument failed", ret_ocl);
	
	// Copy 'input' device buffer to device memory
	ret_ocl = clEnqueueWriteBuffer(oclCommandQueue, deviceInput1, CL_FALSE, 0, size, hostInput1, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueWriteBuffer() for 1st argument failed", ret_ocl);
	
	ret_ocl = clEnqueueWriteBuffer(oclCommandQueue, deviceInput2, CL_FALSE, 0, size, hostInput2, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueWriteBuffer() for 2nd argument failed", ret_ocl);
	
	// Run the OpenCL kernel
	globalWorkSize = roundGlobalSizeToNearestMultipleOfLocalSize(localWorkSize, iNumberOfArrayElements);
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
	ret_ocl = clEnqueueReadBuffer(oclCommandQueue, deviceOutput, CL_TRUE, 0, size, hostOutput, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueReadBuffer() failed", ret_ocl);
	
	vecAddHost(hostInput1, hostInput2, gold, iNumberOfArrayElements);
	
	// Compare results for golden-host
	const float epsilon = 0.000001f;
	bool bAccuracy = true;
	int breakValue = 0;
	for(int i = 0; i < iNumberOfArrayElements; i++) {
		float val1 = gold[i];
		float val2 = hostOutput[i];
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
	fprintf(fp_op, "Size of Array1 = %d \n", iNumberOfArrayElements);
	fprintf(fp_op, "Size of Array2 = %d \n", iNumberOfArrayElements);
	fprintf(fp_op, "Sum of each element from above 2 arrays creates 3rd array \n");
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

void vecAddHost(const float* pFloatArray1, const float* pFloatArray2, float* pFloatResult, int iNumElements) {
	// Code
	StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);
	
	for(int i = 0; i < iNumElements; i++)
		pFloatResult[i] = pFloatArray1[i] + pFloatArray2[i];
	
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
	if(deviceInput1) {
		clReleaseMemObject(deviceInput1);
		deviceInput1 = NULL;
	}
	
	if(deviceInput2) {
		clReleaseMemObject(deviceInput2);
		deviceInput2 = NULL;
	}
	
	if(deviceOutput) {
		clReleaseMemObject(deviceOutput);
		deviceOutput = NULL;
	}
	
	// Free host-memory
	if(hostInput1) {
		free(hostInput1);
		hostInput1 = NULL;
	}
	
	if(hostInput2) {
		free(hostInput2);
		hostInput2 = NULL;
	}
	
	if(hostOutput) {
		free(hostOutput);
		hostOutput = NULL;
	}
	
	if(gold) {
		free(gold);
		gold = NULL;
	}
}
//-----------------------------------------------------------------------------
