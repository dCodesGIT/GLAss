// To demonstrate 1 dimensional array - inline initialization and piece meal display
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int intArr[] = { 10,02,20,00,26,36 };
	int int_size, intArr_size, intArr_n;
	float fArr[] = { 10.02f,2.00f,26.08f,08.00f,36.10f };
	int float_size, fArr_size, fArr_n;
	char	charArr[] = { 'D','a','r','s','h','a','n','V','i','k','a','m','\0' };
	int char_size, charArr_size, charArr_n;

	// Code
	printf("\n\n Inline initialization and piece meal display of elements of array 'intArr[]' : ");
	printf("\n intArr[0] - 1st element : %d", intArr[0]);
	printf("\n intArr[1] - 2nd element : %d", intArr[1]);
	printf("\n intArr[2] - 3rd element : %d", intArr[2]);
	printf("\n intArr[3] - 4th element : %d", intArr[3]);
	printf("\n intArr[4] - 5th element : %d", intArr[4]);
	printf("\n intArr[5] - 6th element : %d", intArr[5]);
	int_size = sizeof(int);
	intArr_size = sizeof(intArr);
	intArr_n = intArr_size / int_size;
	printf("\n Size of data type 'int' is : %d bytes", int_size);
	printf("\n Number of elements in array 'intArr[]' : %d elements", intArr_n);
	printf("\n Size of array 'intArr[]' (%d elements * %d bytes) : %d bytes", intArr_n, int_size, intArr_size);

	printf("\n\n Inline initialization and piece meal display of elements of array 'fArr[]' : ");
	printf("\n fArr[0] - 1st element : %f", fArr[0]);
	printf("\n fArr[1] - 2nd element : %f", fArr[1]);
	printf("\n fArr[2] - 3rd element : %f", fArr[2]);
	printf("\n fArr[3] - 4th element : %f", fArr[3]);
	printf("\n fArr[4] - 5th element : %f", fArr[4]);
	float_size = sizeof(float);
	fArr_size = sizeof(fArr);
	fArr_n = fArr_size / float_size;
	printf("\n Size of data type 'float' is : %d bytes", float_size);
	printf("\n Number of elements in array 'fArr[]' : %d elements", fArr_n);
	printf("\n Size of array 'fArr[]' (%d elements * %d bytes) : %d bytes", fArr_n, float_size, fArr_size);

	printf("\n\n Inline initialization and piece meal display of elements of array 'charArr[]' : ");
	printf("\n charArr[0] - 1st element : %c", charArr[0]);
	printf("\n charArr[1] - 2nd element : %c", charArr[1]);
	printf("\n charArr[2] - 3rd element : %c", charArr[2]);
	printf("\n charArr[3] - 4th element : %c", charArr[3]);
	printf("\n charArr[4] - 5th element : %c", charArr[4]);
	printf("\n charArr[5] - 6th element : %c", charArr[5]);
	printf("\n charArr[6] - 7th element : %c", charArr[6]);
	printf("\n charArr[7] - 8th element : %c", charArr[7]);
	printf("\n charArr[8] - 9th element : %c", charArr[8]);
	printf("\n charArr[9] - 10th element : %c", charArr[9]);
	printf("\n charArr[10] - 11th element : %c", charArr[10]);
	printf("\n charArr[11] - 12th element : %c", charArr[11]);
	char_size = sizeof(char);
	charArr_size = sizeof(charArr);
	charArr_n = charArr_size / char_size;
	printf("\n Size of data type 'char' is : %d bytes", char_size);
	printf("\n Number of elements in array 'charArr[]' : %d elements", charArr_n);
	printf("\n Size of array 'charArr[]' (%d elements * %d bytes) : %d bytes", charArr_n, char_size, charArr_size);

	printf("\n\n");
	return 0;
}