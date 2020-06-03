// To demonstrate entry point function - int main(int, char**, char**)
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main(int argc, char *argv[], char *envp[]) {		// 3 arguments, int return type
	// Variable declaration
	int i;

	// Code
	printf("\n Hello World !!!");
	printf("\n\n Number of command line arguments are %d", argc);
	printf("\n Command line arguments passed to this program are..");
	for(i = 0; i < argc; i++) {
		printf("\n\n Command line number : %d", i + 1);
		printf("\n\t Argument : %s", argv[i]);
	}
	printf("\n\n First 20 Environmental variables in this program are.. ");
	for(i = 0; i < argc; i++)
		printf("\n Environmental Variable number : %d \n\t %s", i + 1, envp[i]);

	printf("\n\n");
	return 0;
}