// To demonstrate a structure - Method 2 - multiple structures
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>

// Global structure declaration
struct coordinates {
	int x, y;
};

struct coordinateProperties {
	int quadrant;
	char axis_location[10];
};

// Global structure variable declaration
struct coordinates point;
struct coordinateProperties pointProperties;

int main() {
	// Code
	printf("\n Enter X - coordinate of point A : ");
	scanf("%d", &point.x);
	printf("\n Enter Y - coordinate of point A : ");
	scanf("%d", &point.y);

	printf("\n The point (%d,%d) ", point.x, point.y);
	if(point.x == 0 && point.y == 0)
		printf("is the origin.", point.x, point.y);
	else {
		if(point.x == 0) {
			if(point.y < 0)
				strcpy(pointProperties.axis_location, "Negative Y");
			else
				strcpy(pointProperties.axis_location, "Positive Y");
			printf("lies on %s axis", pointProperties.axis_location);
		}
		else if(point.y == 0) {
			if(point.x < 0)
				strcpy(pointProperties.axis_location, "Negative X");
			else
				strcpy(pointProperties.axis_location, "Positive X");
			printf("lies on %s axis", pointProperties.axis_location);
		}
		else {
			pointProperties.axis_location[0] = '\0';
			if(point.x > 0 && point.y > 0)
				pointProperties.quadrant = 1;
			else if(point.x < 0 && point.y > 0)
				pointProperties.quadrant = 2;
			else if(point.x < 0 && point.y < 0)
				pointProperties.quadrant = 3;
			else
				pointProperties.quadrant = 4;
			printf("lies in %d quadrant", pointProperties.quadrant);
		}
	}

	printf("\n\n");
	return 0;
}