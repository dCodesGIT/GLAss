// To demonstrate entry point function - application 1
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
int main(int argc, char *argv[], char *envp[]) {
	// Variable declaration
	int i, num, sum = 0;

	// Code
	if(argc == 1) {
		printf("\n No numbers given for addition !!! \n Exitting now...");
		printf("\n Usage : <filename> <first number> <second number>");
		printf("\n\n");
		exit(0);
	}
	printf("\n Sum of all the numbers in the command line arguments is : ");
	for(i = 1; i < argc; i++) {
		num = atoi(argv[i]);
		sum = sum + num;
	}
	printf("%d", sum);

	printf("\n\n");
	return 0;
}