// Adding 5 floats in OpenCL
// By : Darshan Vikam
// Date : 26 July 2021
//=============================================================================

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/opencl.h>	// OpenCL specific header file
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

float *hostInput1 = NULL;
float *hostInput2 = NULL;
float *hostOutput = NULL;

cl_mem deviceInput1 = NULL;
cl_mem deviceInput2 = NULL;
cl_mem deviceOutput = NULL;
//-----------------------------------------------------------------------------

// Entry point function - main()
int main() {
	// Function declaration
	void cleanup();
	char* loadOCLProgram(const char *, const char *, size_t *);
	void exit_error(char *);
	void ocl_exit_error(char *, cl_int);
	
	// Variable declaration
	int inputLen;
	
	// Code
	inputLen = 5;		// hard coded host vector length
	hostInput1 = (float *)malloc(inputLen * sizeof(float));
	if(hostInput1 == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host input array 1.");
	
	hostInput2 = (float *)malloc(inputLen * sizeof(float));
	if(hostInput2 == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host input array 2.");
	
	hostOutput = (float *)malloc(inputLen * sizeof(float));
	if(hostOutput == NULL)
		exit_error("CPU memory fatal error: Cannot allocate memory for host output array.");
	
	// Filling above host input arraay with hard coded values (arbitary values)
	hostInput1[0] = 100.0;
	hostInput1[1] = 200.0;
	hostInput1[2] = 300.0;
	hostInput1[3] = 400.0;
	hostInput1[4] = 500.0;
	
	hostInput2[0] = 100.0;
	hostInput2[1] = 200.0;
	hostInput2[2] = 300.0;
	hostInput2[3] = 400.0;
	hostInput2[4] = 500.0;
	
	// Get OpenCL supporting platform's id
	ret_ocl = clGetPlatformIDs(1, &oclPlatformId, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clGetPlatformIDs() failed", ret_ocl);
	
	// Get OpenCL supporting GPU device's id
	ret_ocl = clGetDeviceIDs(oclPlatformId, CL_DEVICE_TYPE_GPU, 1, &oclDeviceId, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clGetDeviceIDs() failed", ret_ocl);
	
	// Create OpenCL compute context
	oclContext = clCreateContext(NULL, 1, &oclDeviceId, NULL, NULL, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateContext() failed", ret_ocl);
	
	// Create command queue
	oclCommandQueue = clCreateCommandQueue(oclContext, oclDeviceId, 0, &ret_ocl);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clCreateCommandQueue() failed", ret_ocl);
	
	// Create OpenCl program from '.cl' file
	oclSrcCode = loadOCLProgram("VecAdd.cl", "", &kernelCodeLength);
//	printf("%s", oclSrcCode);
	oclProgram = clCreateProgramWithSource(oclContext, 1, (char **)&oclSrcCode, NULL, &ret_ocl);

/*	const char *szOCLKernelPath = 
	"__kernel void vecAdd(__global float *in1, __global float *in2, __global float *out, int len) { \n" \
		"int i = get_global_id(0); \n" \
		"if(i < len) \n" \
			"out[i] = in1[i] + in2[i]; \n" \
	"} \n";
	oclProgram = clCreateProgramWithSource(oclContext, 1, &szOCLKernelPath, NULL, &ret_ocl);
*/	if(ret_ocl != CL_SUCCESS)
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
	int size = inputLen * sizeof(cl_float);
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
	
	ret_ocl = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void *)&inputLen);		// 'inputLen' maps to 'len' in kernel
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
	size_t global_size = 5;
	ret_ocl = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueNDRangeKernel() failed", ret_ocl);
	
	// Finish OpenCL command queue
	clFinish(oclCommandQueue);
	
	// Read back result from device buffer to cpu buffer
	ret_ocl = clEnqueueReadBuffer(oclCommandQueue, deviceOutput, CL_TRUE, 0, size, hostOutput, 0, NULL, NULL);
	if(ret_ocl != CL_SUCCESS)
		ocl_exit_error("clEnqueueReadBuffer() failed", ret_ocl);
	
	// print results into a file
	FILE *fp_op = fopen("Output.txt", "w");
	for(int i = 0; i < inputLen; i++)
		fprintf(fp_op, "%f + %f = %f \n", hostInput1[i], hostInput2[i], hostOutput[i]);
	fclose(fp_op);
	fp_op = NULL;
	
	// total clean up before exitting
	cleanup();
	
	return 0;
}
//-----------------------------------------------------------------------------

void exit_error(char *err_msg) {
	// Code
	FILE *fp = fopen("ErrorLog.txt", "w");
	if(fp == NULL) {
		printf("\n Unable to open file for error log.");
		return;
	}
	
	fprintf(fp, "\n Error : %s", err_msg);
	
	fclose(fp);
	fp = NULL;
	
	exit(EXIT_FAILURE);
}

//-----------------------------------------------------------------------------

void ocl_exit_error(char *error, cl_int err_no) {
	// Code
	FILE *fp = fopen("ErrorLog.txt", "w");
	if(fp == NULL) {
		printf("\n Unable to open file for error log.");
		return;
	}
	
	fprintf(fp, " Error no : %d", (int)err_no);
	fprintf(fp, "\n Error : %s", error);
	
	fclose(fp);
	fp = NULL;
	
	cleanup();
	
	exit(EXIT_FAILURE);
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
}
//-----------------------------------------------------------------------------

char *loadOCLProgram(const char *filename, const char *preamble, size_t *finalLength) {
	// Variable declaration
	FILE *fp = NULL;
	size_t srcLen;
	
	// Code
	fp = fopen(filename, "rb");
	if(fp == NULL)
		return NULL;
	
	size_t preambleLen = (size_t)strlen(preamble);
	fseek(fp, 0, SEEK_END);
	srcLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char *srcString = (char *)malloc(srcLen + preambleLen + 1);
	memcpy(srcString, preamble, preambleLen);
	if(fread((srcString) + preambleLen, srcLen, 1, fp) != 1) {
		fclose(fp);
		free(srcString);
		return NULL;
	}
	
	fclose(fp);
	if(finalLength != 0)
		*finalLength = srcLen + preambleLen;
	
	srcString[srcLen + preambleLen] = '\0';
	
	return srcString;
}
//-----------------------------------------------------------------------------
