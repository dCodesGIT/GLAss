// To demonstrate pointers - string copy using pointers
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define MAX_STR_LEN 512

int main() {
	// Function declaration
	int StringLength(char *);
	void StringCopy(char *, char *);

	// Variable declaration
	int len = 0;
	char *cArray = NULL, *cArray_copy = NULL;

	// Code
	cArray = (char *)malloc(MAX_STR_LEN * sizeof(char));
	if(cArray == NULL) {
		printf("\n Memory allocation for string failed!!!");
		exit(0);
	}

	printf("\n Enter a string : ");
	gets_s(cArray, MAX_STR_LEN);

	len = StringLength(cArray);
	cArray_copy = (char *)malloc(sizeof(char) * len);
	if(cArray_copy == NULL) {
		printf("\n Memory allocation for string failed!!!");
		exit(0);
	}

	StringCopy(cArray_copy, cArray);
	printf("\n\n Original string entered by you is : ");
	printf("\n %s", cArray);
	printf("\n\n Copied string is :");
	printf("\n %s", cArray_copy);

	if(cArray_copy) {
		free(cArray_copy);
		cArray_copy = NULL;
		printf("\n\n Memory of copied string deallocated successfully");
	}
	if(cArray) {
		free(cArray);
		cArray = NULL;
		printf("\n Memory of original string deallocated successfully");
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

void StringCopy(char *dest, char *src) {
	// Function declaration
	int StringLength(char *);

	// Variable declaration
	int i, src_str_len;

	// Code
	src_str_len = StringLength(src);
	for(i = 0; i < src_str_len; i++)
		*(dest + i) = *(src + i);
	*(dest + i) = '\0';
}