// To demonstrate pointers - pointers within structure - using struct pointer - Method 1
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

struct MyData {
	int i;
	int *iptr;
	float f;
	float *fptr;
	double d;
	double *dptr;
};

int main() {
	// Variable declaration
	struct MyData *data_ptr = NULL;

	// Code
	data_ptr = (struct MyData *)malloc(sizeof(struct MyData));
	if(data_ptr == NULL) {
		printf("\n Memory allocation failed !!");
		exit(0);
	}
	(*data_ptr).i = 26;
	(*data_ptr).iptr = &(*data_ptr).i;
	(*data_ptr).f = 26.08;
	(*data_ptr).fptr = &(*data_ptr).f;
	(*data_ptr).d = 2000.2608;
	(*data_ptr).dptr = &(*data_ptr).d;

	printf("\n\n i = %d", *((*data_ptr).iptr));
	printf("\n Address of 'i' = %p", (*data_ptr).iptr);
	printf("\n\n f = %f", *((*data_ptr).fptr));
	printf("\n Address of 'f' = %p", (*data_ptr).fptr);
	printf("\n\n d = %lf", *((*data_ptr).dptr));
	printf("\n Address of 'd' = %p", (*data_ptr).dptr);

	if(data_ptr) {
		free(data_ptr);
		data_ptr = NULL;
	}

	printf("\n\n");
	return 0;
}