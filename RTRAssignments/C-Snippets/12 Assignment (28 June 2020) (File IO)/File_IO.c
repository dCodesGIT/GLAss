// To demonstrate FILE I/O in CUI
// Date : 28 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

int main() {
	// Variable declaration
	FILE *fp = NULL;

	// Code
	if(fopen_s(&fp, "Log.txt", "w") != 0) {
		printf("\n Cannot open file 'Log.txt'.");
		exit(0);
	}
	printf("\n File opened successfully");
	fprintf(fp, "I love INDIA.");
	printf("\n File edited successfully");
	fclose(fp);
	fp = NULL;
	printf("\n File closed successfully");
	
	printf("\n\n");
	return 0;
}