// To demonstrate global variable - External global variable in single file
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Function declaration
	void changeCount(void);

	// Variable declaration
	extern int global_count;

	// Code
	printf("\n Value of global_count before changeCount() : %d", global_count);
	changeCount();
	printf("\n Value of global_count after changeCount() : %d", global_count);

	printf("\n\n");
	return 0;
}

int global_count = 0;		// Global variable declaration

void changeCount(void) {
	global_count = 5;
	printf("\n Value of global_count in changeCount() : % d", global_count);
}