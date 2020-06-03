// To demonstrate entry point function - application 2
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[], char *envp[]) {
	// Variable declaration
	int i;

	// Code
	if(argc != 4) {
		printf("\n Invalid usage !! Exiting now...");
		printf("\n Usage : <filename> < first name> <middle name> <last name>");
		printf("\n\n");
		exit(0);
	}
	printf("\n Your full name is : ");
	for(i = 1; i < argc; i++)
		printf(" %s", argv[i]);

	printf("\n\n");
	return 0;
}