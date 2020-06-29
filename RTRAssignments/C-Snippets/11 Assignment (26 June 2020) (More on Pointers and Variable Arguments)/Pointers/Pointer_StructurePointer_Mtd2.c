// To demonstrate pointers - simple structure pointer - Method 2
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

struct MyData {
	int i;
	float f;
	double d;
};

int main() {
	// Variable declaration
	int i_size;
	int f_size;
	int d_size;
	int struct_size;
	int pointer_of_struct_size;
	struct MyData *ptrdata = NULL;

	// Code
	ptrdata = (struct MyData *)malloc(sizeof(struct MyData));
	if(ptrdata == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}

	ptrdata->i = 26;
	ptrdata->f = 26.08;
	ptrdata->d = 2000.2608;
	printf("\n Data Members of 'struct MyData' are");
	printf("\n i = %d", ptrdata->i);
	printf("\n f = %f", ptrdata->f);
	printf("\n d = %lf", ptrdata->d);

	i_size = sizeof(ptrdata->i);
	f_size = sizeof(ptrdata->f);
	d_size = sizeof(ptrdata->d);
	printf("\n\n Sizes (in Bytes) of data members of 'struct MyData' are");
	printf("\n Size of 'i' = %d bytes", i_size);
	printf("\n Size of 'f' = %d bytes", f_size);
	printf("\n Size of 'd' = %d bytes", d_size);

	struct_size = sizeof(struct MyData);
	pointer_of_struct_size = sizeof(ptrdata);
	printf("\n Size of 'struct MyData' = %d bytes", struct_size);
	printf("\n Size of pointer of 'struct MyData' = %d bytes", pointer_of_struct_size);

	if(ptrdata) {
		free(ptrdata);
		ptrdata = NULL;
	}

	printf("\n\n");
	return 0;
}