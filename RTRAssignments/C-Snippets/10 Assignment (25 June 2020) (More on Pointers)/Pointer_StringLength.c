// To demonstrate pointers - calculate string length using pointers
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define MAX_STR_LEN 512

int main() {
	// Function declaration
	int StringLength(char *);

	// Variable declaration
	int len = 0;
	char *cArray = NULL;

	// Code
	cArray = (char *)malloc(MAX_STR_LEN * sizeof(char));
	if(cArray == NULL) {
		printf("\n Memory allocation for string failed!!!");
		exit(0);
	}

	printf("\n Enter a string : ");
	gets_s(cArray, MAX_STR_LEN);

	printf("\n\n String entered by you is : ");
	printf("\n %s", cArray);

	len = StringLength(cArray);
	printf("\n\n Length of string = %d characters", len);

	if(cArray) {
		free(cArray);
		cArray = NULL;
		printf("\n\n Memory deallocated successfully");
	}

	printf("\n\n");
	return 0;
}

// Function definition
int StringLength(char *string) {
	// Variable declaration
	int i;

	// Code
	for(i = 0; i < MAX_STR_LEN; i++) {
		if(*(string + i) == '\0')
			break;
	}
	return i;
}