// To demonstrate structures - arrays as data member of structure
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>
#define INT_ARRAY_SIZE 10
#define FLOAT_ARRAY_SIZE 5
#define CHAR_ARRAY_SIZE 26

#define NUM_STRINGS 10
#define STRINGS_SIZE 20

#define ALPHABET_BEGINNING 65

struct MyDataOne {
	int iArray[INT_ARRAY_SIZE];
	float fArray[FLOAT_ARRAY_SIZE];
};

struct MyDataTwo {
	char cArray[CHAR_ARRAY_SIZE];
	char strArray[NUM_STRINGS][STRINGS_SIZE];
};

int main() {
	// Variable declaration
	struct MyDataOne data_one;
	struct MyDataTwo data_two;
	int i;

	// Code
	data_one.fArray[0] = 2.6f;
	data_one.fArray[1] = 0.8f;
	data_one.fArray[2] = 2.0f;
	data_one.fArray[3] = 20.19f;
	data_one.fArray[4] = 26.08f;

	printf("\n Enter %d integers : \n", INT_ARRAY_SIZE);
	for(i = 0; i < INT_ARRAY_SIZE; i++) {
		printf(" %d : ", i + 1);
		scanf("%d", &data_one.iArray[i]);
	}

	for(i = 0;i < CHAR_ARRAY_SIZE; i++)
		data_two.cArray[i] = (char)(i + ALPHABET_BEGINNING);

	strcpy(data_two.strArray[0], "Hello");
	strcpy(data_two.strArray[1], "World");
	strcpy(data_two.strArray[2], "\n");
	strcpy(data_two.strArray[3], "This");
	strcpy(data_two.strArray[4], "is");
	strcpy(data_two.strArray[5], "RTR");
	strcpy(data_two.strArray[6], "batch");
	strcpy(data_two.strArray[7], "2020");
	strcpy(data_two.strArray[8], "of");
	strcpy(data_two.strArray[9], "AstroMediComp");

	printf("\n Members of 'struct data_one' are");
	printf("\n Integer array");
	for(i = 0; i < INT_ARRAY_SIZE; i++)
		printf("\n\t data_one.iArray[%d] = %d", i, data_one.iArray[i]);
	printf("\n Float array");
	for(i = 0; i < FLOAT_ARRAY_SIZE; i++)
		printf("\n\t data_one.fArray[%d] = %f", i, data_one.fArray[i]);

	printf("\n\n Members of struct 'data_two' are");
	printf("\n Character array");
	for(i = 0; i < CHAR_ARRAY_SIZE; i++)
		printf("\n\t data_two.cArray[%d] = %c", i, data_two.cArray[i]);
	printf("\n String array\n");
	for(i = 0; i < NUM_STRINGS; i++)
		printf(" %s", data_two.strArray[i]);

	printf("\n\n");
	return 0;
}