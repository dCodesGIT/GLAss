// To demonstrate pointer - Arrays as pointer
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int i;
	int iArray[] = { 10,20,30,40,50,60,70,80,90,100 };
	float fArray[] = { 1.2f,2.3f,3.4f,4.5f,5.6f };
	double dArray[] = { 1.23456789,9.87654321,2.46810 };
	char cArray[] = "Darshan Vikam";

	// Code
	printf("\n Integer array elements and their addresses are as follows");
	for(i = 0; i < 10; i++)
		printf("\n iArray[%d] = %d \t\t Address = %p", i, *(iArray + i), (iArray + i));

	printf("\n\n Float array elements and their addresses are as follows");
	for(i = 0; i < 5; i++)
		printf("\n fArray[%d] = %f \t\t Address = %p", i, *(fArray + i), (fArray + i));

	printf("\n\n Double array elements and their addresses are as follows");
	for(i = 0; i < 3; i++)
		printf("\n dArray[%d] = %lf \t\t Address = %p", i, *(dArray + i), (dArray + i));

	printf("\n\n Character array elements and their addresses are as follows");
	for(i = 0; i < 13; i++)
		printf("\n cArray[%d] = %c \t\t Address = %p", i, *(cArray + i), (cArray + i));

	printf("\n\n");
	return 0;
}