// To demonstrate global variable - External global variable in multiple file - main
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
#include "GlobalVariable_ExternalMultipleFile_01.c"
#include "GlobalVariable_ExternalMultipleFile_02.c"
int global_count = 0;
int main() {
	// Function declaration
	void change_count(void);
	void change_count_1(void);		// Function definition in another file - GlobalVariable_ExternalMultipleFile_01.c
	void change_count_2(void);		// Function definition in another file - GlobalVariable_ExternalMultipleFile_02.c

	// Code
	change_count();
	change_count_1();		// Function in another file - GlobalVariable_ExternalMultipleFile_01.c
	change_count_2();		// Function in another file - GlobalVariable_ExternalMultipleFile_02.c

	printf("\n\n");
	return 0;
}

// Function defintion
void change_count(void) {
	global_count += 1;
	printf("\n Global Count = %d", global_count);
}