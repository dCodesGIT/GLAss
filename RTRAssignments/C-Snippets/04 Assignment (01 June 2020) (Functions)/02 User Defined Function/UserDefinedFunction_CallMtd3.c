// To demonstrate user defined function - call method 3
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Function declaration;
	void display_pledge(void);

	// Code
	display_pledge();

	printf("\n\n");
	return 0;
}

// Function definition
void func_preNameFormat(void) {
	// Code
	printf("\n My name is");
}

void func_Name(void) {
	// Function declaration
	void firstName(void);
	void middleName(void);
	void lastName(void);
	void func_preNameFormat(void);

	// Code
	func_preNameFormat();
	firstName();
	middleName();
	lastName();
}
void firstName(void) {
	printf(" DARSHAN");
}
void middleName(void) {
	printf(" JAYESH");
}
void lastName(void) {
	printf(" VIKAM");
}

void postNameFormat(void) {
	// Function declaration
	void func_Name(void);

	// Code
	func_Name();
	printf("\n from AstroMediComp attending RTR2020 batch...");
}

void display_pledge(void) {
	// Funtion declaration
	void postNameFormat(void);

	// Code
	postNameFormat();
	printf("\n\n India is my Country.");
	printf("\n All Indians are my brothers and sisters.");
	printf("\n I love my country.");
	printf("\n I am proud of its rich and vaired heritage....");
}