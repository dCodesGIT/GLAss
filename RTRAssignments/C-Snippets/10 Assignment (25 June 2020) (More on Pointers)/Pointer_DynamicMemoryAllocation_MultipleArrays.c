// To demonstrate pointers - Memory allocation and deallocation for multiple arrays using malloc() and free()
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define INT_SIZE sizeof(int)
#define FLOAT_SIZE sizeof(float)
#define DOUBLE_SIZE sizeof(double)
#define CHAR_SIZE sizeof(char)

int main() {
	// Variable declaration
	int i;
	int *iArray_ptr = NULL;
	float *fArray_ptr = NULL;
	double *dArray_ptr = NULL;
	char *cArray_ptr = NULL;
	unsigned int iArray_length = 0, fArray_length = 0, dArray_length = 0, cArray_length = 0;

	// Code
	printf("\n Enter the number of integer elements you want in array : ");
	scanf("%d", &iArray_length);
	iArray_ptr = (int *)malloc(sizeof(int) * iArray_length);
	if(iArray_ptr == NULL) {
		printf(" Memory allocation failed !!!");
		printf("\n Exiting now");
		exit(0);
	}
	printf(" Memory allocation succeded");
	printf("\n Addresses from %p to %p have been allocated.", iArray_ptr, iArray_ptr + (iArray_length - 1));
	printf("\n Enter %d elements for integer array\n", iArray_length);
	for(i = 0; i < iArray_length; i++) {
		printf(" %d. ", i + 1);
		scanf("%d", iArray_ptr + i);
	}

	printf("\n Enter the number of float elements you want in array : ");
	scanf("%d", &fArray_length);
	fArray_ptr = (float *)malloc(sizeof(float) * fArray_length);
	if(fArray_ptr == NULL) {
		printf(" Memory allocation failed !!!");
		printf("\n Exiting now");
		exit(0);
	}
	printf(" Memory allocation succeded");
	printf("\n Addresses from %p to %p have been allocated.", fArray_ptr, fArray_ptr + (fArray_length - 1));
	printf("\n Enter %d elements for float array\n", fArray_length);
	for(i = 0; i < fArray_length; i++) {
		printf(" %d. ", i + 1);
		scanf("%f", fArray_ptr + i);
	}

	printf("\n Enter the number of double elements you want in array : ");
	scanf("%d", &dArray_length);
	dArray_ptr = (double *)malloc(sizeof(double) * dArray_length);
	if(dArray_ptr == NULL) {
		printf(" Memory allocation failed !!!");
		printf("\n Exiting now");
		exit(0);
	}
	printf(" Memory allocation succeded");
	printf("\n Addresses from %p to %p have been allocated.", dArray_ptr, dArray_ptr + (dArray_length - 1));
	printf("\n Enter %d elements for double array\n", dArray_length);
	for(i = 0; i < dArray_length; i++) {
		printf(" %d. ", i + 1);
		scanf("%lf", dArray_ptr + i);
	}

	printf("\n Enter the number of character elements you want in array : ");
	scanf("%d", &cArray_length);
	cArray_ptr = (char *)malloc(sizeof(char) * cArray_length);
	if(cArray_ptr == NULL) {
		printf(" Memory allocation failed !!!");
		printf("\n Exiting now");
		exit(0);
	}
	printf(" Memory allocation succeded");
	printf("\n Addresses from %p to %p have been allocated.", cArray_ptr, cArray_ptr + (cArray_length - 1));
	printf("\n Enter %d elements for character array\n", cArray_length);
	for(i = 0; i < cArray_length; i++) {
		cArray_ptr[i] = getche();
	}

	printf("\n\n The integer array you entered is");
	for(i = 0; i < iArray_length; i++)
		printf("\n iArray_ptr[%d] = %d \t Address = %p", i, *(iArray_ptr + i), (iArray_ptr + i));
	printf("\n\n The float array you entered is");
	for(i = 0; i < fArray_length; i++)
		printf("\n fArray_ptr[%d] = %f \t Address = %p", i, *(fArray_ptr + i), (fArray_ptr + i));
	printf("\n\n The double array you entered is");
	for(i = 0; i < dArray_length; i++)
		printf("\n dArray_ptr[%d] = %lf \t Address = %p", i, *(dArray_ptr + i), (dArray_ptr + i));
	printf("\n\n The character array you entered is");
	for(i = 0; i < cArray_length; i++)
		printf("\n cArray_ptr[%d] = %c \t Address = %p", i, *(cArray_ptr + i), (cArray_ptr + i));

	if(cArray_ptr) {
		free(cArray_ptr);
		cArray_ptr = NULL;
		printf("\n Memory of cArray_ptr deallocated successfully...");
	}
	if(dArray_ptr) {
		free(dArray_ptr);
		dArray_ptr = NULL;
		printf("\n Memory of dArray_ptr deallocated successfully...");
	}
	if(fArray_ptr) {
		free(fArray_ptr);
		fArray_ptr = NULL;
		printf("\n Memory of fArray_ptr deallocated successfully...");
	}
	if(iArray_ptr) {
		free(iArray_ptr);
		iArray_ptr = NULL;
		printf("\n Memory of iArray_ptr deallocated successfully...");
	}

	printf("\n\n");
	return 0;
}