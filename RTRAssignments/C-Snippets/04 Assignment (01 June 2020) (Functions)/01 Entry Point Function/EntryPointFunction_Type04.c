// To demonstrate entry point function - int main(int, char**)
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>		// 'stdio.h' contains declaration of printf()
int main(int argc, char *argv[]) {		// 2 arguments, int return type
	// Variable declaration
	int i;
										
	// Code
	printf("\n Hello World !!!");
	printf("\n Number of command line arguments are %d", argc);
	printf("\n Command line arguments passed to this program are..");
	for(i = 0; i < argc; i++) {
		printf("\n Command line argument number : %d", i + 1);
		printf("\n\t Argument : %s", argv[i]);
	}

	printf("\n\n");
	return 0;
}