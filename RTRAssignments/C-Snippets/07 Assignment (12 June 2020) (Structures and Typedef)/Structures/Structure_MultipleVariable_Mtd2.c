// To demonstrate a structure - Method 2 - multiple variables
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>

// Global structure declaration
struct point {
	int x;
	int y;
};
struct point A, B, C, D, E;			// Global structure variable declaration

int main() {
	// Code
	A.x = 0;
	A.y = 0;
	B.x = 3;
	B.y = 0;
	C.x = 5;
	C.y = 3;
	D.x = 3;
	D.y = 5;
	E.x = 0;
	E.y = 3;

	printf("\n Coordinates (x,y) of point A is : (%d,%d)", A.x, A.y);
	printf("\n Coordinates (x,y) of point B is : (%d,%d)", B.x, B.y);
	printf("\n Coordinates (x,y) of point C is : (%d,%d)", C.x, C.y);
	printf("\n Coordinates (x,y) of point D is : (%d,%d)", D.x, D.y);
	printf("\n Coordinates (x,y) of point E is : (%d,%d)", E.x, E.y);

	printf("\n\n");
	return 0;
}