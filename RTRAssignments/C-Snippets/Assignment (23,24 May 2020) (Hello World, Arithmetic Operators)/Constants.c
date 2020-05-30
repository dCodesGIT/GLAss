// Demonstration of constants in C

#include <stdio.h>

#define MY_PI 3.1415926535897932

#define AMC_STRING "AstroMediComp RTR 3.0"

//Un named enum
enum {
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
};

enum {
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

// Named enum
enum Numbers {
	ONE,
	TWO,
	THREE,
	FOUR=5,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN
};

enum boolean {
	TRUE =1,
	FALSE=0
};
int main() {
	// Local constants declarations
	const double epsilon = .000001;

	// Code
	printf("\n Local constant 'epsilon' = %lf", epsilon);

	printf("\n\n Sunday is day number = %d",SUNDAY);
	printf("\n Monday is day number = %d",MONDAY);
	printf("\n Tuesday is day number = %d",TUESDAY);
	printf("\n Wednesday is day number = %d",WEDNESDAY);
	printf("\n Thursday is day number = %d",THURSDAY);
	printf("\n Friday is day number = %d",FRIDAY);
	printf("\n Saturday is day number = %d",SATURDAY);

	printf("\n\n One is enum number %d.", ONE);
	printf("\n Two is enum number %d.", TWO);
	printf("\n Three is enum number %d.", THREE);
	printf("\n Four is enum number %d.", FOUR);
	printf("\n Five is enum number %d.", FIVE);
	printf("\n Six is enum number %d.", SIX);
	printf("\n Seven is enum number %d.", SEVEN);
	printf("\n Eight is enum number %d.", EIGHT);
	printf("\n Nine is enum number %d.", NINE);
	printf("\n Ten is enum number %d.", TEN);

	printf("\n\n January is month number %d", JANUARY);
	printf("\n February is month number %d", FEBRUARY);
	printf("\n March is month number %d", MARCH);
	printf("\n April is month number %d", APRIL);
	printf("\n May is month number %d", MAY);
	printf("\n June is month number %d", JUNE);
	printf("\n July is month number %d", JULY);
	printf("\n August is month number %d", AUGUST);
	printf("\n September is month number %d", SEPTEMBER);
	printf("\n October is month number %d", OCTOBER);
	printf("\n November is month number %d", NOVEMBER);
	printf("\n December is month number %d", DECEMBER);

	printf("\n\n Value of TRUE is %d", TRUE);
	printf("\n Value of FALSE is %d", FALSE);

	printf("\n\n MY_PI macro value = %.10lf", MY_PI);
	printf("\n Area of circle of radius 2 units = %f", (MY_PI * 2.0f * 2.0f));

	printf("\n\n AMC_STRING macro is : %s", AMC_STRING);

	printf("\n\n");
	return 0;
}
