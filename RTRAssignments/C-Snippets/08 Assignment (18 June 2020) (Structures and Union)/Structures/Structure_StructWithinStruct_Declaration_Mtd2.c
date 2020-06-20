// To demonstrate structure inside structure - declaration Method 2
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>

struct point {
	int x;
	int y;
};
struct rectangle {
	struct point A, C;
};

int main() {
	// Variable declaration
	int length, breadth, area;
	struct rectangle rect;

	// Code
	printf("\n Enter the coordinates of point A (x1,y1) : ");
	scanf("%d %d", &rect.A.x, &rect.A.y);
	printf("\n Enter the coordinates of point C (x3,y3) : ");
	scanf("%d %d", &rect.C.x, &rect.C.y);
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