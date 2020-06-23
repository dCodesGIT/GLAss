// To demonstrate pointer - data types and pointer sizes
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
struct Employee {
	char name[20];
	int age;
	float salary;
	char sex;
	char marital_status;
};

int main() {
	// Code
	printf("\n Sizes of data types and its respective pointers are given below");
	printf("\n\n Data type \t\t Size (in Bytes) \t Pointer size (in Bytes)");
	printf("\n Int \t\t\t %d \t\t\t %d", sizeof(int), sizeof(int *));
	printf("\n Char \t\t\t %d \t\t\t %d", sizeof(char), sizeof(char *));
	printf("\n Float \t\t\t %d \t\t\t %d", sizeof(float), sizeof(float *));
	printf("\n Double \t\t %d \t\t\t %d", sizeof(double), sizeof(double *));
	printf("\n Struct Employee \t %d \t\t\t %d", sizeof(struct Employee), sizeof(struct Employee *));

	printf("\n\n");
	return 0;
}