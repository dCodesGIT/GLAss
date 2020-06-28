// To demonstrate pointer to pointer - Sizes of pointer to pointer of different data types
// Date : 25 June 2020
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
	printf("\n Sizes of data types and its pointers are given below.");
	printf("\n\n Data Type \t\t Size \t Size of pointer \t Size of pointer of pointer");
	printf("\n Int \t\t\t %d \t\t %d \t\t\t %d", sizeof(int), sizeof(int *), sizeof(int **));
	printf("\n Float \t\t\t %d \t\t %d \t\t\t %d", sizeof(float), sizeof(float *), sizeof(float **));
	printf("\n Double \t\t %d \t\t %d \t\t\t %d", sizeof(double), sizeof(double *), sizeof(double **));
	printf("\n Char \t\t\t %d \t\t %d \t\t\t %d", sizeof(char), sizeof(char *), sizeof(char **));
	printf("\n Struct Employee \t %d \t\t %d \t\t\t %d", sizeof(struct Employee), sizeof(struct Employee *), sizeof(struct Employee **));

	printf("\n\n");
	return 0;
}