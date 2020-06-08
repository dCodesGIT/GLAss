// To demonstrate 1 dimensional array - inline initialization and display using loops
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int intArr[] = { 10,02,20,00,26,36 };
	int int_size, intArr_size, intArr_n;
	float fArr[] = { 10.02f,2.2000f,26.08f,08.2000f,36.10f };
	int float_size, fArr_size, fArr_n;
	char	charArr[] = { 'D','a','r','s','h','a','n',' ','V','i','k','a','m','\0' };
	int char_size, charArr_size, charArr_n;
	int i;

	// Code
	printf("\n\n Inline initialization and displaying elements of array 'intArr[]' using loops");
	int_size = sizeof(int);
	intArr_size = sizeof(intArr);
	intArr_n = intArr_size / int_size;
	for(i = 0; i < intArr_n; i++)
		printf("\n intArr[%d] - element %d : %d", i, i + 1, intArr[i]);
	printf("\n Size of data type 'int' is : %d bytes", int_size);
	printf("\n Number of elements in array 'intArr[]' : %d elements", intArr_n);
	printf("\n Size of array 'intArr[]' (%d elements * %d bytes) : %d bytes", intArr_n, int_size, intArr_size);
	
	printf("\n\n Inline initialization and displaying elements of array 'fArr[]' using loops");
	float_size = sizeof(float);
	fArr_size = sizeof(fArr);
	fArr_n = fArr_size / float_size;
	for(i = 0; i < fArr_n; i++)
		printf("\n fArr[%d] - element %d : %f", i, i + 1, fArr[i]);
	printf("\n Size of data type 'float' is : %d bytes", float_size);
	printf("\n Number of elements in array 'fArr[]' : %d elements", fArr_n);
	printf("\n Size of array 'fArr[]' (%d elements * %d bytes) : %d bytes", fArr_n, float_size, fArr_size);

	printf("\n\n Inline initialization and displaying elements of array 'charArr[]' using loops");
	char_size = sizeof(char);
	charArr_size = sizeof(charArr);
	charArr_n = charArr_size / char_size;
	for(i = 0; i < charArr_n; i++)
		printf("\n charArr[%d] - element %d : %c", i, i + 1, charArr[i]);
	printf("\n Size of data type 'char' is : %d bytes", char_size);
	printf("\n Number of elements in array 'charArr[]' : %d elements", charArr_n);
	printf("\n Size of array 'charArr[]' (%d elements * %d bytes) : %d bytes", charArr_n, char_size, charArr_size);

	printf("\n\n");
	return 0;
}