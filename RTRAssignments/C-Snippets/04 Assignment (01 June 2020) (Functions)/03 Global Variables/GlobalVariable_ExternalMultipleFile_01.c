// To demonstrate global variable - External global variable in multiple file - 01
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
extern int global_count;
void change_count_1(void) {
	// Code
	global_count += 1;
	printf("\n\n In change_count_1() of file GlobalVariable_ExternalMultipleFile_01.c");
	printf("\n Value of global_count = %d", global_count);
}