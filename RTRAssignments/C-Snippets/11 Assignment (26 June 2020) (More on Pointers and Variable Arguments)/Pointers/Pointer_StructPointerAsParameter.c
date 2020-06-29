// To demonstrate pointers - structure pointer as parameters
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>

struct MyData {
	int i;
	float f;
	double d;
};

int main() {
	// Function declaration
	void ChangeStruct(struct MyData *);

	// Variable declaration
	struct MyData *pData = NULL;

	// Code
	pData = (struct MyData *)malloc(sizeof(struct MyData));
	if(pData == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}

	pData->i = 10;
	pData->f = 10.02f;
	pData->d = 2000.1002;
	printf("\n Data Members of 'struct MyData' before modification are : ");
	printf("\n i = %d", pData->i);
	printf("\n f = %f", pData->f);
	printf("\n d = %lf", pData->d);

	ChangeStruct(pData);
	printf("\n\n Data Members of 'struct MyData' after modification are : ");
	printf("\n i = %d", pData->i);
	printf("\n f = %f", pData->f);
	printf("\n d = %lf", pData->d);

	if(pData) {
		free(pData);
		pData = NULL;
	}

	printf("\n\n");
	return 0;
}

void ChangeStruct(struct MyData *pParamData) {
	// Code
	pParamData->i = 26;
	pParamData->f = 26.08f;
	pParamData->d = 2000.2608;
}