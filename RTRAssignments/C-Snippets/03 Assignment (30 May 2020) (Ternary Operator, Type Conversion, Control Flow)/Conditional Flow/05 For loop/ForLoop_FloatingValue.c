// To demonstrate FOR loop - using floating values
// Date : 30 May 2020
//  By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	float f, f_num = 2.6f;

	// Code
	printf("\n Printing numbers from %f to %f...", f_num, (f_num * 10.0f));
	for(f = f_num; f <= (f_num * 10.0f); f = f + f_num)
		printf("\n %f", f);

	printf("\n\n");
	return 0;
}