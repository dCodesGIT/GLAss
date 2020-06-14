// To demonstrate structure - User input multiple variables
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>

// Global structure declaration
struct point {
	int x, y;
};

int main() {
	// Local structure variable declaration
	struct point A, B, C, D, E;

	// Code
	printf("\n Enter X - coordinate of point A : ");
	scanf("%d", &A.x);
	printf("\n Enter Y - coordinate of point A : ");
	scanf("%d", &A.y);

	printf("\n Enter X - coordinate of point B : ");
	scanf("%d", &B.x);
	printf("\n Enter Y - coordinate of point B : ");
	scanf("%d", &B.y);

	printf("\n Enter X - coordinate of point C : ");
	scanf("%d", &C.x);
	printf("\n Enter Y - coordinate of point C : ");
	scanf("%d", &C.y);

	printf("\n Enter X - coordinate of point D : ");
	scanf("%d", &D.x);
	printf("\n Enter Y - coordinate of point D : ");
	scanf("%d", &D.y);

	printf("\n Enter X - coordinate of point E : ");
	scanf("%d", &E.x);
	printf("\n Enter Y - coordinate of point E : ");
	scanf("%d", &E.y);

	printf("\n Coordinates of point A(x,y) = (%d,%d)", A.x, A.y);
	printf("\n Coordinates of point B(x,y) = (%d,%d)", B.x, B.y);
	printf("\n Coordinates of point C(x,y) = (%d,%d)", C.x, C.y);
	printf("\n Coordinates of point D(x,y) = (%d,%d)", D.x, D.y);
	printf("\n Coordinates of point E(x,y) = (%d,%d)", E.x, E.y);

	printf("\n\n");
	return 0;
}