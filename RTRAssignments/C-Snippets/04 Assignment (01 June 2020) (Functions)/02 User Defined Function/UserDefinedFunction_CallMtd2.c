// To demonstrate user defined function - call method 2
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Function declaration;
	void display_info(void);
	void display_pledge(void);

	// Code
	display_info();
	display_pledge();

	printf("\n\n");
	return 0;
}

// Function definition
void display_info() {
	// Function declaration
	void func_preNameFormat(void);
	void func_Name(void);
	void func_postNameFormat(void);
	
	// Code
	func_preNameFormat();
	func_Name();
	func_postNameFormat();
}

void func_preNameFormat(void) {
	// Code
	printf("\n My name is");
}

void func_Name(void) {
	// Function declaration
	void firstName(void);
	void middleName(void);
	void lastName(void);

	// Code
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

void func_postNameFormat(void) {
	printf("\n from AstroMediComp attending RTR2020 batch...");
}

void display_pledge(void) {
	printf("\n\n India is my Country.");
	printf("\n All Indians are my brothers and sisters.");
	printf("\n I love my country.");
	printf("\n I am proud of its rich and vaired heritage....");
}