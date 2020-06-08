// To demonstrate 1 dimensional array - piece mean initialization and display
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int iArray1[10], iArray2[10];

	//code
	iArray1[0] = 3;
	iArray1[1] = 6;
	iArray1[2] = 9;
	iArray1[3] = 12;
	iArray1[4] = 15;
	iArray1[5] = 18;
	iArray1[6] = 21;
	iArray1[7] = 24;
	iArray1[8] = 27;
	iArray1[9] = 30;

	printf("\n\n Piece meal (hard coded) initialization and display of elements of array 'iArray1[]'");
	printf("\n 1st element or element at 0th index of array 'iArray1[]' is %d", iArray1[0]);
	printf("\n 2nd element or element at 1st index of array 'iArray1[]' is %d", iArray1[1]);
	printf("\n 3rd element or element at 2nd index of array 'iArray1[]' is %d", iArray1[2]);
	printf("\n 4th element or element at 3rd index of array 'iArray1[]' is %d", iArray1[3]);
	printf("\n 5th element or element at 4th index of array 'iArray1[]' is %d", iArray1[4]);
	printf("\n 6th element or element at 5th index of array 'iArray1[]' is %d", iArray1[5]);
	printf("\n 7th element or element at 6th index of array 'iArray1[]' is %d", iArray1[6]);
	printf("\n 8th element or element at 7th index of array 'iArray1[]' is %d", iArray1[7]);
	printf("\n 9th element or element at 8th index of array 'iArray1[]' is %d", iArray1[8]);
	printf("\n 10th element or element at 9th index of array 'iArray1[]' is %d", iArray1[9]);

	printf("\n Enter 1st element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[0]);
	printf("\n Enter 2nd element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[1]);
	printf("\n Enter 3rd element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[2]);
	printf("\n Enter 4th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[3]);
	printf("\n Enter 5th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[4]);
	printf("\n Enter 6th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[5]);
	printf("\n Enter 7th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[6]);
	printf("\n Enter 8th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[7]);
	printf("\n Enter 9th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[8]);
	printf("\n Enter 10th element of array 'iArray2[]' : ");
	scanf("%d", &iArray2[9]);

	printf("\n\n Piece meal (hard coded) initialization and display of elements of array 'iArray2[]'");
	printf("\n 1st element or element at 0th index of array 'iArray2[]' is %d", iArray2[0]);
	printf("\n 2nd element or element at 1st index of array 'iArray2[]' is %d", iArray2[1]);
	printf("\n 3rd element or element at 2nd index of array 'iArray2[]' is %d", iArray2[2]);
	printf("\n 4th element or element at 3rd index of array 'iArray2[]' is %d", iArray2[3]);
	printf("\n 5th element or element at 4th index of array 'iArray2[]' is %d", iArray2[4]);
	printf("\n 6th element or element at 5th index of array 'iArray2[]' is %d", iArray2[5]);
	printf("\n 7th element or element at 6th index of array 'iArray2[]' is %d", iArray2[6]);
	printf("\n 8th element or element at 7th index of array 'iArray2[]' is %d", iArray2[7]);
	printf("\n 9th element or element at 8th index of array 'iArray2[]' is %d", iArray2[8]);
	printf("\n 10th element or element at 9th index of array 'iArray2[]' is %d", iArray2[9]);

	printf("\n\n");
	return 0;
}