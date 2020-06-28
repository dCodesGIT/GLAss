// To demonstrate pointers - string concatenation using pointers
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define MAX_STR_LEN 512

int main() {
	// Function declaration
	void StringConcatenate(char *, char *);
	int StringLength(char *);

	// Variable declaration
	int StrLen = 0;
	char *cArray_one = NULL, *cArray_two = NULL;

	// Code
	cArray_one = (char *)malloc(sizeof(char) * MAX_STR_LEN);
	if(cArray_one == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}
	printf("\n Enter a string : ");
	gets_s(cArray_one, MAX_STR_LEN);

	cArray_two = (char *)malloc(sizeof(char) * MAX_STR_LEN);
	if(cArray_two == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}
	printf("\n Enter another string : ");
	gets_s(cArray_two, MAX_STR_LEN);

	printf("\n *** BEFORE CONCATENATION ***");
	printf("\n Original string 'cArrsy_one' is : %s", cArray_one);
	printf("\n Original string 'cArrsy_two' is : %s", cArray_two);
	StringConcatenate(cArray_one, cArray_two);
	printf("\n\n *** AFTER CONCATENATION ***");
	printf("\n Modified string 'cArray_one' is : %s", cArray_one);
	printf("\n Modified string 'cArray_two' is : %s", cArray_two);

	if(cArray_two) {
		free(cArray_two);
		cArray_two = NULL;
	}
	if(cArray_one) {
		free(cArray_one);
		cArray_one= NULL;
	}
	printf("\n Memory deallocated successfully!!!");

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

void StringConcatenate(char *str_one, char *str_two) {
	// Function declaration
	int StringLength(char *);

	// Variable declaration
	int i, len_one, len_two;

	// Code
	len_one = StringLength(str_one);
	len_two = StringLength(str_two);
	for(i = 0; i < len_two; i++)
		*(str_one + len_one + i) = *(str_two + i);
	*(str_one + i + len_one) = '\0';
}