// To demonstrate structure inside structure - initialization Method 1
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>

struct rectangle {
	struct point {
		int x;
		int y;
	}A, C;
}rect = { {2,10},{8,26} };

int main() {
	// Variable declaration
	int length, breadth, area;

	// Code
	length = rect.C.y - rect.A.y;
	if(length < 0)
		length *= (-1);
	breadth = rect.C.x - rect.A.x;
	if(breadth < 0)
		breadth *= (-1);
	area = length * breadth;

	printf("\n Length of rectangle : %d", length);
	printf("\n Breadth of rectangle : %d", breadth);
	printf("\n Area of rectangle : %d", area);

	printf("\n\n");
	return 0;
}