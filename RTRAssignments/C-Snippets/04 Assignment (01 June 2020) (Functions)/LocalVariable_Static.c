// To demonstrate LOCAL VARIABLE - static local variable
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Function declaration
	void change_count(void);

	// Variable declaration
	int a = 5;

	// Code
	printf("\n A = %d", a);
	change_count();
	change_count();
	change_count();

	printf("\n\n");
	return 0;
}

// Function definition
void change_count(void) {
	// Variable declaration - static
	static int local_count = 0;

	// Code
	local_count += 1;
	printf("\n\n Local count = %d", local_count);
}