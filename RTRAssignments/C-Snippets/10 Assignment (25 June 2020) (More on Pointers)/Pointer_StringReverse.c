// To demonstrate pointers - string reverse using pointers
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define MAX_STR_LEN 512

int main() {
	// Function declaration
	void StringReverse(char *, char *);
	int StringLength(char *);

	// Variable declaration
	int StrLen = 0;
	char *cArray_original = NULL, *cArray_reversed = NULL;

	// Code
	cArray_original = (char *)malloc(sizeof(char) * MAX_STR_LEN);
	if(cArray_original==NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}
	printf("\n Enter a string : ");
	gets_s(cArray_original, MAX_STR_LEN);

	StrLen = StringLength(cArray_original);
	cArray_reversed = (char *)malloc(sizeof(char) * StrLen);
	if(cArray_reversed == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}
	StringReverse(cArray_reversed, cArray_original);
	printf("\n Original string is : %s", cArray_original);
	printf("\n Reversed string is : %s", cArray_reversed);

	if(cArray_reversed) {
		free(cArray_reversed);
		cArray_reversed = NULL;
	}
	if(cArray_original) {
		free(cArray_original);
		cArray_original = NULL;
	}
	printf("\n\n Memory deallocated successfully!!!");

	printf("\n\n");
	return 0;
}

// Function definition
int StringLength(char *string) {
	// Variable declaration
	int i;

	// Code
	for(i = 0; i < MAX_STR_LEN; i++)
		if(*(string + i) == '\0')
			break;
	return i;
}

void StringReverse(char *dest, char *src) {
	// Function declaration
	int StringLength(char *);

	// Variable declaration
	int i, len;

	// Code
	len = StringLength(src);
	for(i = 0; i < len; i++)
		*(dest + i) = *(src + len - i - 1);
	*(dest + i) = '\0';
}