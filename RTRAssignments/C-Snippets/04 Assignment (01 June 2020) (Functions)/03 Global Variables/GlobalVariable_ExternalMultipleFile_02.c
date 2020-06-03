// To demonstrate global variable - External global variable in multiple file - 02
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
void change_count_2(void) {
	// Global Variable declaration
	extern int global_count;

	// Code
	global_count += 1;
	printf("\n\n In change_count_2() of file GlobalVariable_ExternalMultipleFile_02.c");
	printf("\n Value of global_count = %d", global_count);
}