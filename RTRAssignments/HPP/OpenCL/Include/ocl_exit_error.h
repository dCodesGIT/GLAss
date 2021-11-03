// Header file for exit with error message (OpenCL specific)
// By : Darshan Vikam
//=============================================================================

// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <Cl/opencl.h>
//=============================================================================

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
	// Function declaration
	void cleanup();

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
//=============================================================================
