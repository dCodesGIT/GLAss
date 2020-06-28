// To demonstrate pointers - using pointers as return value
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define MAX_STR_LEN 512

int main() {
	// Function declaration
	char *Replacer(char *);
	int StringLength(char *);

	// Variable declaration
	char str[MAX_STR_LEN];
	char *replaced = NULL;

	// Code
	printf("\n Enter a string : ");
	gets_s(str, MAX_STR_LEN);

	replaced = (char *)malloc(sizeof(char) * StringLength(str));
	if(replaced == NULL) {
		printf("\n Memory allocation failed!!!");
		exit(0);
	}
	replaced = Replacer(str);
	if(replaced == NULL) {
		printf("\n Function 'Replacer()' failed!!!");
		exit(0);
	}
	printf("\n Original String is : %s", str);
	printf("\n Replaced string is : %s", replaced);
	
	if(replaced) {
		free(replaced);
		replaced = NULL;
		printf("\n\n Memory of replaced string deallocated successfully..");
	}

	printf("\n\n");
	return 0;
}

// Function definition
char *Replacer(char *original) {
	// Function declaration
	void StringCopy(char *, char *);
	int StringLength(char *);

	// Variable declaration
	char *new_str = NULL;
	int i;

	// Code
	new_str = (char *)malloc(StringLength(original) * sizeof(char));
	if(new_str == NULL) {
		printf("\n Memory allocation for string modification failed !!!");
		return NULL;
	}
	StringCopy(new_str, original);
	for(i = 0; i < StringLength(new_str); i++) {
		switch(new_str[i]) {
			case 'a' :
			case 'A' :
			case 'e' :
			case 'E' :
			case 'i' :
			case 'I' :
			case 'o' :
			case 'O' :
			case 'u' :
			case 'U' :
				new_str[i] = '*';
				break;
			default :
				break;
		}
	}
	return(new_str);
}

void StringCopy(char *dest, char *src) {
	// Variable declaration
	int i;

	// Code
	for(i = 0; *(src + i) != '\0'; i++)
		*(dest + i) = *(src + i);
	*(dest + i) = '\0';
}

int StringLength(char *string) {
	// Variable declaration
	int n;

	// Code
	for(n = 0; n < MAX_STR_LEN; n++)
		if(string[n] == '\0')
			break;
	return n;
}