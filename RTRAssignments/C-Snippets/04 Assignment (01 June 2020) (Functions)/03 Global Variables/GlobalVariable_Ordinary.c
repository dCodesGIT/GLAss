// To demonstrate global variable - Ordinary global variable
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int global_count = 0;		// Global variable
int main() {
	// Function declaration
	void changeCnt1(void);
	void changeCnt2(void);
	void changeCnt3(void);

	// Code
	printf("\n main() - Global_count value = %d", global_count);
	changeCnt1();
	changeCnt2();
	changeCnt3();

	printf("\n\n");
	return 0;
}

void changeCnt1(void) {
	// Code
	global_count = 100;
	printf("\n changeCnt1() - Global_count value = %d", global_count);
}

void changeCnt2(void) {
	// Code
	global_count = global_count + 1;
	printf("\n changeCnt2() - Global_count value = %d", global_count);
}

void changeCnt3(void) {
	// Code
	global_count += 10;
	printf("\n changeCnt3() - Global_count value = %d", global_count);
}