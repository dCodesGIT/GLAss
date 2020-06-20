// To demonstrate structure - Array of structure User Input
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>

#define NUM_EMPLOYEE 5
#define NAME_LENGTH 25
#define MARITAL_STATUS 10

struct Employee {
	char name[NAME_LENGTH];
	int age;
	char sex;
	float salary;
	char marital_status[MARITAL_STATUS];
};

int main() {
	// Function declaration
	void getstring(char[], int);

	// Variable declaration
	struct Employee EmployeeRecord[NUM_EMPLOYEE];
	int i;

	// Code
	for(i = 0; i < NUM_EMPLOYEE; i++) {
		printf("\n\n Data Entry for Employee number : %d", i + 1);
		printf("\n Name : ");
		getstring(EmployeeRecord[i].name, NAME_LENGTH);
		printf("\n Age (in years) : ");
		scanf("%d", &EmployeeRecord[i].age);
		printf(" Sex (M/F) : ");
		scanf(" %c", &EmployeeRecord[i].sex);
		printf(" Salary (in INR) : ");
		scanf("%f", &EmployeeRecord[i].salary);
		printf(" Is employee married (Y/N) : ");
		EmployeeRecord[i].marital_status[0] = getche();
	}

	printf("\n Displaying employee records...");
	for(i = 0; i < NUM_EMPLOYEE; i++) {
		printf("\n\n Employee Number : %d", i + 1);
		printf("\n Name : %s", EmployeeRecord[i].name);
		printf("\n Age : %d", EmployeeRecord[i].age);
		printf("\n Sex : ");
		if(EmployeeRecord[i].sex == 'M' || EmployeeRecord[i].sex == 'm')
			printf("Male");
		else
			printf("Female");
		printf("\n Salary : Rs %f", EmployeeRecord[i].salary);
		printf("\n Marital Status : ");
		if(EmployeeRecord[i].marital_status[0] == 'y' || EmployeeRecord[i].marital_status[0] == 'Y')
			printf("Married");
		else if(EmployeeRecord[i].marital_status[0] == 'n' || EmployeeRecord[i].marital_status[0] == 'N')
			printf("Unmarried");
		else
			printf("Invalid input");
	}

	printf("\n\n");
	return 0;
}

// Function definition
void getstring(char str[], int size) {
	// Variable declaration
	int i;
	char ch;

	// Code
	for(i = 0; i < size; i++)
		if((ch = getche()) != '\r')
			str[i] = ch;
		else
			break;
	if(i == size)
		str[i - 1] = '\0';
	else
		str[i] = '\0';
}