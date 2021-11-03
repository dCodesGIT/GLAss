// OpenCL(Open Compute Library) Device Properties
// Date : 31 March 2021
// By : Darshan Vikam
//=============================================================================

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <CL/opencl.h>		// OpenCL specific header file

// Global variable declaration
FILE *fp_OCLDevProp = NULL;

//-----------------------------------------------------------------------------
// Entry point function - main
void main(void) {
	// Function declaration
	void OpenCL_Device_Properties(void);

	// Code
	OpenCL_Device_Properties();
}

//-----------------------------------------------------------------------------
// Function definitions
void OpenCL_Device_Properties(void) {
	// Function declaration
	void OCL_exit_error(char *);

	// Variable declaration
	char oclPlatformInfo[512];

	// OpenCL specified variable declaration
	cl_int		ret_ocl;
	cl_platform_id	ocl_platform_id;
	cl_uint		dev_count;
	cl_device_id*	ocl_device_ids;

	// Code
	fp_OCLDevProp = fopen("OpenCL Device Properties.txt", "w");
	if(fp_OCLDevProp == NULL) {
		printf("\n Unable to open file 'OpenCL Device Properties'");
		printf("\n Exitting now... \n");
		exit(1);
	}

	fprintf(fp_OCLDevProp, " ***** OpenCL Information *****");

	// Get first platform ID
	ret_ocl = clGetPlatformIDs(1, &ocl_platform_id, NULL);
	if(ret_ocl != CL_SUCCESS)
		OCL_exit_error("clGetPlatformIDs() failed.");

	// Get GPU device count
	ret_ocl = clGetDeviceIDs(ocl_platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &dev_count);
	if(ret_ocl != CL_SUCCESS)
		OCL_exit_error("clGetDeviceIDs() failed.");
	else if(dev_count == 0)
		OCL_exit_error("No OpenCL supported device found.");
	else {
		// Get platform name
		clGetPlatformInfo(ocl_platform_id, CL_PLATFORM_NAME, 500, &oclPlatformInfo, NULL);
		fprintf(fp_OCLDevProp, "\n OpenCL supporting GPU platform name : %s", oclPlatformInfo);

		// Get platform version
		clGetPlatformInfo(ocl_platform_id, CL_PLATFORM_VERSION, 500, &oclPlatformInfo, NULL);
		fprintf(fp_OCLDevProp, "\n OpenCL supporting GPU platform version : %s", oclPlatformInfo);

		// Print number of supporting devices
		fprintf(fp_OCLDevProp, "\n Total number of OpenCL supporting device(s) on this system : %d", dev_count);

		// Allocate memory to hold device(s) IDs
		ocl_device_ids = (cl_device_id *)malloc(sizeof(cl_device_id)*dev_count);

		// Get IDs into allocated buffers
		clGetDeviceIDs(ocl_platform_id, CL_DEVICE_TYPE_GPU, dev_count, ocl_device_ids, NULL);

		char ocl_dev_prop[1024];
		for(int i = 0; i < dev_count; i++) {
			fprintf(fp_OCLDevProp, "\n\n ***** GPU DEVICE GENERAL INFORMATION *****");
			fprintf(fp_OCLDevProp, "\n GPU Device Number : %d", i+1);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_NAME, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Name : %s", ocl_dev_prop);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_VENDOR, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Vendor : %s", ocl_dev_prop);

			clGetDeviceInfo(ocl_device_ids[i], CL_DRIVER_VERSION, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Drive Version : %s", ocl_dev_prop);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_VERSION, sizeof(ocl_dev_prop), &ocl_dev_prop, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device OpenCL Version : %s", ocl_dev_prop);

			cl_uint clock_freq;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_freq), &clock_freq, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Max. Clock Rate : %u", clock_freq);

			cl_bool error_correction_support;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Error Correction Code (ECC) Support : %s", error_correction_support == CL_TRUE ? "Yes" : "No");

			fprintf(fp_OCLDevProp, "\n\n ***** GPU DEVICE MEMORY INFORMATION *****");
			cl_ulong mem_size;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Global Memory : %llu bytes", mem_size);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Local Memory : %llu bytes", mem_size);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Constant Buffer Size : %llu bytes", mem_size);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(mem_size), &mem_size, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Memory Allocation Size : %llu bytes", mem_size);

			fprintf(fp_OCLDevProp, "\n\n ***** GPU DEVICE COMPUTE INFORMATION *****");
			cl_uint compute_units;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Number of Parallel Processors Cores : %u", compute_units);

			size_t workgroup_size;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Work Group Size : %u", (unsigned int)workgroup_size);

			size_t workitem_dims;
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Work Item Dimensions : %u", (unsigned int)workitem_dims);

			size_t workitem_size[3];
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Work Item Sizes : %u %u %u", (unsigned int)workitem_size[0], (unsigned int)workitem_size[1], (unsigned int)workitem_size[2]);

			fprintf(fp_OCLDevProp, "\n\n ***** GPU DEVICE IMAGE SUPPORT *****");
			size_t szMaxDims[5];
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &szMaxDims[0], NULL);
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[1], NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Support 2D image (Width x Height) : %u x %u", (unsigned int)szMaxDims[0], (unsigned int)szMaxDims[1]);

			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &szMaxDims[2], NULL);
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[3], NULL);
			clGetDeviceInfo(ocl_device_ids[i], CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &szMaxDims[4], NULL);
			fprintf(fp_OCLDevProp, "\n GPU Device Support 3D image (Width x Height x Depth) : %u x %u x %u", (unsigned int)szMaxDims[2], (unsigned int)szMaxDims[3], (unsigned int)szMaxDims[4]);
		}
		free(ocl_device_ids);
		ocl_device_ids = NULL;
	}
	fclose(fp_OCLDevProp);
	fp_OCLDevProp = NULL;
}

void OCL_exit_error(char *error) {
	// Code
	fprintf(fp_OCLDevProp, "\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", error);
	fprintf(fp_OCLDevProp, "\n OpenCL Error - %s", error);
	fprintf(fp_OCLDevProp, "\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-", error);
	fclose(fp_OCLDevProp);
	fp_OCLDevProp = NULL;
	printf("\n OpenCL Error - %s.", error);
	printf("\n Exitting now..");
	exit(EXIT_FAILURE);
}
